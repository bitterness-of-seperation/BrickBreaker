#pragma once

#include "GameState.h"
#include "Entities/Ball.h"
#include "Entities/Paddle.h"
#include "Entities/Brick.h"
#include "Managers/CollisionManager.h"
#include "Managers/LevelManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Game;

class PlayState : public GameState {
private:
    // 游戏实体
    std::unique_ptr<Ball> ball;
    std::unique_ptr<Paddle> paddle;
    std::vector<std::unique_ptr<Brick>> bricks;
    
    // 管理器
    CollisionManager collisionManager;
    LevelManager levelManager;
    
    // 游戏状态
    int score;
    int lives;
    bool ballLaunched;//球发射
    bool gameOver;
    bool levelCompleted;
    bool justGameOver; // 标记游戏是否刚刚结束
    
    // UI元素
    sf::Font font;
    std::unique_ptr<sf::Text> scoreText;   // 使用指针避免默认构造函数
    std::unique_ptr<sf::Text> livesText;   // 使用指针避免默认构造函数
    std::unique_ptr<sf::Text> messageText; // 使用指针避免默认构造函数
    
    // 初始化游戏
    void initGame();
    
    // 重置球和挡板位置
    void resetBallAndPaddle();
    
    // 检查游戏状态
    void checkGameStatus();
    
    // 更新UI
    void updateUI();
    
    // 加载关卡
    void loadLevel(int levelNumber);
    
    // 加载下一关
    void loadNextLevel();
    
    // 重新开始游戏
    void restartGame();

public:
    PlayState(Game* game);
    virtual ~PlayState() = default;
    
    // 重写GameState的虚函数
    void init() override;
    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    // 游戏控制
    void launchBall();
    void movePaddle(float direction);
    
    // 启动新的一局
    void startNewGame();
    
    // 获取游戏状态
    int getScore() const;
    int getLives() const;
    bool isGameOver() const;
    bool isLevelCompleted() const;

    // 添加分数
    void addScore(int points);
};