#ifndef UI_TEXT_BOX_H_
#define UI_TEXT_BOX_H_

#include "../textRenderer.h"
#include "../window.h"

#include <memory>
#include <string>
#include <glm/glm.hpp>

class TextBox {
public:
    using Alignment = unsigned int;
    struct ALIGN {
        static constexpr Alignment LEFT    = 0;
        static constexpr Alignment RIGHT   = 1;
        static constexpr Alignment CENTER  = 2;
        static constexpr Alignment JUSTIFY = 3;
    };
    TextBox(std::string fontName, std::shared_ptr<Window> window);
    void setBounds(glm::vec2 topLeft, glm::vec2 bottomRight);
    void setText(std::string text);
    void setColor(glm::vec4 color);
    void setFontScale(double fontScale);
    void setAlignment(Alignment alignment);
    glm::vec2 getTextSize();
    void init();
    void render();
private:
    std::string                      _fontName;
    std::shared_ptr<Window>          _window;
    TextRenderer                     _textRenderer;
    double                           _fontScale = 1;
    glm::vec2                        _topLeft;
    glm::vec2                        _bottomRight;
    TextRenderer::Layout             _textLayout;
    std::string                      _text;
    glm::vec4                        _color{ 0.f, 0.f, 0.f, 1.f };
    Alignment                        _align = ALIGN::LEFT;
    void _recalculateLayout();
};

#endif