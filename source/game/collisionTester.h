#ifndef COLLISION_TESTER_H_
#define COLLISION_TESTER_H_

#include "../lineRenderer.h"
#include "../spritePhysicsEngine.h"
#include "../spriteRenderer.h"
#include "../ui/panel.h"
#include "../sprite.h"
#include "../flatScene.h"
#include "../textRenderer.h"
#include "../ui/textBox.h"
#include "../spriteFactory.h"
#include "../window.h"
#include "../windowedApplication.h"
#include "../event.h"
#include "constants.h"
#include <optional>
#include <array>
#include <box2d/b2_world.h>

class CollisionTester : public WindowedApplication {
private:
    struct SpriteEventListener : public Sprite::DefaultEventListener {
        SpriteEventListener(SpritePhysicsEngine& physicsEngine) : physicsEngine(physicsEngine) {}

        SpritePhysicsEngine& physicsEngine;

        void onCreate(Sprite* sprite) override {
            physicsEngine.addSprite(sprite);
        }
        void onDelete(Sprite* sprite) override {
            physicsEngine.removeSprite(sprite);
        }
    };
public:
    CollisionTester(
        const Window& window,
        SpriteFactory& spriteFactory
    );
    void handleEvent(const Event& event) override;
    void render() override;
    void init() override;
private:
    const Window&                                  _window;
    b2World                                        _world{b2Vec2(0.0f, 0.0f)};
    SpritePhysicsEngine                            _spritePhysicsEngine{_world, PHYSICS_TIMESTEP, PHYSICS_VELOCITY_ITERATIONS, PHYSICS_POSITION_ITERATIONS};
    SpriteEventListener                            _spriteEventListener{_spritePhysicsEngine};
    std::array<std::unique_ptr<SpriteRenderer>, 2> _spriteRenderers;
    SpriteRenderer&                                _spriteRenderer;
    SpriteRenderer&                                _uiSpriteRenderer;
    TextRenderer                                   _textRenderer;
    FlatScene                                      _scene{_window, 45.0};
    FlatScene                                      _uiScene{_window, 45.0};
    LineRenderer                                   _lineRenderer;

    TextBox                                        _clearButtonText{_window, _textRenderer};
    TextBox                                        _fpsCounter{_window, _textRenderer};

    std::unique_ptr<Panel>                         _clearButton;
    std::vector<std::unique_ptr<Panel>>            _panels;
    std::vector<std::unique_ptr<Sprite>>           _panelSprites;

    std::optional<std::size_t>                     _selectedPanelIndex;
    std::unique_ptr<Sprite>                        _selectedSpriteDragging;
    std::unique_ptr<Sprite>                        _selectedSpriteRotating;
    std::vector<std::unique_ptr<Sprite>>           _droppedSprites;

    void                                           _createClearButton(glm::vec2 location, glm::vec2 size, std::string text);

    void                                           _handleMouseEvent(const Event& event);
    void                                           _handleMouseRelease(const Event& event);
    void                                           _handleMousePress(const Event& event);
    void                                           _handleMousePress(const Event& event, int button);
    void                                           _handleMouseRelease(const Event& event, int button);
    void                                           _handleMouseMove(const Event& event);
    void                                           _handleMouseScroll(const Event& event);
    void                                           _addPanel(glm::vec2 location, glm::vec2 size, std::string icon);
    std::optional<std::size_t>                     _getSelectedSpriteVariation(glm::vec2 location);
    void                                           _trySelectSprite(const glm::vec2& location, std::unique_ptr<Sprite>& sprite);
    void                                           _putSpriteBack(std::unique_ptr<Sprite>& sprite);
    bool                                           _isAnySpriteSelected();
};

#endif