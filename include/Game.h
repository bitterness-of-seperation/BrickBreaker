#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stack>
#include <memory>
#include "GameState.h"

class Game {
private:
    sf::RenderWindow window;
    sf::Clock clock;
    std::stack<std::unique_ptr<GameState>> states;
    bool running;
    bool paused;
    float deltaTime;
    
    // 启动页相关
    bool showingSplash;
    float splashTimer;
    float splashDuration;
    std::unique_ptr<sf::Texture> splashTexture; //改为指针
    std::unique_ptr<sf::Sprite> splashSprite;
    
    // 初始化资源
    void initResources();
    
    // 初始化启动页
    void initSplashScreen();

public:
    Game();
    ~Game();
    
    // 初始化游戏
    void init();
    
    // 运行游戏
    void run();
    
    // 处理事件
    void handleEvents();
    
    // 更新游戏逻辑
    void update();
    
    // 渲染
    void render();
    
    // 退出游戏
    void quit();
    
    // 暂停游戏
    void pause();
    
    // 恢复游戏
    void resume();
    
    // 状态管理
    void pushState(std::unique_ptr<GameState> state);
    void popState();
    void changeState(std::unique_ptr<GameState> state);
    GameState* getCurrentState();
    
    // 获取窗口
    sf::RenderWindow& getWindow();
    
    // 获取FPS
    float getFPS() const;
    
    // 获取帧间隔时间
    float getDeltaTime() const;
};