#include "States/HelpState.h"
#include "Game.h"
#include "States/MenuState.h"
#include "Utils/Config.h"
#include "Managers/AssetManager.h"
#include <iostream>

HelpState::HelpState(Game* game)
    : GameState(game),
      selectedItemIndex(0),
      itemSpacing(50.0f),
      menuPosition(0.0f, 0.0f) {
}

void HelpState::init() { //设置背景、字体、菜单项
    // 获取窗口大小
    sf::Vector2u windowSize = game->getWindow().getSize();
    
    // 初始化背景
    background.setSize(sf::Vector2f(windowSize));
    background.setFillColor(sf::Color(20, 20, 50, 255)); // 深蓝色背景
    
    // 加载字体
    if (AssetManager::getInstance()->hasFont("arial")) {
        font = AssetManager::getInstance()->getFont("arial");
        
        // Initialize title
        titleText = std::make_unique<sf::Text>(font, "Help", 60);
        titleText->setFillColor(sf::Color::Yellow);
        titleText->setStyle(sf::Text::Bold);
        
        // 居中标题
        sf::FloatRect titleBounds = titleText->getLocalBounds();
        titleText->setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
        titleText->setPosition({
            windowSize.x / 2.0f,
            windowSize.y * 0.15f
        });
        
        // Initialize info text
        infoText1 = std::make_unique<sf::Text>(font, "Project GitHub address: ", 24);
        infoText1->setFillColor(sf::Color::White);

        // 居中信息文本
        sf::FloatRect infoBounds1 = infoText1->getLocalBounds();
        infoText1->setOrigin({infoBounds1.size.x / 2.0f, infoBounds1.size.y / 2.0f});
        infoText1->setPosition({
            windowSize.x / 2.0f,
            windowSize.y * 0.35f
        });

        infoText2 = std::make_unique<sf::Text>(font, "https://github.com/", 24);
        infoText2->setFillColor(sf::Color::White);
        
        // 居中信息文本
        sf::FloatRect infoBounds2 = infoText2->getLocalBounds();
        infoText2->setOrigin({infoBounds2.size.x / 2.0f, infoBounds2.size.y / 2.0f});
        infoText2->setPosition({
            windowSize.x / 2.0f,
            windowSize.y * 0.45f
        });

        infoText3 = std::make_unique<sf::Text>(font, "    bitterness-of-seperation/BrickBreaker", 24);
        infoText3->setFillColor(sf::Color::White);
        
        // 居中信息文本
        sf::FloatRect infoBounds3 = infoText3->getLocalBounds();
        infoText3->setOrigin({infoBounds3.size.x / 2.0f, infoBounds3.size.y / 2.0f});
        infoText3->setPosition({
            windowSize.x / 2.0f,
            windowSize.y * 0.55f
        });
    } 
    
    // 设置菜单位置
    menuPosition = sf::Vector2f(
        windowSize.x / 2.0f,
        windowSize.y * 0.65f
    );
    
    // 初始化选择器
    selector.setRadius(10.0f);
    selector.setFillColor(sf::Color::Red);
    selector.setOrigin({10.0f, 10.0f});
    
    // 初始化菜单项
    initMenuItems();
    
    // 更新选择器位置
    updateSelectorPosition();
}

void HelpState::initMenuItems() { //设置菜单项
    // 清空现有菜单项
    menuItems.clear();
    
    // 添加简单模式选项
    addMenuItem("Easy", [this]() {
        // 设置简单模式的球速度
        Config::getInstance().setValue("game.ball_speed", 200.0f);
        Config::getInstance().setValue("game.paddle_speed", 200.0f);
        Config::getInstance().setValue("reward.max_balls", 10);
        Config::getInstance().setValue("reward.ball_spawn_chance", 50);
        Config::getInstance().save();
    });
    
    // 添加困难模式选项
    addMenuItem("Hard", [this]() {
        // 设置困难模式的球速度
        Config::getInstance().setValue("game.ball_speed", 400.0f);
        Config::getInstance().setValue("game.paddle_speed", 400.0f);
        Config::getInstance().setValue("reward.max_balls", 5);
        Config::getInstance().setValue("reward.ball_spawn_chance", 30);
        Config::getInstance().save();
    });
    
    // 添加返回菜单选项
    addMenuItem("Back", [this]() {
        game->changeState(std::make_unique<MenuState>(game));
    });
    
    // 设置默认选择项
    selectedItemIndex = 0;
    if (!menuItems.empty()) {
        menuItems[selectedItemIndex].selected = true;
        menuItems[selectedItemIndex].displayText.setFillColor(sf::Color::Yellow);
    }
}

