#include "textBox.h"

TextBox::TextBox(std::string fontName, std::shared_ptr<Window> window) 
: _fontName(fontName), _window(window), _textRenderer(window) {}

void TextBox::setBounds(glm::vec2 topLeft, glm::vec2 bottomRight) 
{   
    auto currentSize = _bottomRight - _topLeft;
    auto newSize = bottomRight - topLeft;
    _topLeft = topLeft;
    _bottomRight = bottomRight;

    if (currentSize.x != newSize.x) {
        _recalculateLayout();
    }
}

void TextBox::setText(std::string text) 
{
    if (text != _text) {
        _text = text;

        _recalculateLayout();
    }
}

void TextBox::setColor(glm::vec4 color) 
{
    _color = color;
}

void TextBox::setFontScale(double fontScale) 
{
    _fontScale = fontScale;

    _recalculateLayout();
}

void TextBox::setAlignment(Alignment alignment) 
{
    _align = alignment;

    _recalculateLayout();
}

glm::vec2 TextBox::getTextSize()
{
    return _textRenderer.getLayoutSize(_textLayout) * (float)_fontScale;
}

void TextBox::init() 
{
    _textRenderer.init(_fontName.c_str());
}

void TextBox::render() 
{
    auto projection = glm::ortho(0.f, _window->getSize().x, _window->getSize().y, 0.f, -1.f, 1.f);
    _textRenderer.setProjection(projection);
    _textRenderer.renderLayout(_textLayout, _topLeft, _fontScale, _color);
}

void TextBox::_recalculateLayout() 
{
    if (_text.empty() || _bottomRight - _topLeft == glm::vec2(0.0)) {
        return;
    }

    TextRenderer::Layout::Alignment align = TextRenderer::Layout::ALIGN::LEFT;
    bool justify = false;

    if (_align == ALIGN::CENTER) {
        align = TextRenderer::Layout::ALIGN::CENTER;
    } else if (_align == ALIGN::RIGHT) {
        align = TextRenderer::Layout::ALIGN::RIGHT;
    } else if (_align == ALIGN::LEFT || _align == ALIGN::JUSTIFY) {
        align = TextRenderer::Layout::ALIGN::LEFT;
    }

    if (_align == ALIGN::JUSTIFY) {
        justify = true;
    }

    _textLayout = TextRenderer::Layout();
    _textRenderer.createLayout(_text, (_bottomRight.x - _topLeft.x) / _fontScale, _textLayout, align, justify);
}
