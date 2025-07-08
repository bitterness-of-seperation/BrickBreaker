#include "Entities/Brick.h"
#include "Entities/Ball.h"
#include "Utils/Config.h"

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
    
    // Update color based on hit points
    updateColorFromHitPoints();
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
        
        // Update color based on remaining hit points
        if (hitPoints <= 0) {
            setActive(false); // Brick is destroyed
        } else {
            updateColorFromHitPoints();
        }
    }
}

void Brick::updateColorFromHitPoints() {
    // Get colors from config or use defaults
    sf::Color color1 = Config::getInstance().getValue("colors.brick1", sf::Color::Red);
    sf::Color color2 = Config::getInstance().getValue("colors.brick2", sf::Color::Yellow);
    sf::Color color3 = Config::getInstance().getValue("colors.brick3", sf::Color::Green);
    sf::Color color4 = Config::getInstance().getValue("colors.brick4", sf::Color::Blue);
    sf::Color color5 = Config::getInstance().getValue("colors.brick5", sf::Color(128, 0, 128));
    
    // Set color based on hit points
    switch (hitPoints) {
        case 1:
            setColor(color1);
            break;
        case 2:
            setColor(color2);
            break;
        case 3:
            setColor(color3);
            break;
        case 4:
            setColor(color4);
            break;
        case 5:
            setColor(color5);
            break;
        default:
            if (hitPoints > 5) {
                setColor(color5); // Use color5 for higher hit points
            } else {
                setColor(sf::Color::White);
            }
            break;
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