#include "Entities/Paddle.h"
#include "Entities/Ball.h"
#include <cmath>

Paddle::Paddle() 
    : Entity(), 
      maxSpeed(500.0f),
      moveDirection(0.0f) {
    speed = 500.0f;
}

Paddle::Paddle(const sf::Vector2f& pos, const sf::Vector2f& size)
    : Entity(pos, size), 
      maxSpeed(500.0f),
      moveDirection(0.0f) {
    speed = 500.0f;
}

void Paddle::update(float deltaTime) {
    if (!active) return;
    
    // 根据移动方向更新位置
    position.x += moveDirection * speed * deltaTime;
    
    // 使用父类的setPosition方法，会正确处理sprite指针
    setPosition(position);
}

void Paddle::onCollision(Entity* other) {
    // 当球与挡板碰撞时的特殊处理
    Ball* ball = dynamic_cast<Ball*>(other);
    if (ball) {
        // 根据球击中挡板的位置来改变球的反弹角度
        float ballCenterX = ball->getPosition().x + ball->getSize().x / 2;
        float paddleCenterX = position.x + size.x / 2;
        
        // 计算球击中挡板位置的相对偏移量（-1.0到1.0之间）
        float hitFactor = (ballCenterX - paddleCenterX) / (size.x / 2);
        
        // 根据击中位置计算新的速度方向
        float angle = hitFactor * 60.0f; // -60度到60度
        
        // 将角度转换为弧度
        float radian = angle * 3.14159f / 180.0f;
        
        // 计算新的速度向量
        float ballSpeed = std::sqrt(ball->getVelocity().x * ball->getVelocity().x + 
                                   ball->getVelocity().y * ball->getVelocity().y);
        
        sf::Vector2f newVelocity(
            std::sin(radian) * ballSpeed,
            -std::cos(radian) * ballSpeed  // 负号确保球向上反弹
        );
        
        ball->setVelocity(newVelocity);
    }
}

void Paddle::move(float direction) {
    // 限制方向值在-1到1之间
    moveDirection = std::max(-1.0f, std::min(1.0f, direction));
}

void Paddle::stop() {
    moveDirection = 0.0f;
}

void Paddle::setMaxSpeed(float speed) {
    maxSpeed = speed;
    this->speed = speed;
}

float Paddle::getMaxSpeed() const {
    return maxSpeed;
}