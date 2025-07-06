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
      lives(3),
      ballLaunched(false),
      gameOver(false),
      levelCompleted(false) {
}

void PlayState::init() {
    // Get window size
    sf::Vector2u windowSize = game->getWindow().getSize();
    
    // Set collision manager
    collisionManager.setWindowSize(windowSize);
    
    // Initialize level manager
    std::vector<std::string> levelFiles = {
        "resources/levels/level1.txt",
        "resources/levels/level2.txt",
        "resources/levels/level3.txt"
    };
    
    levelManager.init(levelFiles, 
                      sf::Vector2f(50.0f, 50.0f), 
                      sf::Vector2f(windowSize.x - 100.0f, 200.0f));
    
    // 加载字体
    try {
        if (AssetManager::getInstance()->hasFont("arial")) {
            font = AssetManager::getInstance()->getFont("arial");
            
            // Initialize UI text - SFML 3.0.0 requires font in constructor
            scoreText = std::make_unique<sf::Text>(font, "Score: 0", 24);
            scoreText->setFillColor(sf::Color::White);
            scoreText->setPosition(sf::Vector2f(10.0f, 10.0f));
            
            livesText = std::make_unique<sf::Text>(font, "Lives: 3", 24);
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

void PlayState::initGame() {
    // Get window size
    sf::Vector2u windowSize = game->getWindow().getSize();
    
    // Create paddle
    paddle = std::make_unique<Paddle>(
        sf::Vector2f((windowSize.x - 100.0f) / 2.0f, windowSize.y - 50.0f),
        sf::Vector2f(100.0f, 20.0f)
    );
    
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
    lives = 3;
    
    // Update UI
    updateUI();
}

void PlayState::handleInput(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* keyEvent = event.getIf<sf::Event::KeyPressed>();
        if (keyEvent) {
            switch (keyEvent->code) {
                case sf::Keyboard::Key::Space:
                    if (!ballLaunched && !gameOver) {
                    launchBall();
                }
                break;
                
                case sf::Keyboard::Key::Escape:
                game->pushState(std::make_unique<PauseState>(game));
                break;
                
            default:
                break;
        }
        }
    }
    
    // Handle continuous key input
    if (!gameOver) {
        float paddleSpeed = 400.0f; // pixels/second
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            movePaddle(-paddleSpeed * game->getDeltaTime());
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            movePaddle(paddleSpeed * game->getDeltaTime());
        }
    }
}

void PlayState::update(float deltaTime) {
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

void PlayState::render(sf::RenderWindow& window) {
    // Draw background
    window.clear(sf::Color(20, 20, 50));//深蓝色背景，增加与砖块的对比度
    
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
        ball->setVelocity(sf::Vector2f(200.0f, -300.0f));
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
            if (messageText) messageText->setString("Congratulations! Press ESC for Menu");
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

void PlayState::onBrickHit(Brick* brick) {
    if (brick) {
        addScore(brick->getScore());
        AssetManager::getInstance()->playSound("break");
    }
}

void PlayState::onBallPaddleCollision() {
    AssetManager::getInstance()->playSound("hit");
    }

void PlayState::updateUI() {
    if (scoreText) scoreText->setString("Score: " + std::to_string(score));
    if (livesText) livesText->setString("Lives: " + std::to_string(lives));
}