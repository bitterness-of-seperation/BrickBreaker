#include "Game.h"
#include "States/MenuState.h"
#include "States/PlayState.h"
#include "States/PauseState.h"
#include "States/GameOverState.h"
#include "Utils/Config.h"
#include "Utils/Utils.h"
#include "Managers/AssetManager.h"
#include <iostream>

Game::Game() : running(false), paused(false), deltaTime(0.0f), showingSplash(true), splashTimer(0.0f) {
}

Game::~Game() {
    // Save configuration before exiting
    Config::getInstance().save();
    
    // Clear state stack
    while (!states.empty()) {
        states.pop();
    }
}

void Game::init() { //创建配置、工具、窗口和初始化资源、push状态
    // Load configuration
    if (!Config::getInstance().load()) {
        std::cout << "Failed to load config.ini, using default settings" << std::endl;
        // Save default configuration
        Config::getInstance().save();
    }
    
    // Initialize random number generator
    Utils::Random::init();
    
    // Initialize time system
    Utils::Time::init();
    
    // Initialize window
    int width = Config::getInstance().getValue("window.width", 800);
    int height = Config::getInstance().getValue("window.height", 600);
    std::string title = Config::getInstance().getValue("window.title", std::string("Brick Breaker"));
    bool fullscreen = Config::getInstance().getValue("window.fullscreen", false);
    
    // SFML 3.0.0 window creation
    if (fullscreen) {
        window.create(sf::VideoMode::getDesktopMode(), title, sf::Style::Default, sf::State::Fullscreen);
    } else {
        window.create(sf::VideoMode(sf::Vector2u(static_cast<unsigned int>(width), static_cast<unsigned int>(height))), title, sf::Style::Default, sf::State::Windowed);
    }
    
    // 设置垂直同步
    bool vsync = Config::getInstance().getValue("window.vsync", true);
    window.setVerticalSyncEnabled(vsync);
    
    // 设置帧率限制
    int frameRateLimit = Config::getInstance().getValue("window.framerate_limit", 60);
    window.setFramerateLimit(frameRateLimit);
    
    // Initialize resources
    initResources();
    
    // 初始化启动页
    initSplashScreen();
    
    // Set running flag
    running = true;
}

void Game::initResources() { //加载纹理、字体和音效
    // Load textures
    AssetManager::getInstance()->loadTexture("ball", "resources/textures/ball.png");
    AssetManager::getInstance()->loadTexture("paddle", "resources/textures/paddle.png");
    AssetManager::getInstance()->loadTexture("brick", "resources/textures/brick.png");
    AssetManager::getInstance()->loadTexture("start", "resources/textures/start.png");
    AssetManager::getInstance()->loadTexture("background", "resources/textures/background.png");
    
    // 加载结束游戏相关纹理
    AssetManager::getInstance()->loadTexture("endGame_back", "resources/textures/endGame_back.png");
    AssetManager::getInstance()->loadTexture("endGame_star_left", "resources/textures/endGame_star_left.png");
    AssetManager::getInstance()->loadTexture("endGame_star_centre", "resources/textures/endGame_star_centre.png");
    AssetManager::getInstance()->loadTexture("endGame_star_right", "resources/textures/endGame_star_right.png");
    
    // Load font
    AssetManager::getInstance()->loadFont("arial", "resources/fonts/arial.ttf");
    
    // Load sound buffers
    AssetManager::getInstance()->loadSoundBuffer("hit", "resources/sounds/hit.wav");
    AssetManager::getInstance()->loadSoundBuffer("break", "resources/sounds/break.wav");
    AssetManager::getInstance()->loadSoundBuffer("ball_windows", "resources/sounds/ball_windows.wav");
    
    // Create sounds
    AssetManager::getInstance()->createSound("hit", "hit");
    AssetManager::getInstance()->createSound("break", "break");
    AssetManager::getInstance()->createSound("ball_windows", "ball_windows");
}

