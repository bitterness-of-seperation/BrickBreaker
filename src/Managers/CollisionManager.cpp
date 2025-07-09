#include "Managers/CollisionManager.h"
#include "Managers/AssetManager.h"
#include <algorithm>
#include <cmath>

CollisionManager::CollisionManager() : windowSize(800, 600) {
}

CollisionManager::CollisionManager(const sf::Vector2u& windowSize) : windowSize(windowSize) {
}

void CollisionManager::setWindowSize(const sf::Vector2u& size) {
    windowSize = size;
}

void CollisionManager::update(Ball* ball, Paddle* paddle, std::vector<std::unique_ptr<Brick>>& bricks) {
    if (!ball || !ball->isActive() || !paddle || !paddle->isActive()) {
        return;
    }
    
    // 检测球与窗口边界的碰撞
    checkBallWindowCollision(ball);
    
    // 检测球与挡板的碰撞
    if (checkEntityCollision(ball, paddle)) {
        handleBallPaddleCollision(ball, paddle);
    }
    
    // 检测球与砖块的碰撞
    for (auto& brick : bricks) {
        if (brick->isActive() && checkEntityCollision(ball, brick.get())) {
            handleBallBrickCollision(ball, brick.get());
            break; // 一次只处理一个碰撞，避免多次反弹
        }
    }
}

void CollisionManager::checkBallWindowCollision(Ball* ball) {
    sf::Vector2f pos = ball->getPosition();
    sf::Vector2f vel = ball->getVelocity();
    float radius = ball->getRadius();
    
    // 左右边界碰撞
    if (pos.x <= 0) {
        ball->setPosition({radius, pos.y});
        ball->reverseX();
        // 播放球碰撞窗口的音效
        AssetManager::getInstance()->playSound("ball_windows");
    } else if (pos.x + radius * 2 >= windowSize.x) {
        ball->setPosition({windowSize.x - radius * 2, pos.y});
        ball->reverseX();
        // 播放球碰撞窗口的音效
        AssetManager::getInstance()->playSound("ball_windows");
    }
    
    // 上边界碰撞
    if (pos.y <= 0) {
        ball->setPosition({pos.x, 0});
        ball->reverseY();
        // 播放球碰撞窗口的音效
        AssetManager::getInstance()->playSound("ball_windows");
    }
    
    // 注意：下边界不反弹，这是游戏失败的条件
}

bool CollisionManager::checkEntityCollision(Entity* a, Entity* b) {
    if (!a || !b || !a->isActive() || !b->isActive()) {
        return false;
    }
    
    auto intersection = a->getBounds().findIntersection(b->getBounds());
    return intersection.has_value();
}

void CollisionManager::setOnBrickHitCallback(std::function<void(Brick*)> callback) {
    onBrickHitCallback = callback;
}

void CollisionManager::setOnBallPaddleCollisionCallback(std::function<void()> callback) {
    onBallPaddleCollisionCallback = callback;
}

void CollisionManager::handleBallBrickCollision(Ball* ball, Brick* brick) {
    if (!ball || !brick) return;
    
    // 获取球和砖块的位置和大小
    sf::FloatRect ballBounds = ball->getBounds();
    sf::FloatRect brickBounds = brick->getBounds();
    
    // 计算碰撞深度
    float overlapLeft = ballBounds.position.x + ballBounds.size.x - brickBounds.position.x;
    float overlapRight = brickBounds.position.x + brickBounds.size.x - ballBounds.position.x;
    float overlapTop = ballBounds.position.y + ballBounds.size.y - brickBounds.position.y;
    float overlapBottom = brickBounds.position.y + brickBounds.size.y - ballBounds.position.y;
    
    // 找出最小的重叠，确定碰撞方向
    float minOverlapX = std::min(overlapLeft, overlapRight);
    float minOverlapY = std::min(overlapTop, overlapBottom);
    
    // 根据碰撞方向反弹球
    if (minOverlapX < minOverlapY) {
        // 水平碰撞（左右）
        ball->reverseX();
    } else {
        // 垂直碰撞（上下）
        ball->reverseY();
    }
    
    // 通知砖块被击中
    brick->onCollision(ball);
    
    // 通知球碰撞到了砖块
    ball->onCollision(brick);
    
    // 调用回调函数
    if (onBrickHitCallback && !brick->isActive()) {
        onBrickHitCallback(brick);
    }
}

void CollisionManager::handleBallPaddleCollision(Ball* ball, Paddle* paddle) {
    if (!ball || !paddle) return;
    
    // 球向下移动时才处理与挡板的碰撞（避免球卡在挡板上）
    if (ball->getVelocity().y > 0) {
        // 获取球和挡板的位置
        sf::Vector2f ballPos = ball->getPosition();
        sf::Vector2f paddlePos = paddle->getPosition();
        sf::Vector2f paddleSize = paddle->getSize();
        
        // 计算球相对于挡板的位置（-1到1之间，0为中心）
        float hitFactor = (ballPos.x + ball->getRadius() - paddlePos.x) / paddleSize.x;
        hitFactor = std::max(-1.0f, std::min(1.0f, 2 * hitFactor - 1));
        
        // 计算反弹角度（-60度到60度）
        float angle = hitFactor * 60.0f;
        float radian = angle * 3.14159f / 180.0f;
        
        // 计算新的速度向量
        float ballSpeed = std::sqrt(ball->getVelocity().x * ball->getVelocity().x + 
                                    ball->getVelocity().y * ball->getVelocity().y);
        
        sf::Vector2f newVelocity(
            std::sin(radian) * ballSpeed,
            -std::abs(std::cos(radian) * ballSpeed) // 确保球总是向上反弹
        );
        
        ball->setVelocity(newVelocity);
        
        // 确保球在挡板上方
        float newY = paddlePos.y - 2 * ball->getRadius();
        ball->setPosition({ballPos.x, newY});
        
        // 通知实体发生了碰撞
        ball->onCollision(paddle);
        paddle->onCollision(ball);
        
        // 调用回调函数
        if (onBallPaddleCollisionCallback) {
            onBallPaddleCollisionCallback();
        }
    }
}

bool CollisionManager::isBallLost(const Ball* ball) const {
    if (!ball) return true;
    
    // 如果球的Y坐标超过窗口高度，则认为球已经掉落
    return ball->getPosition().y > windowSize.y;
}
