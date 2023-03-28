#ifndef UI_PANEL_H_
#define UI_PANEL_H_

#include "../spriteRenderer.h"
#include "../window.h"

#include <glm/glm.hpp>

class Panel {
public:
    Panel(SpriteRenderer& renderer, const Window& window);

    void setBounds(glm::vec2 topLeft, glm::vec2 bottomRight);
    void setInnerBounds(glm::vec2 topLeft, glm::vec2 bottomRight);
    void init();
    void render();
    void getInnerBounds(glm::vec2* topLeft, glm::vec2* bottomRight);
    void getOuterBounds(glm::vec2* topLeft, glm::vec2* bottomRight);
private:
    SpriteRenderer& _spriteRenderer;

    const Window&           _window;
    glm::vec2               _topLeft;
    glm::vec2               _bottomRight;
    glm::vec2               _innerTopLeft;
    glm::vec2               _innerBottomRight;
   
    std::unique_ptr<Sprite> _topLeftSprite;    
    std::unique_ptr<Sprite> _topRightSprite;   
    std::unique_ptr<Sprite> _bottomLeftSprite; 
    std::unique_ptr<Sprite> _bottomRightSprite;
    std::unique_ptr<Sprite> _leftSprite;       
    std::unique_ptr<Sprite> _rightSprite;      
    std::unique_ptr<Sprite> _topSprite;        
    std::unique_ptr<Sprite> _bottomSprite;     
    std::unique_ptr<Sprite> _centerSprite;

    glm::vec2 _getBorderSize();    
};

#endif