void Game::initSplashScreen() {
    // 初始化启动页
    splashTexture = std::make_unique<sf::Texture>(AssetManager::getInstance()->getTexture("start"));
    splashSprite = std::make_unique<sf::Sprite>(*splashTexture);
    
    // 设置启动页居中显示
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = splashTexture->getSize();
    
    // 计算缩放比例，使图片适应窗口
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    float scale = std::min(scaleX, scaleY);
    
    splashSprite->setScale({scale, scale});
    
    // 居中显示
    sf::FloatRect bounds = splashSprite->getGlobalBounds();
    splashSprite->setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
    splashSprite->setPosition({windowSize.x / 2.0f, windowSize.y / 2.0f});
    
    // 设置启动页显示时间（秒）
    splashDuration = 3.0f;
    splashTimer = 0.0f;
    showingSplash = true;
}

void Game::run() { //主循环:event、update、render
    if (!running) {
        init();
    }
    
    // 主循环
    while (running && window.isOpen()) {
        // Calculate delta time
        deltaTime = clock.restart().asSeconds();
        
        // 更新启动页计时器
        if (showingSplash) {
            splashTimer += deltaTime;
            if (splashTimer >= splashDuration) {
                showingSplash = false;
                // 启动页结束后，推入菜单状态
                pushState(std::make_unique<MenuState>(this));
            }
        }
        
        // Handle events
        handleEvents();
        
        // Update game logic
        update();
        
        // Render
        render();
    }
}

void Game::handleEvents() { //state->handleEvents
    auto eventOpt = window.pollEvent();
    while (eventOpt.has_value()) {
        sf::Event event = eventOpt.value();
        
        // Window close event
        if (event.is<sf::Event::Closed>()) {
            quit();
        }
        
        // 如果显示启动页，任意键跳过
        if (showingSplash && event.is<sf::Event::KeyPressed>()) {
            showingSplash = false;
            pushState(std::make_unique<MenuState>(this));
        }
        // 如果不显示启动页，则正常处理事件
        else if (!showingSplash && !states.empty()) {
            states.top()->handleInput(event);
        }
        
        eventOpt = window.pollEvent();
    }
}

void Game::update() { //state->update
    // 如果显示启动页，不更新游戏状态
    if (showingSplash) {
        return;
    }
    
    if (!paused && !states.empty()) {
        states.top()->update(deltaTime);
    }
}

void Game::render() { //state->render
    window.clear(Config::getInstance().getValue("colors.background", sf::Color(20, 20, 50)));
    
    // 如果显示启动页，渲染启动页
    if (showingSplash) {
        window.draw(*splashSprite);
    }
    // 否则渲染当前状态
    else if (!states.empty()) {
        states.top()->render(window);
    }
    
    window.display();
}

void Game::quit() { //直接关闭窗口
    // Save configuration before exiting
    Config::getInstance().save();
    
    running = false;
    window.close();
}

void Game::pause() { //state->onPause
    paused = true;
    
    if (!states.empty()) {
        states.top()->onPause();
    }
}

void Game::resume() { //state->onResume
    paused = false;
    
    if (!states.empty()) {
        states.top()->onResume();
    }
}

void Game::pushState(std::unique_ptr<GameState> state) {
    // Pause current state
    if (!states.empty()) {
        states.top()->onPause();
    }
    
    // Initialize and push new state
    state->init();
    states.push(std::move(state));
    states.top()->onEnter();
}

void Game::popState() {
    if (!states.empty()) {
        // Exit current state
        states.top()->onExit();
        states.pop();
        
        // Resume previous state
        if (!states.empty()) {
            states.top()->onResume();
        } else {
            // If no more states, exit game
            quit();
        }
    }
}

void Game::changeState(std::unique_ptr<GameState> state) {
    // Exit current state
    if (!states.empty()) {
        states.top()->onExit();
        states.pop();
    }
    
    // Initialize and push new state
    state->init();
    states.push(std::move(state));
    states.top()->onEnter();
}

GameState* Game::getCurrentState() {
    return states.empty() ? nullptr : states.top().get();
}

sf::RenderWindow& Game::getWindow() {
    return window;
}

float Game::getFPS() const {
    return deltaTime > 0 ? 1.0f / deltaTime : 0.0f;
}

float Game::getDeltaTime() const {
    return deltaTime;
}