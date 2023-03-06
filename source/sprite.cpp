#include "sprite.h"
#include "spriteBuffer.h"
#include "spriteSheet.h"
#include "time.h"

#include <glm/ext.hpp>

#include <tuple>
#include <array>
#include <cstddef>

#include <iostream>
namespace {
    const std::array<const glm::ivec2, 8> directions{{
        { -1,  0 }, // w
        { -1,  1 }, // sw
        {  0,  1 }, // s
        {  1,  1 }, // se
        {  1,  0 }, // e
        {  1, -1 }, // ne
        {  0, -1 }, // n
        { -1, -1 }  // nw
    }};
    const std::array<const glm::ivec2, 4> cardinalDirections{{
        { -1,  0 }, // w
        {  0,  1 }, // s
        {  1,  0 }, // e
        {  0, -1 }  // n
    }};
}

Sprite::Sprite(
    std::string spriteName, 
    unsigned int id, 
    std::shared_ptr<SpriteSheet>    spriteSheet, 
    std::shared_ptr<SpriteBuffer>   spriteBuffer
) : 
    _name(spriteName), 
    _id(id), 
    _sheet(spriteSheet),
    _buffer(spriteBuffer)
{}

unsigned int Sprite::id() const
{
    return _id;
}

Sprite& Sprite::operator=(const Sprite& sprite)
{
    _name = sprite._name;
    _states[_buffer->getStep()] = sprite._states[_buffer->getStep()];
    _states[_buffer->getNextStep()] = sprite._states[_buffer->getNextStep()];

    _textureUpdated = true;

    _buffer = sprite._buffer;
    _sheet = sprite._sheet;

    updateModelMatrix();
    updateTextureIndex();

    return *this;
}

void Sprite::moveTo(float x, float y) 
{
    SpriteState* nextState;
    _useNextState(nextState);
    return move(x - nextState->_x, y - nextState->_y);
}

void Sprite::moveTo(glm::vec2 xy) 
{
    return moveTo(xy.x, xy.y);
}

void Sprite::move(float x, float y) 
{
    SpriteState* nextState;
    _useNextState(nextState);
    nextState->_x = nextState->_x + x;
    nextState->_y = nextState->_y + y;

    updateModelMatrix();
}

void Sprite::move(glm::vec2 xy) 
{
    return move(xy.x, xy.y);
}

void Sprite::rotate(float radians) 
{
    SpriteState* nextState;
    _useNextState(nextState);
    nextState->_rotate = nextState->_rotate + radians;

    updateModelMatrix();
}

void Sprite::rotateTo(float radians) 
{
    SpriteState* nextState;
    _useNextState(nextState);
    return rotate(radians - nextState->_rotate);
}

void Sprite::scaleBy(float x, float y) 
{
    SpriteState* nextState;
    _useNextState(nextState);
    nextState->_width =  nextState->_width * x;
    nextState->_height = nextState->_height * y;

    updateModelMatrix();
}

void Sprite::scaleBy(glm::vec2 xy) 
{
    return scaleBy(xy.x, xy.y);
}

void Sprite::setScale(float x, float y)
{
    SpriteState* nextState;
    _useNextState(nextState);
    nextState->_width = x;
    nextState->_height = y;

    updateModelMatrix();
}

void Sprite::setScale(glm::vec2 xy)
{
    setScale(xy.x, xy.y);
}

void Sprite::calculateEdgeTangents() 
{
    struct PixelDirection
    {
        glm::ivec2 location;
        glm::ivec2 direction;
    };
    struct PixelDirectionRun
    {
        glm::ivec2  location;
        glm::ivec2  direction;
        std::size_t length;
    };

    auto now = Time::timestamp();
    auto firstPixel = _findFirstPixel();
    
    int currentX = firstPixel.x;
    int currentY = firstPixel.y;
    glm::ivec2 direction(-1, -1);
    std::vector<PixelDirection> pixels;

    do {
        if (!_findNeighborEdgePixel(&currentX, &currentY, &direction)) {
            std::cout << "edge detection failed\n";
            return;
        }
        pixels.push_back({
            .location{currentX, currentY},
            .direction{direction}
        });
    } while (currentX != firstPixel.x || currentY != firstPixel.y);
    auto elapsed = Time::timestamp() - now;

    while (pixels.front().direction == pixels.back().direction) {
        pixels.insert(pixels.begin(), 1, pixels.back());
        pixels.pop_back();
    }
    
    std::vector<PixelDirectionRun> runs;
    glm::ivec2 currentDirection = pixels.front().direction;
    glm::ivec2 runStart = pixels.front().location;
    std::size_t currentRun = 0;
    for (auto [location, direction] : pixels) {
        if (direction == currentDirection) {
            currentRun++;
            continue;
        }
        runs.push_back({
            .location{runStart},
            .direction{currentDirection},
            .length = currentRun
        });

        std::cout << currentRun << ", " << (currentDirection.x + 1) * 3 + (currentDirection.y + 1) << "\n";
        currentDirection = direction;
        runStart = location;
        currentRun = 1;
    }

    runs.push_back({
        .location{runStart},
        .direction{currentDirection},
        .length = currentRun
    });
    
    std::cout << elapsed << "ms" << "\n";
}

