#ifndef SPRITE_H_
#define SPRITE_H_

#include "spriteResource.h"
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <box2d/b2_body.h>
#include <box2d/b2_mouse_joint.h>
class SpriteSheet;
class SpriteBuffer;
class LineRenderer;

class Sprite {
public:
    struct EventListener {
        virtual void onCreate(Sprite* sprite) = 0;
        virtual void onUpdate(Sprite* sprite) = 0;
        virtual void onDelete(Sprite* sprite) = 0;
    };

    struct DefaultEventListener : public EventListener {
        void onCreate(Sprite* sprite) override {}
        void onUpdate(Sprite* sprite) override {}
        void onDelete(Sprite* sprite) override {}
    };

    Sprite(
        std::string spriteName, 
        const SpriteSheet& spriteSheet,
        b2Body* body,
        SpriteBuffer& buffer,
        bool useLinearScaling = true,
        bool enableCollisions = true,
        EventListener* eventListener = new DefaultEventListener()
    );
    Sprite(const Sprite& other) = default;
    Sprite(Sprite&& other);
    unsigned int id() const;

    Sprite& operator=(const Sprite &);

    void moveTo(float x, float y);
    void moveTo(glm::vec2 xy);

    void move(float x, float y);
    void move(glm::vec2 xy);

    void rotate(float radians);
    void rotateTo(float radians);

    void scaleBy(float x, float y);
    void scaleBy(glm::vec2 xy);

    void setScale(float x, float y);
    void setScale(glm::vec2 xy);

    void setOpacity(float opacity);

    glm::vec2 getBaseSize() const;
    std::string getName() const;
    glm::vec2 getPosition() const;
    glm::vec2 getCenter() const;
    glm::vec2 getSize() const;
    float getRotation() const;

    void debugDraw(LineRenderer* renderer) const;

    void update();

    bool pointIsInHitbox(float x, float y) const;

    void switchToKinematic();
    void switchToDynamic();
    void switchToStatic();
    
    ~Sprite();
private:
    std::string         _name;
    const SpriteSheet&  _sheet;
    SpriteBuffer&       _buffer;
    SpriteResource      _bufferResource;
    b2Body*             _body;
    const unsigned int  _id;
    glm::vec2           _scale{1,1};
    glm::vec2           _nextScale{1,1};
    float               _opacity;
    bool                _enableCollisions;
    EventListener*      _eventListener;
    bool                _sizeChanged{true};
    glm::vec2           _position{0,0};
    float               _rotation{0};
    const glm::vec2     _baseSize;

    void                _updateFixtures();
    void                _setTransform();
};

#endif