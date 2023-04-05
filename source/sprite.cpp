#include "sprite.h"
#include "spriteBuffer.h"
#include "spriteSheet.h"
#include "time.h"
#include "lineRenderer.h"

#include <glm/ext.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_shape.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>

#include <tuple>
#include <array>
#include <cstddef>

namespace {
    constexpr float box2dScale = 200.0f;
    constexpr float box2dScaleInv = 1.f / box2dScale;
}


Sprite::Sprite(
    std::string spriteName, 
    const SpriteSheet& spriteSheet,
    b2Body* body,
    SpriteBuffer& spriteBuffer,
    bool useLinearScaling,
    bool enableCollisions,
    EventListener* eventListener
) : _name(spriteName), 
    _sheet(spriteSheet),
    _buffer(spriteBuffer),
    _bufferResource(spriteBuffer.createResource(
        *body,
        spriteSheet.getSpriteIndex(spriteName.c_str()), 
        useLinearScaling
    )),
    _body(body),
    _id(_bufferResource.id),
    _enableCollisions(enableCollisions),
    _eventListener(eventListener),
    _baseSize(spriteSheet.getSize(spriteName.c_str()))
{
    _updateFixtures();
    _setTransform();
    _eventListener->onCreate(this);
}

Sprite& Sprite::operator=(const Sprite& sprite)
{

    Sprite::~Sprite();
    Sprite* spritePtr(this);

    new (this) Sprite(sprite);

    return *spritePtr;
}

Sprite::Sprite(Sprite&& other) :
_name(std::move(other._name)),
_sheet(std::move(other._sheet)),
_buffer(other._buffer),
_bufferResource(std::move(other._bufferResource)),
_body(std::move(other._body)),
_id(std::move(other._id)),
_enableCollisions(std::move(other._enableCollisions)),
_eventListener(std::move(other._eventListener)),
_baseSize(std::move(other._baseSize))
{
    _updateFixtures();
    _setTransform();
    _eventListener->onCreate(this);
}

unsigned int Sprite::id() const
{
    return _id;
}

void Sprite::moveTo(float x, float y) 
{
    _position = { x, y };
    _setTransform();
    _eventListener->onUpdate(this);
}

void Sprite::moveTo(glm::vec2 xy) 
{
    return moveTo(xy.x, xy.y);
}

void Sprite::move(float x, float y) 
{
    _position += glm::vec2(x, y);
    _setTransform();
    _eventListener->onUpdate(this);
}

void Sprite::move(glm::vec2 xy) 
{
    return move(xy.x, xy.y);
}

void Sprite::rotate(float radians) 
{
    _rotation += radians;
    _setTransform();
    _eventListener->onUpdate(this);
}

void Sprite::rotateTo(float radians) 
{
    _rotation = radians;
    _setTransform();
    _eventListener->onUpdate(this);
}

void Sprite::scaleBy(float x, float y) 
{
    return scaleBy(glm::vec2(x, y));
}

void Sprite::scaleBy(glm::vec2 xy) 
{
    _nextScale *= xy;
    _updateFixtures();
    _setTransform();
    _eventListener->onUpdate(this);
}

void Sprite::setScale(float x, float y)
{
    return setScale(glm::vec2(x, y));
}

void Sprite::setOpacity(float opacity)
{
    _opacity = opacity;
    _eventListener->onUpdate(this);
}

void Sprite::setScale(glm::vec2 xy)
{
    _nextScale = xy;
    _updateFixtures();
    _setTransform();
    _eventListener->onUpdate(this);
}

glm::vec2 Sprite::getBaseSize() const
{
    return _baseSize;
}

std::string Sprite::getName() const
{
    return _name;
}

glm::vec2 Sprite::getPosition() const
{
    auto halfSize = _nextScale * _baseSize * 0.5f;
    auto pos = _body->GetPosition();
    return glm::vec2(pos.x * box2dScale, pos.y * box2dScale) - halfSize;
}

glm::vec2 Sprite::getCenter() const
{
    return getPosition() + _scale * _baseSize * 0.5f;
}

glm::vec2 Sprite::getSize() const
{
    return _scale * _baseSize;
}

float Sprite::getRotation() const
{
    return _body->GetAngle();
}

