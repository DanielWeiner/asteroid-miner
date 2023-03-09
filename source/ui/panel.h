#ifndef UI_PANEL_H_
#define UI_PANEL_H_

#include "../spriteRenderer.h"
#include "../window.h"

#include <memory>
#include <glm/glm.hpp>

class Panel {
public:
    Panel(SpriteRenderer& renderer, Window& window);

    void setBounds(glm::vec2 topLeft, glm::vec2 bottomRight);
    void setInnerBounds(glm::vec2 topLeft, glm::vec2 bottomRight);
    void init();
    void render();
    void getInnerBounds(glm::vec2* topLeft, glm::vec2* bottomRight);
    void getOuterBounds(glm::vec2* topLeft, glm::vec2* bottomRight);
private:
    SpriteRenderer&               _spriteRenderer;

    Window&                       _window;
    glm::vec2                     _topLeft;
    glm::vec2                     _bottomRight;
    glm::vec2                     _innerTopLeft;
    glm::vec2                     _innerBottomRight;

    std::unique_ptr<Sprite> const _topLeftSprite;    
    std::unique_ptr<Sprite> const _topRightSprite;   
    std::unique_ptr<Sprite> const _bottomLeftSprite; 
    std::unique_ptr<Sprite> const _bottomRightSprite;
    std::unique_ptr<Sprite> const _leftSprite;       
    std::unique_ptr<Sprite> const _rightSprite;      
    std::unique_ptr<Sprite> const _topSprite;        
    std::unique_ptr<Sprite> const _bottomSprite;     
    std::unique_ptr<Sprite> const _centerSprite;

    glm::vec2 _getBorderSize();    
};

#endif