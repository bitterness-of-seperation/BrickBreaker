#include "States/PlayState.h"
#include "Game.h"
#include "States/PauseState.h"
#include "States/GameOverState.h"
#include "Managers/AssetManager.h"
#include "Utils/Config.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <filesystem>

PlayState::PlayState(Game* game) 
    : GameState(game),
      score(0),
      lives(Config::getInstance().getValue("game.initial_lives", 3)),
      ballLaunched(false),
      gameOver(false),
      levelCompleted(false),
      justGameOver(false),
      paddleMovingLeft(false),
      paddleMovingRight(false) {
    // 加载奖励机制设置
    loadRewardSettings();
}

void PlayState::init() { //设置碰撞管理、关卡管理和初始化游戏
    // Get window size
    sf::Vector2u windowSize = game->getWindow().getSize();
    
    // Set collision manager
    collisionManager.setWindowSize(windowSize);
    
    // Set collision callbacks
    collisionManager.setOnBrickHitCallback([this](Brick* brick) {
        if (brick) {
            this->addScore(brick->getScore());
            AssetManager::getInstance()->playSound("break");
        }
    });
    
    collisionManager.setOnBallPaddleCollisionCallback([this]() {
        AssetManager::getInstance()->playSound("hit");
    });
    
    // Initialize level manager
    std::vector<std::string> levelFiles = {
        "resources/levels/level1.txt",
        "resources/levels/level2.txt",
        "resources/levels/level3.txt"
    };
    
    // 确保关卡区域不会超出屏幕宽度
    float levelWidth = windowSize.x - 100.0f; // 左右各留50像素边距
    
    levelManager.init(levelFiles, 
                      sf::Vector2f(50.0f, 50.0f), 
                      sf::Vector2f(levelWidth, 200.0f));
    
    // 设置砖块尺寸和间距
    levelManager.setBrickSize(sf::Vector2f(70.0f, 30.0f));
    levelManager.setBrickPadding(sf::Vector2f(2.0f, 2.0f));
    
    // 加载字体
    try {
        if (AssetManager::getInstance()->hasFont("arial")) {
            font = AssetManager::getInstance()->getFont("arial");
            
            // Initialize UI text - SFML 3.0.0 requires font in constructor
            scoreText = std::make_unique<sf::Text>(font, "Score: 0", 24);
            scoreText->setFillColor(sf::Color::White);
            scoreText->setPosition(sf::Vector2f(10.0f, 10.0f));
            
            livesText = std::make_unique<sf::Text>(font, "Lives: " + std::to_string(lives), 24);
            livesText->setFillColor(sf::Color::White);
            livesText->setPosition(sf::Vector2f(10.0f, 40.0f));
            
            messageText1 = std::make_unique<sf::Text>(font, "Press Space to Launch Ball", 30);
            messageText1->setFillColor(sf::Color::Yellow);
            messageText1->setStyle(sf::Text::Bold);
            
            // Center message text
            sf::FloatRect messageBounds = messageText1->getLocalBounds();
            messageText1->setOrigin({messageBounds.size.x / 2.0f, messageBounds.size.y / 2.0f});
            messageText1->setPosition(sf::Vector2f(
                windowSize.x / 2.0f,
                windowSize.y * 0.7f
            ));

            messageText2 = std::make_unique<sf::Text>(font, "", 30);
            messageText2->setFillColor(sf::Color::Yellow);
            messageText2->setStyle(sf::Text::Bold);
            
            // Center messageText2
            sf::FloatRect messageBounds2 = messageText2->getLocalBounds();
            messageText2->setOrigin({messageBounds2.size.x / 2.0f, messageBounds2.size.y / 2.0f});
            messageText2->setPosition(sf::Vector2f(
                windowSize.x / 2.0f,
                windowSize.y * 0.75f  // 稍微低于messageText1
            ));
            
        } else {
            std::cerr << "Warning: Could not load font, using default font" << std::endl;
        }
    } catch (const std::exception& e) {
                    std::cerr << "Error loading font: " << e.what() << std::endl;
    }
    
    // 尝试加载存档，如果失败则初始化新游戏
    std::cout << "Attempting to load game state..." << std::endl;
    if (!loadGameState()) {
        std::cout << "Failed to load game state, initializing new game" << std::endl;
        initGame();
    } else {
        std::cout << "Game state loaded successfully" << std::endl;
    }
}

