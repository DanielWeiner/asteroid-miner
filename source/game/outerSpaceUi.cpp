#include "outerSpaceUi.h"

#include <glm/ext.hpp>
#include <algorithm>

OuterSpaceUi::OuterSpaceUi(std::unique_ptr<SpriteFactory> &&spriteFactory, std::shared_ptr<Window> window) 
: _spriteFactory(std::move(spriteFactory)), _window(window), _textRenderer(_window)
{
}

void OuterSpaceUi::drawPanel(glm::vec2 topLeft, glm::vec2 bottomRight) 
{
    auto topLeftSprite      = _spriteFactory->createSprite("UI/metalPanel/topLeft.png");
    auto topRightSprite     = _spriteFactory->createSprite("UI/metalPanel/topRight.png");
    auto bottomLeftSprite   = _spriteFactory->createSprite("UI/metalPanel/bottomLeft.png");
    auto bottomRightSprite  = _spriteFactory->createSprite("UI/metalPanel/bottomRight.png");
    auto leftSprite         = _spriteFactory->createSprite("UI/metalPanel/left.png");
    auto rightSprite        = _spriteFactory->createSprite("UI/metalPanel/right.png");
    auto topSprite          = _spriteFactory->createSprite("UI/metalPanel/top.png");
    auto bottomSprite       = _spriteFactory->createSprite("UI/metalPanel/bottom.png");
    auto centerSprite       = _spriteFactory->createSprite("UI/metalPanel/center.png");
    
    auto borderSize = topLeftSprite->getSize() + bottomRightSprite->getSize();
    auto fullSize = bottomRight - topLeft;
    auto innerSize = fullSize - borderSize;

    leftSprite->scaleBy(glm::vec2(1, innerSize.y));
    rightSprite->scaleBy(glm::vec2(1, innerSize.y));

    topSprite->scaleBy(glm::vec2(innerSize.x, 1));
    bottomSprite->scaleBy(glm::vec2(innerSize.x, 1));

    centerSprite->scaleBy(innerSize);

    auto innerTopLeft = topLeft + topLeftSprite->getSize();
    auto innerBottomRight = innerTopLeft + innerSize;

    topLeftSprite->moveTo(topLeft);
    topRightSprite->moveTo(innerBottomRight.x, topLeft.y);
    bottomLeftSprite->moveTo(topLeft.x, innerBottomRight.y);
    bottomRightSprite->moveTo(innerBottomRight);

    leftSprite->moveTo(topLeft.x, innerTopLeft.y);
    rightSprite->moveTo(innerBottomRight.x, innerTopLeft.y);
    topSprite->moveTo(innerTopLeft.x, topLeft.y);
    bottomSprite->moveTo(innerTopLeft.x, innerBottomRight.y);
    centerSprite->moveTo(innerTopLeft);

    auto projection = glm::ortho(0.f, _window->getSize().x, _window->getSize().y, 0.f, -1.f, 1.f);
    _spriteRenderer->setProjection(projection);
    _spriteRenderer->setView(glm::mat4(1.0));
    _spriteRenderer->draw();   

    _textRenderer.setProjection(projection);
    _textRenderer.setView(glm::mat4(1.0));


    float fontScale = 6;
    _textRenderer.renderText("The quick brown fox jumps over the lazy dog.", glm::vec2(200,500), fontScale, glm::vec4(1,1,1,1));
    _textRenderer.renderText("bazinga", glm::vec2(300,800), fontScale, glm::vec4(1,1,0,1));
}

void OuterSpaceUi::init() 
{
    _spriteRenderer = _spriteFactory->createRenderer(false);
    //_textRenderer.init("JUST Sans Regular.otf", "JUST Sans");
    _textRenderer.init("Hyperwave Display.otf", "Hyperwave");
}