#include "Entities/Brick.h"
#include "Entities/Ball.h"

Brick::Brick() 
    : Entity(), 
      hitPoints(1), 
      score(100), 
      breakable(true),
      color(sf::Color::White) {
}

Brick::Brick(const sf::Vector2f& pos, const sf::Vector2f& size, int hitPoints, int score)
    : Entity(pos, size), 
      hitPoints(hitPoints), 
      score(score), 
      breakable(true),
      color(sf::Color::White) {
}

void Brick::update(float deltaTime) {
    // Bricks typically don't need per-frame updates unless special effects are added
    // If animation effects are needed, they can be implemented here
}

void Brick::onCollision(Entity* other) {
    // Check if collision is with a ball
    if (dynamic_cast<Ball*>(other) != nullptr && breakable) {
        hit();
    }
}

int Brick::getHitPoints() const {
    return hitPoints;
}

void Brick::setHitPoints(int points) {
    hitPoints = points;
}

int Brick::getScore() const {
    return score;
}

void Brick::setScore(int score) {
    this->score = score;
}

bool Brick::isBreakable() const {
    return breakable;
}

void Brick::setBreakable(bool breakable) {
    this->breakable = breakable;
}

void Brick::hit() {
    if (breakable) {
        hitPoints--;
        
        // Change color based on remaining hit points
        if (hitPoints <= 0) {
            setActive(false); // Brick is destroyed
        } else if (hitPoints == 1) {
            setColor(sf::Color::Red);
        } else if (hitPoints == 2) {
            setColor(sf::Color::Yellow);
        } else {
            setColor(sf::Color::Green);
        }
    }
}

void Brick::setColor(const sf::Color& color) {
    this->color = color;
    if (sprite) {
        sprite->setColor(color);
    }
}

sf::Color Brick::getColor() const {
    return color;
}