void PlayState::initGame() { //创建实体和更新ui
    // Get window size
    sf::Vector2u windowSize = game->getWindow().getSize();
    
    // Create paddle
    paddle = std::make_unique<Paddle>(
        sf::Vector2f((windowSize.x - 100.0f) / 2.0f, windowSize.y - 50.0f),
        sf::Vector2f(100.0f, 20.0f)
    );
    
    // Set paddle window width for boundary checking
    paddle->setWindowWidth(static_cast<float>(windowSize.x));
    
    // Set paddle speed from config
    paddle->setMaxSpeed(Config::getInstance().getValue("game.paddle_speed", 500.0f));
    
    // Set paddle texture
    if (AssetManager::getInstance()->hasTexture("paddle")) {
        paddle->setTexture(AssetManager::getInstance()->getTexture("paddle"));
    }
    
    // 清空现有的球
    balls.clear();
    
    // 创建初始球
    auto initialBall = createNewBall(
        sf::Vector2f(windowSize.x / 2.0f - 10.0f, windowSize.y - 80.0f),
        sf::Vector2f(0.0f, 0.0f)
    );
    
    // Load first level
    GameState::currentLevel = 0;
    loadLevel(GameState::currentLevel);
    
    // Reset state
    ballLaunched = false;
    gameOver = false;
    levelCompleted = false;
    score = 0;
    lives = Config::getInstance().getValue("game.initial_lives", 3);
    justGameOver = false;
    
    // Update UI
    updateUI();
}

void PlayState::handleInput(const sf::Event& event) { //输入事件
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* keyEvent = event.getIf<sf::Event::KeyPressed>();
        if (keyEvent) {
            switch (keyEvent->code) {
                case sf::Keyboard::Key::Enter:
                case sf::Keyboard::Key::Space:
                    if (!ballLaunched && !gameOver && !levelCompleted) {
                        launchBall();
                    } else if (levelCompleted && levelManager.hasNextLevel()) {
                        // Load next level
                        loadNextLevel();
                    } 
                    break;
                
                case sf::Keyboard::Key::P:
                    game->pushState(std::make_unique<PauseState>(game));
                    break;
                
            default:
                break;
        }
        }
    }
    
    // 处理键盘按下事件
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* keyEvent = event.getIf<sf::Event::KeyPressed>();
        if (keyEvent && !gameOver && !levelCompleted) {
            // 设置挡板移动标志
            if (keyEvent->code == sf::Keyboard::Key::Left || keyEvent->code == sf::Keyboard::Key::A) {
                paddleMovingLeft = true;
            }
            if (keyEvent->code == sf::Keyboard::Key::Right || keyEvent->code == sf::Keyboard::Key::D) {
                paddleMovingRight = true;
            }
        }
    }
    
    // 处理键盘释放事件
    if (event.is<sf::Event::KeyReleased>()) {
        const auto* keyEvent = event.getIf<sf::Event::KeyReleased>();
        if (keyEvent) {
            // 清除挡板移动标志
            if (keyEvent->code == sf::Keyboard::Key::Left || keyEvent->code == sf::Keyboard::Key::A) {
                paddleMovingLeft = false;
            }
            if (keyEvent->code == sf::Keyboard::Key::Right || keyEvent->code == sf::Keyboard::Key::D) {
                paddleMovingRight = false;
            }
        }
    }
}