void HelpState::handleInput(const sf::Event& event) { //输入事件
    if (event.is<sf::Event::KeyPressed>()) {
        const auto* keyEvent = event.getIf<sf::Event::KeyPressed>();
        if (keyEvent) {
            switch (keyEvent->code) {
                case sf::Keyboard::Key::Up:
                    moveUp();
                    break;
                
                case sf::Keyboard::Key::Down:
                    moveDown();
                    break;
                
                case sf::Keyboard::Key::Enter:
                case sf::Keyboard::Key::Space:
                    select();
                    break;
                
                default:
                    break;
            }
        }
    }
}

void HelpState::update(float deltaTime) {
    // HelpState通常不需要每帧更新逻辑 
}

void HelpState::render(sf::RenderWindow& window) { //渲染背景、标题、信息、选择器和菜单项
    // 绘制背景
    window.draw(background);
    
    // 绘制标题
    window.draw(*titleText);
    
    // 绘制信息文本
    window.draw(*infoText1);
    window.draw(*infoText2);
    window.draw(*infoText3);
    
    // 绘制选择器和菜单项
    window.draw(selector);
    
    for (const auto& item : menuItems) {
        window.draw(item.displayText);
    }
}

void HelpState::moveUp() {
    if (menuItems.empty()) return;
    
    // 取消当前选择
    menuItems[selectedItemIndex].selected = false;
    menuItems[selectedItemIndex].displayText.setFillColor(sf::Color::White);
    
    // 选择上一项
    selectedItemIndex = (selectedItemIndex - 1 + menuItems.size()) % menuItems.size();
    
    // 设置新选择
    menuItems[selectedItemIndex].selected = true;
    menuItems[selectedItemIndex].displayText.setFillColor(sf::Color::Yellow);
    
    // 更新选择器位置
    updateSelectorPosition();
}

void HelpState::moveDown() {
    if (menuItems.empty()) return;
    
    // 取消当前选择
    menuItems[selectedItemIndex].selected = false;
    menuItems[selectedItemIndex].displayText.setFillColor(sf::Color::White);
    
    // 选择下一项
    selectedItemIndex = (selectedItemIndex + 1) % menuItems.size();
    
    // 设置新选择
    menuItems[selectedItemIndex].selected = true;
    menuItems[selectedItemIndex].displayText.setFillColor(sf::Color::Yellow);
    
    // 更新选择器位置
    updateSelectorPosition();
}

void HelpState::select() {
    if (menuItems.empty()) return;
    
    // 执行选中项的动作
    if (menuItems[selectedItemIndex].action) {
        menuItems[selectedItemIndex].action();
    }
}

void HelpState::addMenuItem(const std::string& text, const std::function<void()>& action) {
    HelpMenuItem item(text, action, font);
    
    // 设置文本属性
    item.displayText.setCharacterSize(30);
    item.displayText.setFillColor(menuItems.empty() ? sf::Color::Yellow : sf::Color::White);
    
    // 居中文本
    sf::FloatRect textBounds = item.displayText.getLocalBounds();
    item.displayText.setOrigin({textBounds.size.x / 2.0f, textBounds.size.y / 2.0f});
    
    // 设置位置
    item.displayText.setPosition({
        menuPosition.x,
        menuPosition.y + menuItems.size() * itemSpacing
    });
    
    // 设置选中状态
    item.selected = menuItems.empty();
    
    // 添加到菜单项列表
    menuItems.push_back(std::move(item));
    
    // 如果这是第一个菜单项，更新选择器位置
    if (menuItems.size() == 1) {
        updateSelectorPosition();
    }
}

void HelpState::updateSelectorPosition() {
    if (menuItems.empty()) return;
    
    // 获取当前选中项的位置
    sf::Vector2f itemPos = menuItems[selectedItemIndex].displayText.getPosition();
    sf::FloatRect bounds = menuItems[selectedItemIndex].displayText.getLocalBounds();
    
    // 将选择器放在菜单项的左侧
    sf::Vector2f selectorPos(
        itemPos.x - bounds.size.x / 2.0f - 30.0f,
        itemPos.y
    );
    selector.setPosition(selectorPos);
} 