#pragma once

#include "GameState.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <string>
#include <memory>

class Game;

struct PauseMenuItem {
    std::string text;
    sf::Text displayText;
    std::function<void()> action;
    bool selected;
    
    PauseMenuItem(const std::string& text, const std::function<void()>& action, const sf::Font& font)
        : text(text), displayText(font, text, 30), action(action), selected(false) {}
};

class PauseState : public GameState {
private:
    sf::Font font;
    std::unique_ptr<sf::Text> titleText;  // 使用指针避免默认构造函数
    std::vector<PauseMenuItem> menuItems;
    int selectedItemIndex;
    
    // 半透明背景
    sf::RectangleShape background;
    
    // 选择指示器
    sf::CircleShape selector;
    
    // 菜单项间距
    float itemSpacing;
    
    // 菜单项位置
    sf::Vector2f menuPosition;
    
    // 初始化菜单项
    void initMenuItems();
    
    // 更新选择器位置
    void updateSelectorPosition();

public:
    PauseState(Game* game);
    virtual ~PauseState() = default;
    
    // 重写GameState的虚函数
    void init() override;
    void handleInput(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    // 菜单操作
    void moveUp();
    void moveDown();
    void select();
    
    // 添加菜单项
    void addMenuItem(const std::string& text, const std::function<void()>& action);
};