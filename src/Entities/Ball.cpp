#include "Entities/Ball.h"
#include <cmath>

Ball::Ball() : Entity(), velocity(200.0f, -200.0f), radius(10.0f) {
    speed = 400.0f;
}

Ball::Ball(const sf::Vector2f& pos, float radius) 
    : Entity(pos, sf::Vector2f(radius * 2, radius * 2)), 
      velocity(200.0f, -200.0f), 
      radius(radius) {
    speed = 400.0f;
}

void Ball::update(float deltaTime) {
    if (!active) return;
    
    // Update position
    position += velocity * deltaTime;
    // Use parent's setPosition method which will properly handle sprite pointer
    setPosition(position);
}

void Ball::reverseX() {
    velocity.x = -velocity.x;
}

void Ball::reverseY() {
    velocity.y = -velocity.y;
}

void Ball::setVelocity(const sf::Vector2f& vel) {
    velocity = vel;
    
    // Ensure ball speed doesn't exceed maximum
    float length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (length > speed) {
        velocity = velocity / length * speed;
    }
}

sf::Vector2f Ball::getVelocity() const {
    return velocity;
}

void Ball::onCollision(Entity* other) {
    // Collision response will be handled in CollisionManager
}

float Ball::getRadius() const {
    return radius;
}