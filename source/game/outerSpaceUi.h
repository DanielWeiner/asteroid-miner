#ifndef OUTER_SPACE_UI_H_
#define OUTER_SPACE_UI_H_

#include "../spriteFactory.h"
#include "../textRenderer.h"
#include "../ui/panel.h"
#include "../ui/textBox.h"
#include <memory>
#include <glm/glm.hpp>

class OuterSpaceUi {
public:
    OuterSpaceUi(std::unique_ptr<SpriteFactory>&& spriteFactory, std::shared_ptr<Window> window);
    void setPanelBounds(glm::vec2 topLeft, glm::vec2 bottomRight);
    void setFontSize(double fontSize);
    void setFontColor(glm::vec4 color);
    void setText(std::string text);
    void render();
    void init();
private:
    TextBox                         _textBox;
    Panel                           _panel;

    void _resizeVertically();
};

#endif