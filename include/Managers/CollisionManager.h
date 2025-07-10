#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "Entities/Entity.h"
#include "Entities/Ball.h"
#include "Entities/Brick.h"
#include "Entities/Paddle.h"

class CollisionManager {
private:
    // 游戏窗口尺寸
    sf::Vector2u windowSize;
    
    // 回调函数
    std::function<void(Brick*)> onBrickHitCallback;
    std::function<void()> onBallPaddleCollisionCallback;
    
    // 检测球与窗口边界的碰撞
    void checkBallWindowCollision(Ball* ball);
    
    // 检测两个实体之间的碰撞
    bool checkEntityCollision(Entity* a, Entity* b);
    
    // 处理球与砖块的碰撞
    void handleBallBrickCollision(Ball* ball, Brick* brick);
    
    // 处理球与挡板的碰撞
    void handleBallPaddleCollision(Ball* ball, Paddle* paddle);

public:
    CollisionManager();
    CollisionManager(const sf::Vector2u& windowSize);
    
    // 设置窗口尺寸
    void setWindowSize(const sf::Vector2u& size);
    
    // 设置回调函数
    void setOnBrickHitCallback(std::function<void(Brick*)> callback);
    void setOnBallPaddleCollisionCallback(std::function<void()> callback);
    
    // 更新所有碰撞检测
    void update(Ball* ball, Paddle* paddle, std::vector<std::unique_ptr<Brick>>& bricks);
    
    // 更新多球碰撞检测
    void update(std::vector<std::unique_ptr<Ball>>& balls, Paddle* paddle, std::vector<std::unique_ptr<Brick>>& bricks);
    
    // 检测球是否掉落（游戏失败条件）
    bool isBallLost(const Ball* ball) const;
};