void PlayState::update(float deltaTime) { //更新实体和ui
    // 如果游戏刚结束，切换到GameOverState
    if (gameOver && !justGameOver) {
        justGameOver = true;
        game->pushState(std::make_unique<GameOverState>(game, score));
        return;
    }
    
    if (gameOver || levelCompleted) {
        return;
    }
    
    // 如果没有球被发射，让第一个球跟随挡板
    if (!ballLaunched && !balls.empty()) {
        balls[0]->setPosition(sf::Vector2f(
            paddle->getPosition().x + paddle->getSize().x / 2.0f - balls[0]->getRadius(),
            paddle->getPosition().y - balls[0]->getRadius() * 2
        ));
    }
    
    // 更新挡板
    paddle->update(deltaTime);
    
    // 根据移动标志更新挡板位置
    if (!gameOver && !levelCompleted) {
        float paddleSpeed = paddle->getMaxSpeed();
        if (paddleMovingLeft) {
            movePaddle(-paddleSpeed * deltaTime);
        }
        if (paddleMovingRight) {
            movePaddle(paddleSpeed * deltaTime);
        }
    }
    
    if (ballLaunched) {
        // 更新所有球
        for (auto it = balls.begin(); it != balls.end();) {
            auto& ball = *it;
            if (ball->isActive()) {
        ball->update(deltaTime);
                // 检查球是否掉落
        if (collisionManager.isBallLost(ball.get())) {
                    // 移除掉落的球
                    it = balls.erase(it);
                    continue;
                }
                ++it;
            } else {
                it = balls.erase(it);
            }
        }
        
        // 检查碰撞
        collisionManager.update(balls, paddle.get(), bricks);
        
        // 检查是否所有球都消失了
        if (balls.empty()) {
            lives--;
            if (lives <= 0) {
                gameOver = true;
                if (messageText1) {
                    messageText1->setString("Game Over! Press ESC for Menu");
                    // 重新计算并设置原点以居中
                    sf::FloatRect messageBounds = messageText1->getLocalBounds();
                    messageText1->setOrigin({messageBounds.size.x / 2.0f, messageBounds.size.y / 2.0f});
                    
                    // 获取窗口大小并设置位置
                    sf::Vector2u windowSize = game->getWindow().getSize();
                    messageText1->setPosition(sf::Vector2f(
                        windowSize.x / 2.0f,
                        windowSize.y * 0.7f
                    ));
                }
            } else {
                resetBallAndPaddle();
            }
            updateUI();
        }
    }
    
    // Update bricks
    for (auto& brick : bricks) {
        if (brick->isActive()) {
            brick->update(deltaTime);
        }
    }
    
    // Check if all bricks are destroyed
    checkGameStatus();
}

void PlayState::render(sf::RenderWindow& window) { //渲染实体和ui
    // 绘制背景图片
    if (AssetManager::getInstance()->hasTexture("background")) {
        sf::Sprite backgroundSprite(AssetManager::getInstance()->getTexture("background"));
        
        // 调整背景图片大小以适应窗口
        sf::Vector2u windowSize = window.getSize();
        sf::Vector2u textureSize = backgroundSprite.getTexture().getSize();
        
        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        backgroundSprite.setScale({scaleX, scaleY});
        
        window.draw(backgroundSprite);
    } else {
        // 如果没有背景图片，使用配置中的颜色
        sf::Color bgColor = Config::getInstance().getValue("colors.background", sf::Color(20, 20, 50));
        window.clear(bgColor);
    }
    
    // Draw entities
    paddle->render(window);
    
    // 渲染所有球
    for (auto& ball : balls) {
        if (ball->isActive()) {
    ball->render(window);
        }
    }
    
    for (auto& brick : bricks) {
        if (brick->isActive()) {
            brick->render(window);
        }
    }
    
    // Draw UI
    if (scoreText) window.draw(*scoreText);
    if (livesText) window.draw(*livesText);
    if (messageText1) window.draw(*messageText1);
    if (messageText2) window.draw(*messageText2);
}

void PlayState::launchBall() {
    if (!ballLaunched && !balls.empty()) {
        ballLaunched = true;
        
        // Get ball speed from config
        float ballSpeed = Config::getInstance().getValue("game.ball_speed", 300.0f);
        
        // Calculate initial velocity components
        float vx = ballSpeed * 0.5f; // Horizontal component (adjust as needed)
        float vy = -ballSpeed;       // Vertical component (negative to go up)
        
        // 设置第一个球的速度
        balls[0]->setVelocity(sf::Vector2f(vx, vy));
        if (messageText1) messageText1->setString("");
    }
}

void PlayState::movePaddle(float direction) {
    if (paddle) {
        paddle->move(direction);
    }
}

