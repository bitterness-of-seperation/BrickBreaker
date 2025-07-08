#pragma once

#include "Entities/Entity.h"

class Paddle : public Entity {
private:
    float maxSpeed;
    float moveDirection;
    float windowWidth;  // 添加窗口宽度属性

public:
    Paddle();
    Paddle(const sf::Vector2f& pos, const sf::Vector2f& size);

    void update(float deltaTime) override;
    void onCollision(Entity* other) override;
    
    void move(float direction);
    void stop();
    
    void setMaxSpeed(float speed);
    float getMaxSpeed() const;
    
    // 添加设置窗口宽度的方法
    void setWindowWidth(float width);
};