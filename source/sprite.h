#ifndef SPRITE_H_
#define SPRITE_H_

#include <glm/glm.hpp>
#include <string>

class SpriteSheet;
class SpriteBuffer;

class Sprite {
public:
    Sprite(
        std::string spriteName, 
        unsigned int id, 
        SpriteSheet& spriteSheet, 
        SpriteBuffer& spriteBuffer
    );
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

    glm::vec2 getBaseSize();

    std::string getName();
    glm::vec2 getPosition();
    glm::vec2 getCenter();
    glm::vec2 getSize();
    float getRotation();

    glm::vec2 getNextPosition();

    void updateModelMatrix();
    void updateTextureIndex();

    bool pointIsInHitbox(float x, float y);

    void moveToEndOfBuffer();

    void init();

    

    ~Sprite();
private:
    struct SpriteState {
        float _x = 0;
        float _y = 0;
        float _width = 1;
        float _height = 1;
        float _rotate = 0;
    };

    SpriteState        _states[2];
    bool               _textureUpdated = false;
    
    std::string        _name;
    const unsigned int _id;
    unsigned int       _lastStep = -1;
    glm::mat4          _lastModel;
    bool               _initialized = false;

    SpriteSheet&       _sheet;
    SpriteBuffer&      _buffer;

    void _useNextState(SpriteState*& state);
    void _useCurrentState(SpriteState*& state);
    void _update();
};

#endif