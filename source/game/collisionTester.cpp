#include "collisionTester.h"
#include "../util/range.h"
#include "../time.h"
#include "constants.h"

#include <iostream>

#include <glm/glm.hpp>

namespace {
    const bool DEBUG_DRAW = false;
}

namespace {
    const double CLEAR_BUTTON_FONT_SCALE = 2.25;
    int prevButtons = 0;
    long long prevTime = Time::timestamp();
    /**
     * Determine if the button was pressed or released based on the previous and current state of the buttons.
     * 
     * @param prev The previous state of the buttons
     * @param current The current state of the buttons
     * @param button The button to check
     * @param action The action to check. 1 for pressed, 0 for released
     * 
     * @return true if the button was pressed or released, false otherwise
     * */ 
    bool isNewButtonAction(int prev, int current, int button, int action) {
        return 1 << button & (prev ^ current) & (prev ^ -action);
    }

    std::string getFps() 
    {
        static double frameCount = 0;
        static double lastFps = 0;
        static long long lastTime = Time::timestamp(); // last time in milliseconds

        const int sampleTimeMs = 100;
        frameCount++;
        long long currentTime = Time::timestamp();
        if (currentTime - lastTime >= sampleTimeMs) {
            lastFps = frameCount / (double)(currentTime - lastTime) * 1000;
            frameCount = 0;
            lastTime = currentTime;
        }
        return std::to_string((int)lastFps);
    }
}

CollisionTester::CollisionTester(
    const Window& window, 
    SpriteFactory& spriteFactory
) : 
_window(window), 
_spriteRenderers{ 
    std::unique_ptr<SpriteRenderer>(spriteFactory.createRenderer(_world, &_spriteEventListener)), 
    std::unique_ptr<SpriteRenderer>(spriteFactory.createRenderer(_world, &_spriteEventListener)) 
},
_spriteRenderer(*_spriteRenderers[0]),
_uiSpriteRenderer(*_spriteRenderers[1])
{
}

void CollisionTester::handleEvent(const Event& event)
{
    if (event.type == EventType::MOUSE) {
        _handleMouseEvent(event);
        prevButtons = event.button;
    }
}

void CollisionTester::render() 
{
    _spritePhysicsEngine.update();

    _textRenderer.setProjection(_uiScene.getProjection());
    _uiSpriteRenderer.setProjection(_uiScene.getProjection());
    _uiSpriteRenderer.setView(_uiScene.getView());
    _uiSpriteRenderer.draw();

    _clearButtonText.render();

    _spriteRenderer.setProjection(_scene.getProjection());
    _spriteRenderer.setView(_scene.getView());
    _spriteRenderer.draw();
    
    _fpsCounter.setText(getFps());
    _fpsCounter.render();

    if (DEBUG_DRAW) {
        _lineRenderer.setProjection(_scene.getProjection());
        _lineRenderer.setView(_scene.getView());

        for (auto& sprite : _droppedSprites) {
            sprite->debugDraw(&_lineRenderer);
        }

        if (_selectedSpriteDragging) {
            _selectedSpriteDragging->debugDraw(&_lineRenderer);
        }
        if (_selectedSpriteRotating) {
            _selectedSpriteRotating->debugDraw(&_lineRenderer);
        }
    }
}

void CollisionTester::init() 
{
    _textRenderer.init("Teorema");
    _lineRenderer.init();

    _addPanel({ 100, 100 }, { 150, 150 }, "playerShip1_blue.png");
    _addPanel({ 100, 300 }, { 150, 150 }, "playerShip2_orange.png");
    _addPanel({ 100, 500 }, { 150, 150 }, "playerShip3_red.png");

    _createClearButton({ 75, 750 }, { 200, 75 }, "Clear");

    _fpsCounter.setFontScale(2);
    _fpsCounter.setBounds({ 10, 10 }, { 1000, 1000 });
    _fpsCounter.setColor({ 1, 0.9, 0.1, 1 });

    prevTime = Time::timestamp();
}

void CollisionTester::_addPanel(glm::vec2 location, glm::vec2 size, std::string icon) 
{
    Panel* panelPtr = new Panel(_uiSpriteRenderer, _window);
    _panels.push_back(std::unique_ptr<Panel>(panelPtr));
    Panel& panel = *panelPtr;

    Sprite* spritePtr = _uiSpriteRenderer.createSprite(icon, true, false);
    _panelSprites.push_back(std::unique_ptr<Sprite>(spritePtr));
    Sprite& sprite = *spritePtr;

    sprite.switchToStatic();

    panel.setBounds(location, location + size);
    glm::vec2 innerTopLeft, innerBottomRight;
    panel.getInnerBounds(&innerTopLeft, &innerBottomRight);

    glm::vec2 panelSize(innerBottomRight - innerTopLeft);

    glm::vec2 scaleFactor = panelSize / sprite.getBaseSize();

    if (sprite.getBaseSize().x < sprite.getBaseSize().y) {
        auto scale = scaleFactor.y * glm::vec2(1.f);
        auto size = sprite.getBaseSize() * scale;

        sprite.setScale(scaleFactor.y * glm::vec2(1.f));
        sprite.moveTo(((panelSize - size) / 2.f).x + innerTopLeft.x, innerTopLeft.y);
    } else {
        auto scale = scaleFactor.x * glm::vec2(1.f);
        auto size = sprite.getBaseSize() * scale;

        sprite.setScale(scaleFactor.x * glm::vec2(1.f));
        sprite.moveTo(innerTopLeft.x, ((panelSize - size) / 2.f).y + innerTopLeft.y);
    }
}

