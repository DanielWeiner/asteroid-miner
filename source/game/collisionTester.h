#ifndef COLLISION_TESTER_H_
#define COLLISION_TESTER_H_

#include "../ui/panel.h"
#include "../sprite.h"
#include "../flatScene.h"
#include "../textRenderer.h"
#include "../ui/textBox.h"
#include "../spriteFactory.h"
#include "../window.h"
#include "../windowedApplication.h"
#include "../event.h"

#include <optional>

class CollisionTester : public WindowedApplication {
public:
    CollisionTester(
        const Window& window, 
        SpriteRenderer& spriteRenderer, 
        SpriteRenderer& uiSpriteRenderer
    );
    void handleEvent(const Event& event) override;
    void render() override;
    void init() override;
private:
    const Window&                        _window;
    SpriteRenderer&                      _spriteRenderer;
    SpriteRenderer&                      _uiSpriteRenderer;
    TextRenderer                         _textRenderer;
    FlatScene                            _scene{_window, 45.0};
    FlatScene                            _uiScene{_window, 45.0};


    TextBox                               _clearButtonText{_window, _textRenderer};
    TextBox                               _fpsCounter{_window, _textRenderer};

    std::unique_ptr<Panel>               _clearButton;
    std::vector<std::unique_ptr<Panel>>  _panels;
    std::vector<std::unique_ptr<Sprite>> _panelSprites;

    std::optional<std::size_t>           _selectedPanelIndex;
    std::unique_ptr<Sprite>              _selectedSprite;
    std::vector<std::unique_ptr<Sprite>> _droppedSprites;

    void                                 _createClearButton(glm::vec2 location, glm::vec2 size, std::string text);

    void                                 _handleMouseEvent(const Event& event);
    void                                 _handleMouseRelease(const Event& event);
    void                                 _handleMousePress(const Event& event);
    void                                 _handleMouseMove(const Event& event);
    void                                 _handleMouseScroll(const Event& event);
    void                                 _addPanel(glm::vec2 location, glm::vec2 size, std::string icon);
    std::optional<std::size_t>           _getSelectedSpriteVariation(glm::vec2 location);

};

#endif