void PlayState::resetBallAndPaddle() {
    if (!paddle) return;
    
    // 清空所有球
    balls.clear();
    
    // 创建新的初始球
    sf::Vector2u windowSize = game->getWindow().getSize();
    createNewBall(
        sf::Vector2f(windowSize.x / 2.0f - 10.0f, windowSize.y - 80.0f),
        sf::Vector2f(0, 0)
    );
    
    ballLaunched = false;
    
    // Reset paddle position
    paddle->setPosition(sf::Vector2f(
        (windowSize.x - paddle->getSize().x) / 2.0f,
        windowSize.y - 50.0f
    ));
    
    if (messageText1) {
        messageText1->setString("Press Space to Launch Ball");
        // 重新计算并设置原点以居中
        sf::FloatRect messageBounds = messageText1->getLocalBounds();
        messageText1->setOrigin({messageBounds.size.x / 2.0f, messageBounds.size.y / 2.0f});
        
        // 获取窗口大小并设置位置
        messageText1->setPosition(sf::Vector2f(
            windowSize.x / 2.0f,
            windowSize.y * 0.7f
        ));
    }
    
    // 清空第二个消息
    if (messageText2) {
        messageText2->setString("");
    }
}

void PlayState::checkGameStatus() {
    // Check if all bricks are destroyed
    bool allBricksDestroyed = true;
    for (const auto& brick : bricks) {
        if (brick->isActive()) {
            allBricksDestroyed = false;
            break;
        }
    }
    
    if (allBricksDestroyed) {
        levelCompleted = true;
        if (levelManager.hasNextLevel()) {
            // 更新消息文本
            if (messageText1) {
                messageText1->setString("Level Complete!");
                // 重新计算并设置原点以居中
                sf::FloatRect messageBounds = messageText1->getLocalBounds();
                messageText1->setOrigin({messageBounds.size.x / 2.0f, messageBounds.size.y / 2.0f});
            }
            
            if (messageText2) {
                messageText2->setString("Press Space to Continue");
                // 重新计算并设置原点以居中
                sf::FloatRect messageBounds = messageText2->getLocalBounds();
                messageText2->setOrigin({messageBounds.size.x / 2.0f, messageBounds.size.y / 2.0f});
                
                // 获取窗口大小并设置位置
                sf::Vector2u windowSize = game->getWindow().getSize();
                messageText2->setPosition(sf::Vector2f(
                    windowSize.x / 2.0f,
                    windowSize.y * 0.75f  // 稍微低于messageText1
                ));
            }
        } else {
            game->pushState(std::make_unique<GameOverState>(game, score));
        }
    }
}

void PlayState::loadLevel(int levelNumber) {
    bricks = levelManager.loadLevel(levelNumber);
}

void PlayState::startNewGame() {
    initGame();
}

int PlayState::getScore() const {
    return score;
}

int PlayState::getLives() const {
    return lives;
}

bool PlayState::isGameOver() const {
    return gameOver;
}

bool PlayState::isLevelCompleted() const {
    return levelCompleted;
}

void PlayState::addScore(int points) {
    score += points;
    updateUI();
    
    // 尝试触发奖励机制
    if (ballLaunched && multiballEnabled) {
        // 尝试生成新球
        trySpawnNewBall();
    }
}

void PlayState::updateUI() {
    if (scoreText) scoreText->setString("Score: " + std::to_string(score));
    if (livesText) livesText->setString("Lives: " + std::to_string(lives));
}

void PlayState::loadNextLevel() {
    // Load the next level
    GameState::currentLevel = levelManager.getCurrentLevel();
    bricks = levelManager.loadLevel(GameState::currentLevel + 1);
    
    // Reset game state for new level
    levelCompleted = false;
    ballLaunched = false;
    
    // Maintain the current score but reset lives to initial value
    lives = Config::getInstance().getValue("game.initial_lives", 3);
    
    // Reset ball and paddle positions
    resetBallAndPaddle();
    
    // 保存游戏状态，以便在关卡切换后仍能恢复
    saveGameState();
    
    // Update message and UI
    if (messageText1) {
        messageText1->setString("Press Space to Launch Ball");
        // 重新计算并设置原点以居中
        sf::FloatRect messageBounds = messageText1->getLocalBounds();
        messageText1->setOrigin({messageBounds.size.x / 2.0f, messageBounds.size.y / 2.0f});
        
        // 获取窗口大小并设置位置
        sf::Vector2u windowSize = game->getWindow().getSize();
        messageText1->setPosition(sf::Vector2f(
            windowSize.x / 2.0f,
            windowSize.y * 0.7f
        ));
    }
    
    // 清空第二个消息
    if (messageText2) {
        messageText2->setString("");
    }
    
    updateUI();
}