std::optional<std::size_t> CollisionTester::_getSelectedSpriteVariation(glm::vec2 location)
{
    for (auto i : Util::Range(0, _panels.size())) {
        glm::vec2 panelTopLeft, panelBottomRight;

        _panels[i]->getOuterBounds(&panelTopLeft, &panelBottomRight);
        if (location.x >= panelTopLeft.x && location.y >= panelTopLeft.y && location.x < panelBottomRight.x && location.y < panelBottomRight.y) {
            return i;
        }
    }

    return std::nullopt;
}

void CollisionTester::_createClearButton(glm::vec2 location, glm::vec2 size, std::string text) 
{
    _clearButton.reset(new Panel(_uiSpriteRenderer, _window));

    _clearButton->setBounds(location, location + size);
    glm::vec2 innerTopLeft, innerBottomRight;
    _clearButton->getOuterBounds(&innerTopLeft, &innerBottomRight);

    _clearButtonText.setAlignment(TextBox::ALIGN::CENTER);
    _clearButtonText.setFontScale(CLEAR_BUTTON_FONT_SCALE);
    _clearButtonText.setBounds(innerTopLeft, innerBottomRight);
    _clearButtonText.setColor({ 0.2, 0.2, 0.2, 1 });
    _clearButtonText.setText(text);

    auto textHeight = _clearButtonText.getTextSize().y;
    auto panelHeight = innerBottomRight.y - innerTopLeft.y;

    auto topTextPadding = (panelHeight - textHeight) / 2.0;
    auto offset = 4.0;

    // reposition text to be centered vertically
    _clearButtonText.setBounds({
        innerTopLeft.x,
        innerTopLeft.y + topTextPadding + offset
    }, { 
        innerBottomRight.x,
        innerBottomRight.y - topTextPadding + offset
    });
}

void CollisionTester::_handleMouseEvent(const Event &event) 
{
    switch (event.action) {
        case EventAction::PRESS:
            return _handleMousePress(event);
        case EventAction::RELEASE:
            return _handleMouseRelease(event);
        case EventAction::HOVER:
            return _handleMouseMove(event);
        case EventAction::SCROLL:
            return _handleMouseScroll(event);
        default:
            return;
    }
}

void CollisionTester::_handleMouseScroll(const Event& event)
{
    if (event.mods & KeyModifier::KEY_MOD_SHIFT) {
        _scene.zoomIn(event.yoffset * 10);
        return;
    }

    // if hovering over a sprite, enlarge or shrink it
    auto worldCoords = _scene.toWorldCoordinates({ event.x, event.y });
    bool foundSprite = _selectedSpriteDragging != nullptr;
    bool spriteSelected = foundSprite;
    
    if (_selectedSpriteDragging) {
        spriteSelected = false;
        foundSprite = true;
    } else if(!_isAnySpriteSelected()) {
        _trySelectSprite(worldCoords, _selectedSpriteDragging);
        if (_selectedSpriteDragging) {
            spriteSelected = true;
            foundSprite = true;
        }
    }
    if (foundSprite) {
        auto size = _selectedSpriteDragging->getSize() / _selectedSpriteDragging->getBaseSize();
        auto newSize = size + ((float)event.yoffset * -0.1f * size);

        _selectedSpriteDragging->setScale(newSize);
        _selectedSpriteDragging->moveTo(worldCoords - newSize * _selectedSpriteDragging->getBaseSize() / 2.0f);
    }
    if (spriteSelected) {
        _putSpriteBack(_selectedSpriteDragging);
    }
}

void CollisionTester::_handleMouseRelease(const Event& event) 
{
    for (int i{MouseButton::BUTTON_0}; i <= MouseButton::BUTTON_LAST; i++) {
        if (isNewButtonAction(prevButtons, event.button, i, 0)) {
           _handleMouseRelease(event, i);
        }
    }
}

void CollisionTester::_handleMousePress(const Event& event)
{
    // For each mouse button, use isNewButtonAction to see if the button has been newly pressed.
    for (int i{MouseButton::BUTTON_0}; i <= MouseButton::BUTTON_LAST; i++) {
        if (isNewButtonAction(prevButtons, event.button, i, 1)) {
           _handleMousePress(event, i);
        }
    }
}