void Sprite::debugDraw(LineRenderer* renderer) const
{
    auto fixture = _body->GetFixtureList();

    while (fixture)
    {
        auto shape = fixture->GetShape();

        if (shape->GetType() == b2Shape::e_polygon)
        {
            auto polygon = (b2PolygonShape*)shape;

            for (int i = 0; i < polygon->m_count; i++)
            {
                auto& point = polygon->m_vertices[i];
                auto& nextPoint = polygon->m_vertices[(i + 1) % polygon->m_count];

                auto transform = _body->GetTransform();

                auto matrix = glm::mat4(1.0);
                matrix = glm::scale(matrix, glm::vec3(box2dScale, box2dScale, 1.0f));
                matrix = glm::translate(matrix, glm::vec3(transform.p.x, transform.p.y, 0.0f));
                matrix = glm::rotate(matrix, _body->GetAngle(), glm::vec3(0.0f, 0.0f, 1.0f));
                

                auto transformedPoint = matrix * glm::vec4(point.x, point.y, 0.0f, 1.0f);
                auto transformedNextPoint = matrix * glm::vec4(nextPoint.x, nextPoint.y, 0.0f, 1.0f);

                renderer->lineTo(glm::vec2(transformedPoint), glm::vec2(transformedNextPoint), glm::vec4(0.0f, 1.0f, 0.0f, 0.75f), 1.25);
            }
        }

        fixture = fixture->GetNext();
    }
}

void Sprite::update()
{
    glm::mat4& model = *_buffer.getModelMatrix(_bufferResource.id);
    _scale = _nextScale;
    _position = getPosition();
    _rotation = getRotation();
    auto size = _nextScale * _baseSize;
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(_position.x, _position.y, 0.0f));  

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); 
    model = glm::rotate(model, _rotation, glm::vec3(0.0f, 0.0f, 1.0f)); 
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));

    if (_body->GetType() != b2_staticBody) {
        switchToDynamic();
    }
}

bool Sprite::pointIsInHitbox(float x, float y) const
{
    auto fixture = _body->GetFixtureList();
    while(fixture)
    {
        if(fixture->TestPoint(b2Vec2(x * box2dScaleInv, y * box2dScaleInv)))
        {
            return true;
        }
        fixture = fixture->GetNext();
    }
    return false;
}

void Sprite::switchToKinematic() 
{
    _body->SetType(b2_kinematicBody);
}

void Sprite::switchToDynamic() 
{
    _body->SetType(b2_dynamicBody);
}

void Sprite::switchToStatic() 
{
    _body->SetType(b2_staticBody);
}

Sprite::~Sprite() 
{
    _eventListener->onDelete(this);
    _buffer.destroyResource(_bufferResource);
    _body->GetWorld()->DestroyBody(_body);
}

void Sprite::_updateFixtures() 
{
    auto& geometry = _sheet.getSpriteGeometry(_name.c_str());
    b2Fixture* fixture = _body->GetFixtureList();
    while(fixture) {
        _body->DestroyFixture(fixture);
        fixture = _body->GetFixtureList();
    }

    auto triangles = geometry.getTriangles();

    for(auto& triangle : triangles) {
        b2PolygonShape shape;
        b2Vec2 vertices[3];
        
        auto nextSize = _nextScale * _baseSize;

        for(int i = 0; i < 3; i++) {
            glm::vec2 transformedPoint = nextSize * triangle[i];

            vertices[i] = b2Vec2(transformedPoint.x * box2dScaleInv, transformedPoint.y * box2dScaleInv);
        }

        shape.Set(vertices, 3);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 1.0f;
        fixtureDef.filter.maskBits = _enableCollisions ? 0xFFFF : 0x0000;

        _body->CreateFixture(&fixtureDef);
    }
}

void Sprite::_setTransform() 
{
    if (_body->GetType() != b2_staticBody) {
        switchToKinematic();
        _body->SetAngularVelocity(0.0f);
        _body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    }
    
    auto halfSize = _nextScale * _baseSize * 0.5f;
    _body->SetTransform(b2Vec2((_position.x + halfSize.x) * box2dScaleInv, (_position.y + halfSize.y) * box2dScaleInv), _rotation);
}