void PlayState::restartGame() {
    // Reset score
    score = 0;
    
    // Reset lives to initial value from config
    lives = Config::getInstance().getValue("game.initial_lives", 3);
    
    // Reset game state
    gameOver = false;
    levelCompleted = false;
    ballLaunched = false;
    justGameOver = false;
    
    // Load first level
    GameState::currentLevel = 0;
    loadLevel(GameState::currentLevel);
    
    // Reset ball and paddle positions
    resetBallAndPaddle();
    
    // Update UI
    updateUI();
    
    // Update message
    if (messageText1) {
        messageText1->setString("Press Space to Launch Ball");
        
        // Reset message position (in case it was moved for congratulations message)
        sf::Vector2u windowSize = game->getWindow().getSize();
        sf::FloatRect messageBounds = messageText1->getLocalBounds();
        messageText1->setOrigin({messageBounds.size.x / 2.0f, messageBounds.size.y / 2.0f});
        messageText1->setPosition(sf::Vector2f(
            windowSize.x / 2.0f,
            windowSize.y * 0.7f
        ));
    }
}

void PlayState::loadRewardSettings() {
    // 从配置中加载奖励机制设置
    Config& config = Config::getInstance();
    
    // 多球功能始终启用
    multiballEnabled = true;
    maxBalls = config.getValue("reward.max_balls", 3);
    ballSpawnChance = config.getValue("reward.ball_spawn_chance", 30);
}

Ball* PlayState::createNewBall(const sf::Vector2f& position, const sf::Vector2f& velocity) {
    // 创建新球
    auto newBall = std::make_unique<Ball>(position, 10.0f);
    
    // 设置球的速度
    newBall->setVelocity(velocity);
    
    // 设置球的纹理
    if (AssetManager::getInstance()->hasTexture("ball")) {
        newBall->setTexture(AssetManager::getInstance()->getTexture("ball"));
    }
    
    // 获取指针并添加到球列表中
    Ball* ballPtr = newBall.get();
    balls.push_back(std::move(newBall));
    
    return ballPtr;
}

void PlayState::trySpawnNewBall() {
    // 如果多球功能未启用，直接返回
    if (!multiballEnabled) {
        return;
    }
    
    // 如果已经达到最大球数，直接返回
    if (balls.size() >= static_cast<size_t>(maxBalls)) {
        return;
    }
    
    // 根据概率决定是否生成新球
    int randomValue = rand() % 100;
    if (randomValue < ballSpawnChance) {
        // 如果有球存在，基于现有球生成新球
        if (!balls.empty()) {
            // 随机选择一个现有的球
            size_t ballIndex = rand() % balls.size();
            Ball* existingBall = balls[ballIndex].get();
            
            if (existingBall && existingBall->isActive()) {
                // 获取现有球的位置和速度
                sf::Vector2f position = existingBall->getPosition();
                sf::Vector2f velocity = existingBall->getVelocity();
                
                // 稍微调整新球的位置和速度方向
                position.x += (rand() % 20) - 10;
                position.y += (rand() % 20) - 10;
                
                // 计算新的速度向量（与原球方向略有不同）
                float angle = (rand() % 60 - 30) * 3.14159f / 180.0f; // -30到30度的随机角度
                float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
                
                // 旋转速度向量
                float cosA = cos(angle);
                float sinA = sin(angle);
                sf::Vector2f newVelocity(
                    velocity.x * cosA - velocity.y * sinA,
                    velocity.x * sinA + velocity.y * cosA
                );
                
                // 确保速度大小不变
                float newSpeed = sqrt(newVelocity.x * newVelocity.x + newVelocity.y * newVelocity.y);
                newVelocity = newVelocity * (speed / newSpeed);
                
                // 创建新球
                createNewBall(position, newVelocity);
            }
        }
    }
}

