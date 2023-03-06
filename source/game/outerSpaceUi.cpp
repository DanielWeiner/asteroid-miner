#include "outerSpaceUi.h"

#include <glm/ext.hpp>
#include <algorithm>

namespace {
    const std::string fontName = "Teorema";
}

OuterSpaceUi::OuterSpaceUi(std::unique_ptr<SpriteFactory> &&spriteFactory, std::shared_ptr<Window> window) 
: 
_textBox(fontName, window), 
_panel(std::move(spriteFactory), window)
{
}

void OuterSpaceUi::setPanelBounds(glm::vec2 topLeft, glm::vec2 bottomRight) 
{
    _panel.setBounds(topLeft, bottomRight);
    glm::vec2 innerTopLeft, innerBottomRight;
    _panel.getInnerBounds(&innerTopLeft, &innerBottomRight);
    _textBox.setBounds(innerTopLeft, innerBottomRight);
    _resizeVertically();
}

void OuterSpaceUi::setFontSize(double fontSize) 
{
    _textBox.setFontScale(fontSize);
    _resizeVertically();
}

void OuterSpaceUi::setFontColor(glm::vec4 color) 
{
    _textBox.setColor(color);
}

void OuterSpaceUi::setText(std::string text) 
{
    _textBox.setText(text);
    _resizeVertically();
}

void OuterSpaceUi::render() 
{
    _panel.render();
    _textBox.render();
}

void OuterSpaceUi::init() 
{
    _panel.init();
    _textBox.init();
    _textBox.setAlignment(TextBox::ALIGN::LEFT);
}

void OuterSpaceUi::_resizeVertically() 
{
    glm::vec2 topLeft, bottomRight;
    _panel.getInnerBounds(&topLeft, &bottomRight);
    auto textSize = _textBox.getTextSize();
    if (textSize.y > 0) {
        bottomRight.y = topLeft.y + textSize.y;
        _panel.setInnerBounds(topLeft, bottomRight);
    }
}
