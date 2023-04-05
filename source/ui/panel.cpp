#include "panel.h"

#include <glm/ext.hpp>

Panel::Panel(SpriteRenderer& renderer, const Window& window) :  
    _spriteRenderer(renderer),
    _window(window),
    _topLeftSprite(_spriteRenderer.createSprite("UI/metalPanel/topLeft.png", false, false)),  
    _topRightSprite(_spriteRenderer.createSprite("UI/metalPanel/topRight.png", false, false)), 
    _bottomLeftSprite(_spriteRenderer.createSprite("UI/metalPanel/bottomLeft.png", false, false)),
    _bottomRightSprite(_spriteRenderer.createSprite("UI/metalPanel/bottomRight.png", false, false)),
    _leftSprite(_spriteRenderer.createSprite("UI/metalPanel/left.png", false, false)),       
    _rightSprite(_spriteRenderer.createSprite("UI/metalPanel/right.png", false, false)),      
    _topSprite(_spriteRenderer.createSprite("UI/metalPanel/top.png", false, false)),        
    _bottomSprite(_spriteRenderer.createSprite("UI/metalPanel/bottom.png", false, false)),     
    _centerSprite(_spriteRenderer.createSprite("UI/metalPanel/center.png", false, false)) 
{
    _topLeftSprite->switchToStatic();
    _topRightSprite->switchToStatic();
    _bottomLeftSprite->switchToStatic();
    _bottomRightSprite->switchToStatic();
    _leftSprite->switchToStatic();
    _rightSprite->switchToStatic();
    _topSprite->switchToStatic();
    _bottomSprite->switchToStatic();
    _centerSprite->switchToStatic();
}

void Panel::setBounds(glm::vec2 topLeft, glm::vec2 bottomRight) 
{
    _topLeft = topLeft;
    _bottomRight = bottomRight;

    auto fullSize = glm::floor(bottomRight - topLeft);
    auto innerSize = glm::floor(fullSize - _getBorderSize());

    _leftSprite->setScale(glm::vec2(1, innerSize.y));
    _rightSprite->setScale(glm::vec2(1, innerSize.y));

    _topSprite->setScale(glm::vec2(innerSize.x, 1));
    _bottomSprite->setScale(glm::vec2(innerSize.x, 1));

    _centerSprite->setScale(innerSize);

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
}

void Panel::render() 
{
    auto projection = glm::ortho(0.f, _window.getSize().x, _window.getSize().y, 0.f, -1.f, 1.f);
    _spriteRenderer.setProjection(projection);
    _spriteRenderer.setView(glm::mat4(1.0));
    _spriteRenderer.draw();
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
