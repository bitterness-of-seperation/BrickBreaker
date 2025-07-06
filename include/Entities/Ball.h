#pragma once

#include "Entities/Entity.h"

class Ball : public Entity {
private:
    sf::Vector2f velocity;
    float radius;

public:
    Ball();
    Ball(const sf::Vector2f& pos, float radius);

    void update(float deltaTime) override;
    void reverseX();
    void reverseY();
    void setVelocity(const sf::Vector2f& vel);
    sf::Vector2f getVelocity() const;
    void onCollision(Entity* other) override;
    
    float getRadius() const;
};