void PlayState::saveGameState() const {
    // 使用Config类保存游戏状态
    Config& config = Config::getInstance();
    
    // 保存游戏状态
    config.setValue("save.score", score);
    config.setValue("save.lives", lives);
    config.setValue("save.level", levelManager.getCurrentLevel());
    GameState::currentLevel = levelManager.getCurrentLevel(); // 更新静态变量
    config.setValue("save.ball_launched", ballLaunched);
    
    // 保存挡板位置
    if (paddle) {
        config.setValue("save.paddle_x", paddle->getPosition().x);
        config.setValue("save.paddle_y", paddle->getPosition().y);
    }
    
    // 保存球的数量和位置
    config.setValue("save.ball_count", static_cast<int>(balls.size()));
    for (size_t i = 0; i < balls.size() && i < 10; ++i) { // 最多保存10个球
        if (balls[i]) {
            std::string prefix = "save.ball" + std::to_string(i) + "_";
            config.setValue(prefix + "x", balls[i]->getPosition().x);
            config.setValue(prefix + "y", balls[i]->getPosition().y);
            config.setValue(prefix + "vx", balls[i]->getVelocity().x);
            config.setValue(prefix + "vy", balls[i]->getVelocity().y);
        }
    }
    
    // 保存砖块状态
    config.setValue("save.brick_count", static_cast<int>(bricks.size()));
    for (size_t i = 0; i < bricks.size() && i < 100; ++i) { // 最多保存100个砖块
        if (bricks[i]) {
            std::string prefix = "save.brick" + std::to_string(i) + "_";
            config.setValue(prefix + "active", bricks[i]->isActive());
        }
    }
    
    // 验证要保存的数据
    std::cout << "Verifying save data before writing..." << std::endl;
    std::cout << "Score: " << score << std::endl;
    std::cout << "Lives: " << lives << std::endl;
    std::cout << "Level: " << levelManager.getCurrentLevel() << std::endl;
    std::cout << "Ball count: " << balls.size() << std::endl;
    
    if (paddle) {
        std::cout << "Paddle position: (" << paddle->getPosition().x << ", " 
                  << paddle->getPosition().y << ")" << std::endl;
    } else {
        std::cerr << "Warning: Paddle is null when saving game state" << std::endl;
    }
    
    // 保存到文件
    std::cout << "Attempting to save game state to save.ini..." << std::endl;
    std::cout << "Current path: " << std::filesystem::current_path().string() << std::endl;
    
    bool saved = config.save("save.ini");
    if (saved) {
        std::cout << "Game state successfully saved to save.ini" << std::endl;
        
        // 验证文件是否确实创建
        std::ifstream checkFile("save.ini");
        if (checkFile.good()) {
            std::cout << "Verified: save.ini exists and is readable" << std::endl;
            
            // 读取文件内容进行验证
            std::string line;
            int lineCount = 0;
            while (std::getline(checkFile, line) && lineCount < 10) {
                std::cout << "save.ini line " << lineCount << ": " << line << std::endl;
                lineCount++;
            }
            
            checkFile.close();
        } else {
            std::cerr << "Warning: save.ini was reported as saved but cannot be opened for reading" << std::endl;
        }
    } else {
        std::cerr << "Failed to save game state to save.ini" << std::endl;
    }
}

