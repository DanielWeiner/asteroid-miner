#ifndef UI_PANEL_H_
#define UI_PANEL_H_

#include "../spriteRenderer.h"
#include "../window.h"

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
    SpriteRenderer& _spriteRenderer;

    Window&         _window;
    glm::vec2       _topLeft;
    glm::vec2       _bottomRight;
    glm::vec2       _innerTopLeft;
    glm::vec2       _innerBottomRight;
   
    Sprite&         _topLeftSprite;    
    Sprite&         _topRightSprite;   
    Sprite&         _bottomLeftSprite; 
    Sprite&         _bottomRightSprite;
    Sprite&         _leftSprite;       
    Sprite&         _rightSprite;      
    Sprite&         _topSprite;        
    Sprite&         _bottomSprite;     
    Sprite&         _centerSprite;

    glm::vec2 _getBorderSize();    
};

#endif