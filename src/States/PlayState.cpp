#include "States/PlayState.h"
#include "Game.h"
#include "States/PauseState.h"
#include "States/GameOverState.h"
#include "Managers/AssetManager.h"
#include "Utils/Config.h"
#include <iostream>

PlayState::PlayState(Game* game) 
    : GameState(game),
      score(0),
      lives(Config::getInstance().getValue("game.initial_lives", 3)),
      ballLaunched(false),
      gameOver(false),
      levelCompleted(false),
      justGameOver(false) {
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
            
            messageText = std::make_unique<sf::Text>(font, "Press Space to Launch Ball", 30);
            messageText->setFillColor(sf::Color::Yellow);
            messageText->setStyle(sf::Text::Bold);
            
            // Center message text
            sf::FloatRect messageBounds = messageText->getLocalBounds();
            messageText->setOrigin({messageBounds.size.x / 2.0f, messageBounds.size.y / 2.0f});
            messageText->setPosition(sf::Vector2f(
                windowSize.x / 2.0f,
                windowSize.y * 0.7f
            ));
        } else {
            std::cerr << "Warning: Could not load font, using default font" << std::endl;
        }
    } catch (const std::exception& e) {
                    std::cerr << "Error loading font: " << e.what() << std::endl;
    }
    
    // Initialize game
    initGame();
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
    
    // Create ball
    ball = std::make_unique<Ball>(
        sf::Vector2f(windowSize.x / 2.0f - 10.0f, windowSize.y - 80.0f),
        10.0f
    );
    
    // Set ball texture
    if (AssetManager::getInstance()->hasTexture("ball")) {
        ball->setTexture(AssetManager::getInstance()->getTexture("ball"));
    }
    
    // Load first level
    loadLevel(0);
    
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
    
    // Handle continuous key input
    if (!gameOver && !levelCompleted) {
        // Get configured paddle speed and key bindings
        float paddleSpeed = paddle->getMaxSpeed();
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            movePaddle(-paddleSpeed * game->getDeltaTime());
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            movePaddle(paddleSpeed * game->getDeltaTime());
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
    
    // If ball not launched, make it follow the paddle
    if (!ballLaunched) {
        ball->setPosition(sf::Vector2f(
            paddle->getPosition().x + paddle->getSize().x / 2.0f - ball->getRadius(),
            paddle->getPosition().y - ball->getRadius() * 2
        ));
    }
    
    // Update entities
    paddle->update(deltaTime);
    
    if (ballLaunched) {
        ball->update(deltaTime);
        
        // Check collisions
        collisionManager.update(ball.get(), paddle.get(), bricks);
        
        // Check if ball is lost
        if (collisionManager.isBallLost(ball.get())) {
            lives--;
            if (lives <= 0) {
                gameOver = true;
                if (messageText) messageText->setString("Game Over! Press ESC for Menu");
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
    ball->render(window);
    
    for (auto& brick : bricks) {
        if (brick->isActive()) {
            brick->render(window);
        }
    }
    
    // Draw UI
    if (scoreText) window.draw(*scoreText);
    if (livesText) window.draw(*livesText);
    if (messageText) window.draw(*messageText);
}

void PlayState::launchBall() {
    if (!ballLaunched) {
        ballLaunched = true;
        
        // Get ball speed from config
        float ballSpeed = Config::getInstance().getValue("game.ball_speed", 300.0f);
        
        // Calculate initial velocity components
        float vx = ballSpeed * 0.5f; // Horizontal component (adjust as needed)
        float vy = -ballSpeed;       // Vertical component (negative to go up)
        
        ball->setVelocity(sf::Vector2f(vx, vy));
        if (messageText) messageText->setString("");
    }
}

void PlayState::movePaddle(float direction) {
    if (paddle) {
        paddle->move(direction);
    }
}

void PlayState::resetBallAndPaddle() {
    if (!ball || !paddle) return;
    
    // Reset ball position
    sf::Vector2u windowSize = game->getWindow().getSize();
    ball->setPosition(sf::Vector2f(
        windowSize.x / 2.0f - ball->getRadius(),
        windowSize.y - 80.0f
    ));
    ball->setVelocity(sf::Vector2f(0, 0));
    ballLaunched = false;
    
    // Reset paddle position
    paddle->setPosition(sf::Vector2f(
        (windowSize.x - paddle->getSize().x) / 2.0f,
        windowSize.y - 50.0f
    ));
    
    if (messageText) messageText->setString("Press Space to Launch Ball");
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
            if (messageText) messageText->setString("Level Complete! Press Space to Continue");
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
}

void PlayState::updateUI() {
    if (scoreText) scoreText->setString("Score: " + std::to_string(score));
    if (livesText) livesText->setString("Lives: " + std::to_string(lives));
}

void PlayState::loadNextLevel() {
    // Load the next level
    int currentLevel = levelManager.getCurrentLevel();
    bricks = levelManager.loadLevel(currentLevel + 1);
    
    // Reset game state for new level
    levelCompleted = false;
    ballLaunched = false;
    
    // Maintain the current score but reset lives to initial value
    lives = Config::getInstance().getValue("game.initial_lives", 3);
    
    // Reset ball and paddle positions
    resetBallAndPaddle();
    
    // Update message and UI
    if (messageText) messageText->setString("Press Space to Launch Ball");
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
    loadLevel(0);
    
    // Reset ball and paddle positions
    resetBallAndPaddle();
    
    // Update UI
    updateUI();
    
    // Update message
    if (messageText) {
        messageText->setString("Press Space to Launch Ball");
        
        // Reset message position (in case it was moved for congratulations message)
        sf::Vector2u windowSize = game->getWindow().getSize();
        sf::FloatRect messageBounds = messageText->getLocalBounds();
        messageText->setOrigin({messageBounds.size.x / 2.0f, messageBounds.size.y / 2.0f});
        messageText->setPosition(sf::Vector2f(
            windowSize.x / 2.0f,
            windowSize.y * 0.7f
        ));
    }
}