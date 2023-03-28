#include "collisionTester.h"
#include "../util/range.h"
#include "../time.h"

#include <iostream>

#include <glm/glm.hpp>

namespace {
    const double CLEAR_BUTTON_FONT_SCALE = 2;
    double frameCount = 0;
    double lastFps = 0;
    long long lastTime = Time::timestamp(); // last time in milliseconds

    std::string getFps() 
    {
        int sampleTimeMs = 100;
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
    SpriteRenderer& spriteRenderer,
    SpriteRenderer& uiSpriteRenderer
) : 
_window(window), 
_spriteRenderer(spriteRenderer),
_uiSpriteRenderer(uiSpriteRenderer)
{
}

void CollisionTester::handleEvent(const Event& event)
{
    if (event.type == EventType::MOUSE) {
        _handleMouseEvent(event);
    }
}

void CollisionTester::render() 
{
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
}

void CollisionTester::init() 
{
    _textRenderer.init("Teorema");

    _addPanel({ 100, 100 }, { 150, 150 }, "playerShip1_blue.png");
    _addPanel({ 100, 300 }, { 150, 150 }, "playerShip2_orange.png");
    _addPanel({ 100, 500 }, { 150, 150 }, "playerShip3_red.png");

    _createClearButton({ 75, 750 }, { 200, 75 }, "Clear");

    _fpsCounter.setFontScale(2);
    _fpsCounter.setBounds({ 10, 10 }, { 1000, 1000 });
    // make it a pleasing yellow
    _fpsCounter.setColor({ 1, 0.9, 0.1, 1 });
}

void CollisionTester::_addPanel(glm::vec2 location, glm::vec2 size, std::string icon) 
{
    Panel* panelPtr = new Panel(_uiSpriteRenderer, _window);
    _panels.push_back(std::unique_ptr<Panel>(panelPtr));
    Panel& panel = *panelPtr;

    Sprite* spritePtr = _uiSpriteRenderer.createSprite(icon);
    _panelSprites.push_back(std::unique_ptr<Sprite>(spritePtr));
    Sprite& sprite = *spritePtr;

    panel.setBounds(location, location + size);
    glm::vec2 innerTopLeft, innerBottomRight;
    panel.getInnerBounds(&innerTopLeft, &innerBottomRight);

    glm::vec2 panelSize(innerBottomRight - innerTopLeft);

    glm::vec2 scaleFactor = panelSize / sprite.getBaseSize();

    if (panelSize.x < panelSize.y) {
        glm::vec2 scaledSize = sprite.getBaseSize() * scaleFactor.y;
        sprite.setScale(scaledSize);
        sprite.moveTo(innerTopLeft + glm::vec2((panelSize.x - scaledSize.x) / 2.0, 0));
    } else {
        glm::vec2 scaledSize = sprite.getBaseSize() * scaleFactor.x;
        sprite.setScale(scaledSize);
        sprite.moveTo(innerTopLeft + glm::vec2(0, (panelSize.y - scaledSize.y) / 2.0));
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
    _clearButtonText.setColor({ 0, 0, 0, 1 });
    _clearButtonText.setText(text);

    auto textHeight = _clearButtonText.getTextSize().y;
    auto panelHeight = innerBottomRight.y - innerTopLeft.y;

    auto topTextPadding = (panelHeight - textHeight) / 2.0;
    auto offset = 4.0;

    // reposition text to be centered vertically
    _clearButtonText.setBounds({
        innerTopLeft.x,
        innerTopLeft.y + topTextPadding + offset
    }, 
    { 
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
    // if hovering over a sprite, enlarge or shrink it
    auto worldCoords = _uiScene.toWorldCoordinates({ event.x, event.y });

    for (auto i : Util::Range((int)_droppedSprites.size() - 1, -1)) {
        auto position = _droppedSprites[i]->getPosition();
        auto size = _droppedSprites[i]->getSize();

        if (worldCoords.x >= position.x && worldCoords.y >= position.y && worldCoords.x < position.x + size.x && worldCoords.y < position.y + size.y) {
            auto newSize = size + ((float)event.yoffset * -0.1f * _droppedSprites[i]->getSize());
            _droppedSprites[i]->setScale(newSize);
            _droppedSprites[i]->moveTo(_droppedSprites[i]->getPosition() + (size - newSize) / 2.0f);
            return;
        }
    }
}

void CollisionTester::_handleMouseRelease(const Event& event) 
{
if (event.isButtonPressed(MouseButton::BUTTON_LEFT) || !(_selectedSprite || _selectedPanelIndex.has_value())) {
        return;
    }
    _selectedPanelIndex = std::nullopt;
    if (!_selectedSprite) {
        return;
    }

    _droppedSprites.push_back(std::move(_selectedSprite));
    // sort the sprites by id so that the last one added is on top
    std::sort(_droppedSprites.begin(), _droppedSprites.end(), [](const auto& a, const auto& b) {
        return a->id() < b->id();
    });
    _selectedSprite = nullptr;
}

void CollisionTester::_handleMousePress(const Event& event)
{
    if (!event.isButtonPressed(MouseButton::BUTTON_LEFT) || _selectedPanelIndex.has_value()) {
        return;
    }

    _selectedPanelIndex = _getSelectedSpriteVariation(_uiScene.toWorldCoordinates({ event.x, event.y }));
    if (_selectedPanelIndex.has_value()) {
        return;
    }

    // if we didn't click on a panel, check if we clicked on the clear button
    glm::vec2 topLeft, bottomRight;
    _clearButton->getOuterBounds(&topLeft, &bottomRight);
    if (event.x >= topLeft.x && event.y >= topLeft.y && event.x < bottomRight.x && event.y < bottomRight.y) {
        _droppedSprites.clear();

        return;
    }


    auto worldCoords = _uiScene.toWorldCoordinates({ event.x, event.y });
    // if we didn't click on a panel or the clear button, check if we clicked on a dropped sprite. If we did, then 
    // assign it to the selected sprite and remove it from the dropped sprites list.
    for (auto i : Util::Range((int)_droppedSprites.size() -1, -1)) {
        auto position = _droppedSprites[i]->getPosition();
        auto size = _droppedSprites[i]->getSize();

        if (worldCoords.x >= position.x && worldCoords.y >= position.y && worldCoords.x < position.x + size.x && worldCoords.y < position.y + size.y) {
            _selectedSprite = std::move(_droppedSprites[i]);
            _droppedSprites.erase(_droppedSprites.begin() + i);
            return;
        }
    }
}

void CollisionTester::_handleMouseMove(const Event& event) 
{
    _window.setCursor(Window::Cursor::ARROW);
    if (_selectedPanelIndex) {
        if (!_selectedSprite) {
            _selectedSprite.reset(_spriteRenderer.createSprite(_panelSprites[*_selectedPanelIndex]->getName()));
        }
    }

    if (_selectedSprite) {
        _selectedSprite->moveTo(_uiScene.toWorldCoordinates({ event.x, event.y }) - _selectedSprite->getSize() / 2.f);

        return;
    }

    auto position = _uiScene.toWorldCoordinates({ event.x, event.y });
    std::vector<Panel*> panels{ _clearButton.get() };
    panels.insert(panels.end(), (Panel**)&_panels[0], (Panel**)&_panels[_panels.size() - 1] + 1);

    for (auto panel : panels) {
        glm::vec2 topLeft, bottomRight;
        panel->getOuterBounds(&topLeft, &bottomRight);
        if (position.x >= topLeft.x && position.y >= topLeft.y && position.x < bottomRight.x && position.y < bottomRight.y) {
            _window.setCursor(Window::Cursor::HAND);
            return;
        }
    }
}
