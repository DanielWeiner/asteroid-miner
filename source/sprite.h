#ifndef SPRITE_H_
#define SPRITE_H_

#include <glm/glm.hpp>
#include <string>
#include <memory>

class SpriteSheet;
class SpriteBuffer;

class Sprite {
public:
    Sprite(
        std::string spriteName, 
        unsigned int id, 
        std::shared_ptr<SpriteSheet> spriteSheet, 
        std::shared_ptr<SpriteBuffer> spriteBuffer
    );

    Sprite& operator=(const Sprite &);

    void moveTo(float x, float y);
    void moveTo(glm::vec2 xy);

    void move(float x, float y);
    void move(glm::vec2 xy);

    void rotate(float radians);
    void rotateTo(float radians);

    void scaleBy(float x, float y);
    void scaleBy(glm::vec2 xy);

    std::string getName();
    glm::vec2 getPosition();
    glm::vec2 getSize();

    void updateModelMatrix();
    void updateTextureMatrix();

    bool pointIsInHitbox(float x, float y);

    ~Sprite();
private:
    std::string        _name;
    const unsigned int _id;

    float              _x = 0;
    float              _y = 0;
    float              _width = 1;
    float              _height = 1;
    float              _rotate = 0;

    std::shared_ptr<SpriteSheet>    _sheet;
    std::shared_ptr<SpriteBuffer>   _buffer;
};

#endif