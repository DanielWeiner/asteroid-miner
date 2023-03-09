#include "sprite.h"
#include "spriteBuffer.h"
#include "spriteSheet.h"
#include "time.h"

#include <glm/ext.hpp>

#include <tuple>
#include <array>
#include <cstddef>

Sprite::Sprite(
    std::string spriteName, 
    unsigned int id, 
    SpriteSheet& spriteSheet, 
    SpriteBuffer& spriteBuffer
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
    _states[_buffer.getStep()] = sprite._states[_buffer.getStep()];
    _states[_buffer.getNextStep()] = sprite._states[_buffer.getNextStep()];

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

glm::vec2 Sprite::getBaseSize()
{
    return _sheet.getSize(_name.c_str());
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

    glm::mat4* modelMatrix = _buffer.getModelMatrix(_id);

    *modelMatrix = glm::mat4(1.0);
    
    *modelMatrix = glm::translate(*modelMatrix, glm::vec3(state->_x, state->_y, 0.0f));

    *modelMatrix = glm::translate(*modelMatrix, glm::vec3(0.5f * state->_width, 0.5f * state->_height, 0.0f));
    *modelMatrix = glm::rotate(*modelMatrix, state->_rotate, glm::vec3(0.0f, 0.0f, 1.0f));

    *modelMatrix = glm::translate(*modelMatrix, glm::vec3(-0.5f * state->_width, -0.5f * state->_height, 0.0f));

    *modelMatrix = glm::scale(*modelMatrix, glm::vec3(state->_width, state->_height, 1.0f));
}

void Sprite::updateTextureIndex() 
{
    _buffer.setTexture(_id, _sheet.getSpriteIndex(_name.c_str()));   
}

bool Sprite::pointIsInHitbox(float x, float y)
{
    glm::vec2 pos = _lastModel / glm::vec4(x, y, 0.0, 1.0);

    if (!(pos.x < 1.f && pos.x >= 0.f && pos.y < 1.f && pos.y >= 0.f)) {
        return false;
    }

    return _sheet.pixelAt(_name.c_str(), pos.x, pos.y).a;
}

void Sprite::moveToEndOfBuffer() 
{
    _buffer.moveToEnd(_id);
}

void Sprite::init() 
{
    if (!_initialized) {
        _initialized = true;
        _states[_buffer.getStep()] = _states[_buffer.getNextStep()];
    }
}

Sprite::~Sprite() 
{
    _buffer.destroyResource(_id);
}

void Sprite::_useNextState(SpriteState*& state)
{
    _update();
    state = &_states[(_lastStep + 1) % 2];
}

void Sprite::_useCurrentState(SpriteState*& state)
{
    _update();
    state = &_states[_lastStep];
}

void Sprite::_update(){
    if (_lastStep != _buffer.getStep()) {
        _states[_buffer.getNextStep()] = _states[_buffer.getStep()];
    }
    _lastStep = _buffer.getStep();
}