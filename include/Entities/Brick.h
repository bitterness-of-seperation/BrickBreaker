#pragma once

#include "Entities/Entity.h"

class Brick : public Entity {
private:
    int hitPoints;//hit points
    int score;
    bool breakable;
    sf::Color color;
    
    // Helper method to update color based on hit points
    void updateColorFromHitPoints();

public:
    Brick();
    Brick(const sf::Vector2f& pos, const sf::Vector2f& size, int hitPoints = 1, int score = 100);

    void update(float deltaTime) override;
    void onCollision(Entity* other) override;
    
    int getHitPoints() const;
    void setHitPoints(int points);
    
    int getScore() const;
    void setScore(int score);
    
    bool isBreakable() const;
    void setBreakable(bool breakable);
    
    void hit();//handle hit
    
    void setColor(const sf::Color& color);
    sf::Color getColor() const;
};