#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class Game;

class GameState {
protected:
    Game* game;
public:
    static int currentLevel;
    GameState(Game* game) : game(game) {}
    virtual ~GameState() = default;
    
    virtual void init() = 0;
    virtual void handleInput(const sf::Event& event) = 0; // 修改为常量引用
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    // 状态切换事件：暂未使用
    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onPause() {}
    virtual void onResume() {}

};