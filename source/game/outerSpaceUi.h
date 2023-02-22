#ifndef OUTER_SPACE_UI_H_
#define OUTER_SPACE_UI_H_

#include "../spriteFactory.h"
#include "../textRenderer.h"
#include <memory>
#include <glm/glm.hpp>

class OuterSpaceUi {
public:
    OuterSpaceUi(std::unique_ptr<SpriteFactory>&& spriteFactory, std::shared_ptr<Window> window);
    void drawPanel(glm::vec2 topLeft, glm::vec2 bottomRight);
    void init();
private:
    std::unique_ptr<SpriteFactory>  _spriteFactory;
    std::unique_ptr<SpriteRenderer> _spriteRenderer;
    std::shared_ptr<Window>         _window;
    TextRenderer                    _textRenderer;
};

#endif