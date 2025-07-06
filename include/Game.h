#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>
#include <string>
#include "GameState.h"
#include "Utils/Config.h"

class Game {
private:
    // 窗口
    sf::RenderWindow window;
    
    // 游戏状态栈
    std::stack<std::unique_ptr<GameState>> states;
    
    // 游戏循环控制
    bool running;
    bool paused;
    
    // 帧率控制
    sf::Clock clock;
    float deltaTime;
    
    // 资源初始化
    void initResources();
    
    // 处理事件
    void handleEvents();
    
    // 更新游戏逻辑
    void update();
    
    // 渲染
    void render();

public:
    Game();
    ~Game();
    
    // 初始化游戏
    void init();
    
    // 运行游戏
    void run();
    
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
    
    // 获取窗口引用
    sf::RenderWindow& getWindow();
    
    // 获取帧率
    float getFPS() const;
    
    // 获取Delta Time
    float getDeltaTime() const;
};