bool PlayState::loadGameState() {
    try {
        // 尝试加载存档文件
        Config& config = Config::getInstance();
        
        // 检查save.ini文件是否存在
        std::ifstream checkFile("save.ini");
        if (!checkFile.good() || !config.load("save.ini")) {
            std::cout << "Save file does not exist or failed to load, creating new game" << std::endl;
            return false; // 加载失败
        }
        
        // 检查存档文件是否包含必要的数据
        if (!config.hasValue("save.level") || !config.hasValue("save.lives")) {
            std::cout << "Save file incomplete, creating new game" << std::endl;
            return false;
        }
        
        // 加载基本游戏状态
        score = config.getValue("save.score", 0);
        lives = config.getValue("save.lives", 3);
        GameState::currentLevel = config.getValue("save.level", 0);
        ballLaunched = config.getValue("save.ball_launched", false);
        
        // 加载关卡
        loadLevel(GameState::currentLevel);
        
        // 设置挡板位置
        float paddleX = 0.0f, paddleY = 0.0f;
        try {
            paddleX = config.getValue("save.paddle_x", 350.0f);
            paddleY = config.getValue("save.paddle_y", 450.0f);
            std::cout << "Paddle position from save: (" << paddleX << ", " << paddleY << ")" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading paddle position: " << e.what() << std::endl;
            // 使用默认值
            sf::Vector2u windowSize = game->getWindow().getSize();
            paddleX = (windowSize.x - 100.0f) / 2.0f;
            paddleY = windowSize.y - 50.0f;
        }
        
        // 检查位置是否有效
        sf::Vector2u windowSize = game->getWindow().getSize();
        if (paddleX < 0 || paddleX > windowSize.x || paddleY < 0 || paddleY > windowSize.y) {
            std::cout << "Paddle position out of bounds, using default position" << std::endl;
            paddleX = (windowSize.x - 100.0f) / 2.0f;
            paddleY = windowSize.y - 50.0f;
        }
        
        if (paddle) {
            paddle->setPosition(sf::Vector2f(paddleX, paddleY));
        } else {
            std::cerr << "Error: Paddle is null when trying to set position" << std::endl;
            // 创建新的挡板
            paddle = std::make_unique<Paddle>(
                sf::Vector2f(paddleX, paddleY),
                sf::Vector2f(100.0f, 20.0f)
            );
            paddle->setWindowWidth(static_cast<float>(windowSize.x));
            paddle->setMaxSpeed(Config::getInstance().getValue("game.paddle_speed", 500.0f));
            if (AssetManager::getInstance()->hasTexture("paddle")) {
                paddle->setTexture(AssetManager::getInstance()->getTexture("paddle"));
            }
        }
        
        // 清空现有的球
        balls.clear();
        
        // 加载球
        int ballCount = config.getValue("save.ball_count", 0);
        std::cout << "Loading " << ballCount << " balls from save file" << std::endl;
        
        // 确保至少有一个球
        if (ballCount <= 0) {
            ballCount = 1;
            std::cout << "No balls found in save file, creating default ball" << std::endl;
        }
        
        for (int i = 0; i < ballCount && i < 10; ++i) {
            std::string prefix = "save.ball" + std::to_string(i) + "_";
            
            // 获取球的位置和速度，如果出错则使用默认值
            float x = 0.0f, y = 0.0f, vx = 0.0f, vy = 0.0f;
            try {
                x = config.getValue(prefix + "x", 400.0f);
                y = config.getValue(prefix + "y", 300.0f);
                vx = config.getValue(prefix + "vx", 0.0f);
                vy = config.getValue(prefix + "vy", 0.0f);
                
                std::cout << "Ball " << i << " position: (" << x << ", " << y 
                          << "), velocity: (" << vx << ", " << vy << ")" << std::endl;
            }
            catch (const std::exception& e) {
                std::cerr << "Error loading ball " << i << ": " << e.what() << std::endl;
                // 使用默认值
                x = 400.0f;
                y = 300.0f;
                vx = 0.0f;
                vy = 0.0f;
            }
            
            // 检查位置是否有效
            sf::Vector2u windowSize = game->getWindow().getSize();
            if (x < 0 || x > windowSize.x || y < 0 || y > windowSize.y) {
                std::cout << "Ball " << i << " position out of bounds, using default position" << std::endl;
                x = windowSize.x / 2.0f;
                y = windowSize.y - 80.0f;
            }
            
            createNewBall(sf::Vector2f(x, y), sf::Vector2f(vx, vy));
        }
        
        // 如果没有球，创建一个默认的球
        if (balls.empty()) {
            sf::Vector2u windowSize = game->getWindow().getSize();
            createNewBall(
                sf::Vector2f(windowSize.x / 2.0f - 10.0f, windowSize.y - 80.0f),
                sf::Vector2f(0.0f, 0.0f)
            );
        }
        
        // 加载砖块状态
        int brickCount = config.getValue("save.brick_count", 0);
        if (brickCount > 0 && !bricks.empty()) {
            for (int i = 0; i < brickCount && i < 100 && i < bricks.size(); ++i) {
                std::string prefix = "save.brick" + std::to_string(i) + "_";
                bool active = config.getValue(prefix + "active", true);
                if (bricks[i] && !active) {
                    bricks[i]->setActive(false);
                }
            }
        }
        
        // 更新UI
        updateUI();
        
        std::cout << "Save file loaded successfully!" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception while loading save file: " << e.what() << std::endl;
        // 如果加载过程中出错，重置游戏状态
        balls.clear();
        sf::Vector2u windowSize = game->getWindow().getSize();
        createNewBall(
            sf::Vector2f(windowSize.x / 2.0f - 10.0f, windowSize.y - 80.0f),
            sf::Vector2f(0.0f, 0.0f)
        );
        return false;
    }
}