glm::vec2 Sprite::getBaseSize()
{
    return _sheet->getSize(_name.c_str());
}

std::string Sprite::getName()
{
    return _name;
}

glm::vec2 Sprite::getPosition()
{
    SpriteState* state;
    _useCurrentState(state);
    return glm::vec2(state->_x, state->_y);
}

glm::vec2 Sprite::getCenter()
{
    SpriteState* state;
    _useCurrentState(state);
    
    return glm::vec2(state->_x, state->_y) + glm::vec2(state->_width, state->_height) / 2.f;
}

glm::vec2 Sprite::getSize()
{
    SpriteState* state;
    _useCurrentState(state);
    return glm::vec2(state->_width, state->_height);
}

float Sprite::getRotation()
{
    SpriteState* state;
    _useCurrentState(state);
    return state->_rotate;
}

glm::vec2 Sprite::getNextPosition()
{
    SpriteState* state;
    _useNextState(state);
    return glm::vec2(state->_x, state->_y);
}

void Sprite::updateModelMatrix()
{
    SpriteState* state;
    _useNextState(state);

    _buffer->initializeModel(_id, glm::mat4(1.0f));
    _buffer->translateModel(_id, glm::vec3(state->_x, state->_y, 0.0f));

    _buffer->translateModel(_id, glm::vec3(0.5f * state->_width, 0.5f * state->_height, 0.0f));
    _buffer->rotateModel(_id, state->_rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    _buffer->translateModel(_id, glm::vec3(-0.5f * state->_width, -0.5f * state->_height, 0.0f));

    _buffer->scaleModel(_id, glm::vec3(state->_width, state->_height, 1.0f));

    _lastModel = _buffer->getModelMatrix(_id);
}

void Sprite::updateTextureIndex() 
{
    _buffer->setTexture(_id, _sheet->getSpriteIndex(_name.c_str()));   
}

bool Sprite::pointIsInHitbox(float x, float y)
{
    glm::vec2 pos = _lastModel / glm::vec4(x, y, 0.0, 1.0);

    if (!(pos.x < 1.f && pos.x >= 0.f && pos.y < 1.f && pos.y >= 0.f)) {
        return false;
    }

    return _sheet->pixelAt(_name.c_str(), pos.x, pos.y).a;
}

void Sprite::moveToEndOfBuffer() 
{
    _buffer->moveToEnd(_id);
}

void Sprite::init() 
{
    if (!_initialized) {
        _initialized = true;
        _states[_buffer->getStep()] = _states[_buffer->getNextStep()];
    }
}

Sprite::~Sprite() 
{
    if (_buffer) {
        _buffer->destroyResource(_id);
    }
}

void Sprite::_useNextState(SpriteState*& state)
{
    _update();
    state = &_states[_buffer->getNextStep()];
}

void Sprite::_useCurrentState(SpriteState*& state)
{
    _update();
    state = &_states[_buffer->getStep()];
}

void Sprite::_update(){
    if (_lastStep != _buffer->getStep()) {
        _states[_buffer->getNextStep()] = _states[_buffer->getStep()];
    }
    _lastStep = _buffer->getStep();
}

bool Sprite::_isPixelAtEdge(int x, int y)
{
    const char* name = _name.c_str();
    if (_sheet->pixelAt(name, x, y).a == 0) {
        return false;
    }

    for (auto direction : directions) {
        if (_sheet->pixelAt(name, x + direction.x, y + direction.y).a == 0) {
            return true;
        }
    }

    return false;
}

bool Sprite::_findNeighborEdgePixel(int* x, int* y, glm::ivec2* direction)
{
    int currentX = *x;
    int currentY = *y;

    for (int i = 0; i < cardinalDirections.size(); i++) {
        auto dir = cardinalDirections[i];
        if (*direction == -dir) {
            continue;
        }

        if (currentX + dir.x < 0 || currentY + dir.y < 0) {
            continue;
        }
        
        if (_isPixelAtEdge(currentX + dir.x, currentY + dir.y)) {
            *direction = dir;
            *x += dir.x;
            *y += dir.y;
            return true;
        }
    }

    return false;
}

glm::vec2 Sprite::_findFirstPixel()
{
    const char* nameStr = _name.c_str();
    // find a starting point
    glm::vec2 size = _sheet->getSize(nameStr);

    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            auto pixel = _sheet->pixelAt(nameStr, x, y);
            if (pixel.a > 0) {
                return glm::vec2(x, y);
            }
        }
    }

    return glm::vec2(-1, -1);
}