void CollisionTester::_handleMousePress(const Event& event, int button)
{
    auto uiWorldCoords = _uiScene.toWorldCoordinates({ event.x, event.y });
    auto worldCoords = _scene.toWorldCoordinates({ event.x, event.y });

    if (button == MouseButton::BUTTON_LEFT) {
        if (!_selectedPanelIndex.has_value()) {
            _selectedPanelIndex = _getSelectedSpriteVariation(uiWorldCoords);
            if (_selectedPanelIndex.has_value()) {
                return;
            }
        }

        // if we didn't click on a panel, check if we clicked on the clear button
        glm::vec2 topLeft, bottomRight;
        _clearButton->getOuterBounds(&topLeft, &bottomRight);
        if (uiWorldCoords.x >= topLeft.x && uiWorldCoords.y >= topLeft.y && uiWorldCoords.x < bottomRight.x && uiWorldCoords.y < bottomRight.y) {
            _droppedSprites.clear();
            _selectedSpriteDragging.reset();
            _selectedSpriteRotating.reset();
            return;
        }
    }

    if (_isAnySpriteSelected()) {
        return;
    }

    if (button == MouseButton::BUTTON_LEFT) {
        _trySelectSprite(worldCoords, _selectedSpriteDragging);
    }
    else if (button == MouseButton::BUTTON_RIGHT) {
        _trySelectSprite(worldCoords, _selectedSpriteRotating);
    }
}

void CollisionTester::_trySelectSprite(const glm::vec2& worldCoords, std::unique_ptr<Sprite>& spritePtr)
{
    if (spritePtr) {
        return;
    }

    for (auto it{_droppedSprites.rbegin() }; it != _droppedSprites.rend(); ++it) {
        auto& sprite = *it;

        if (sprite->pointIsInHitbox(worldCoords.x, worldCoords.y)) {
            spritePtr = std::move(sprite);
            _droppedSprites.erase(std::next(it).base());
            return;
        }
    }
}

void CollisionTester::_handleMouseRelease(const Event& event, int button)
{
    if (button == MouseButton::BUTTON_LEFT) {
        _selectedPanelIndex = std::nullopt;
        _putSpriteBack(_selectedSpriteDragging);
    }
    else if (button == MouseButton::BUTTON_RIGHT) {
        _putSpriteBack(_selectedSpriteRotating);
    }
}

void CollisionTester::_putSpriteBack(std::unique_ptr<Sprite>& spritePtr)
{
    if (!spritePtr) {
        return;
    }

    // insert the sprite back into the list of dropped sprites, by order of id
    auto it = std::upper_bound(_droppedSprites.begin(), _droppedSprites.end(), spritePtr, [](const auto& a, const auto& b) {
        return a->id() < b->id();
    });

    _droppedSprites.insert(it, std::move(spritePtr));
    spritePtr = nullptr;
}

bool CollisionTester::_isAnySpriteSelected()
{
    return _selectedSpriteDragging || _selectedSpriteRotating;
}

void CollisionTester::_handleMouseMove(const Event& event) 
{
    auto worldCoords = _scene.toWorldCoordinates({ event.x, event.y });
    auto uiWorldCoords = _uiScene.toWorldCoordinates({ event.x, event.y });

    std::vector<std::reference_wrapper<Panel>> panels{ std::ref(*_clearButton) };
    for (auto& panel : _panels) {
        panels.push_back(std::ref(*panel));
    }
    
    _window.setCursor(Window::Cursor::ARROW);
    for (auto& panel : panels) {
        glm::vec2 topLeft, bottomRight;
        panel.get().getOuterBounds(&topLeft, &bottomRight);
        if (uiWorldCoords.x >= topLeft.x && uiWorldCoords.y >= topLeft.y && uiWorldCoords.x < bottomRight.x && uiWorldCoords.y < bottomRight.y) {
            _window.setCursor(Window::Cursor::HAND);
            break;
        }
    }

    if (_selectedPanelIndex.has_value()) {
        if (!_selectedSpriteDragging) {
            _selectedSpriteDragging.reset(_spriteRenderer.createSprite(_panelSprites[*_selectedPanelIndex]->getName()));
        }
        _selectedPanelIndex = std::nullopt;
    }

    if (_selectedSpriteDragging) {
        _selectedSpriteDragging->moveTo(worldCoords - _selectedSpriteDragging->getSize() / 2.f);
        return;
    }

    if (_selectedSpriteRotating) {
        auto spriteCenter = _selectedSpriteRotating->getCenter();
        auto angle = glm::atan(worldCoords.y - spriteCenter.y, worldCoords.x - spriteCenter.x) + HALF_PI;
        _selectedSpriteRotating->rotateTo(angle);
        return;
    }
}
