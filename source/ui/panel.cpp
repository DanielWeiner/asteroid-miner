#include "panel.h"

#include <glm/ext.hpp>

Panel::Panel(std::unique_ptr<SpriteFactory>&& spriteFactory, std::shared_ptr<Window> window) : 
    _spriteFactory(std::move(spriteFactory)), _window(window) {}

void Panel::setBounds(glm::vec2 topLeft, glm::vec2 bottomRight) 
{
    _topLeft = topLeft;
    _bottomRight = bottomRight;

    auto fullSize = glm::floor(bottomRight - topLeft);
    auto innerSize = glm::floor(fullSize - _getBorderSize());

    _leftSprite->setScale(_leftSprite->getBaseSize() * glm::vec2(1, innerSize.y));
    _rightSprite->setScale(_rightSprite->getBaseSize() * glm::vec2(1, innerSize.y));

    _topSprite->setScale(_topSprite->getBaseSize() * glm::vec2(innerSize.x, 1));
    _bottomSprite->setScale(_bottomSprite->getBaseSize() * glm::vec2(innerSize.x, 1));

    _centerSprite->setScale(_centerSprite->getBaseSize() * innerSize);

    _innerTopLeft = glm::floor(topLeft + _topLeftSprite->getSize());
    _innerBottomRight = glm::floor(_innerTopLeft + innerSize);

    _topLeftSprite->moveTo(topLeft);
    _topRightSprite->moveTo(_innerBottomRight.x, topLeft.y);
    _bottomLeftSprite->moveTo(topLeft.x, _innerBottomRight.y);
    _bottomRightSprite->moveTo(_innerBottomRight);

    _leftSprite->moveTo(topLeft.x, _innerTopLeft.y);
    _rightSprite->moveTo(_innerBottomRight.x, _innerTopLeft.y);
    _topSprite->moveTo(_innerTopLeft.x, topLeft.y);
    _bottomSprite->moveTo(_innerTopLeft.x, _innerBottomRight.y);
    _centerSprite->moveTo(_innerTopLeft);
}

void Panel::setInnerBounds(glm::vec2 topLeft, glm::vec2 bottomRight) 
{
    setBounds(topLeft - _topLeftSprite->getBaseSize(), bottomRight + _bottomRightSprite->getBaseSize());
}

void Panel::init() 
{
    _spriteRenderer = _spriteFactory->createRenderer(false);

    _topLeftSprite     = _spriteFactory->createSprite("UI/metalPanel/topLeft.png");
    _topRightSprite    = _spriteFactory->createSprite("UI/metalPanel/topRight.png");
    _bottomLeftSprite  = _spriteFactory->createSprite("UI/metalPanel/bottomLeft.png");
    _bottomRightSprite = _spriteFactory->createSprite("UI/metalPanel/bottomRight.png");
    _leftSprite        = _spriteFactory->createSprite("UI/metalPanel/left.png");
    _rightSprite       = _spriteFactory->createSprite("UI/metalPanel/right.png");
    _topSprite         = _spriteFactory->createSprite("UI/metalPanel/top.png");
    _bottomSprite      = _spriteFactory->createSprite("UI/metalPanel/bottom.png");
    _centerSprite      = _spriteFactory->createSprite("UI/metalPanel/center.png");
}

void Panel::render() 
{
    auto projection = glm::ortho(0.f, _window->getSize().x, _window->getSize().y, 0.f, -1.f, 1.f);
    _spriteRenderer->setProjection(projection);
    _spriteRenderer->setView(glm::mat4(1.0));
    _spriteRenderer->draw();
}

void Panel::getInnerBounds(glm::vec2* topLeft, glm::vec2* bottomRight)
{
    if (topLeft != NULL) {
        *topLeft = _innerTopLeft;
    }
    if (bottomRight != NULL) {
        *bottomRight = _innerBottomRight;
    }
}

void Panel::getOuterBounds(glm::vec2* topLeft, glm::vec2* bottomRight)
{
    if (topLeft != NULL) {
        *topLeft = _topLeft;
    }
    if (bottomRight != NULL) {
        *bottomRight = _bottomRight;
    }
}

glm::vec2 Panel::_getBorderSize()
{
    return _topLeftSprite->getBaseSize() + _bottomRightSprite->getBaseSize();
}
