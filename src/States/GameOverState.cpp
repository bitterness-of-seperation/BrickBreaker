#include "States/GameOverState.h"
#include "Game.h"
#include "States/MenuState.h"
#include "States/PlayState.h"
#include "Managers/AssetManager.h"
#include <iostream>

GameOverState::GameOverState(Game* game, int score)
    : GameState(game),
      selectedItemIndex(0),
      itemSpacing(50.0f),
      menuPosition(0.0f, 0.0f),
      finalScore(score) {
}

void GameOverState::init() { //设置背景、字体、菜单项
    // 获取窗口大小
    sf::Vector2u windowSize = game->getWindow().getSize();
    
    // 初始化背景
    background.setSize(sf::Vector2f(windowSize));
    background.setFillColor(sf::Color(20, 20, 50, 255)); // 深蓝色背景
    
    // 加载字体
    try {
        if (AssetManager::getInstance()->hasFont("arial")) {
            font = AssetManager::getInstance()->getFont("arial");
            
            // Initialize title
            titleText = std::make_unique<sf::Text>(font, "Game Over", 60);
            titleText->setFillColor(sf::Color::Red);
            titleText->setStyle(sf::Text::Bold);
            
            // 居中标题
            sf::FloatRect titleBounds = titleText->getLocalBounds();
            titleText->setPosition(sf::Vector2f(
                (windowSize.x - titleBounds.size.x) / 2.0f,
                windowSize.y * 0.15f
            ));
            
            // Initialize score text
            scoreText = std::make_unique<sf::Text>(font, "Final Score: " + std::to_string(finalScore), 40);
            scoreText->setFillColor(sf::Color::Yellow);
            
            // 居中分数文本
            sf::FloatRect scoreBounds = scoreText->getLocalBounds();
            scoreText->setPosition(sf::Vector2f(
                (windowSize.x - scoreBounds.size.x) / 2.0f,
                windowSize.y * 0.3f
            ));
        } else {
            std::cerr << "Warning: Could not load font, using default font" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading font: " << e.what() << std::endl;
    }
    
    // 设置菜单位置
    menuPosition = sf::Vector2f(
        windowSize.x / 2.0f,
        windowSize.y * 0.5f
    );
    
    // 初始化选择器
    selector.setRadius(10.0f);
    selector.setFillColor(sf::Color::Red);
    selector.setOrigin(sf::Vector2f(10.0f, 10.0f));
    
    // 初始化菜单项
    initMenuItems();
    
    // 更新选择器位置
    updateSelectorPosition();
}

void GameOverState::initMenuItems() { //设置菜单项
    // 清空现有菜单项
    menuItems.clear();
    
    // Add default menu items
    addMenuItem("Play Again", [this]() {
        // Create a new PlayState to restart the game from level 1
        auto playState = std::make_unique<PlayState>(game);
        game->changeState(std::move(playState));
    });
    
    addMenuItem("Main Menu", [this]() {
        game->changeState(std::make_unique<MenuState>(game));
    });
    
    addMenuItem("Exit Game", [this]() {
        game->quit();
    });
    
    // 设置默认选择项
    selectedItemIndex = 0;
    if (!menuItems.empty()) {
        menuItems[selectedItemIndex].selected = true;
    }
}

void GameOverState::handleInput(const sf::Event& event) { //输入事件
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
                
                case sf::Keyboard::Key::Escape:
                game->changeState(std::make_unique<MenuState>(game));
                break;
                
            default:
                break;
            }
        }
    }
}

void GameOverState::update(float deltaTime) { //游戏结束不需要频繁更新
    // GameOverState通常不需要每帧更新逻辑
}

void GameOverState::render(sf::RenderWindow& window) { //渲染背景、标题、分数、选择器和菜单项
    // 绘制背景
    window.draw(background);
    
    // 绘制标题
    window.draw(*titleText);
    
    // 绘制分数
    window.draw(*scoreText);
    
    // 绘制选择器和菜单项
    window.draw(selector);
    
    for (const auto& item : menuItems) {
        window.draw(item.displayText);
    }
}

void GameOverState::moveUp() {
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

void GameOverState::moveDown() {
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

void GameOverState::select() {
    if (menuItems.empty()) return;
    
    // 执行选中项的动作
    if (menuItems[selectedItemIndex].action) {
        menuItems[selectedItemIndex].action();
    }
}

void GameOverState::addMenuItem(const std::string& text, const std::function<void()>& action) {
    GameOverMenuItem item(text, action, font);
    
    // 设置文本属性
    item.displayText.setCharacterSize(30);
    item.displayText.setFillColor(menuItems.empty() ? sf::Color::Yellow : sf::Color::White);
    
    // 居中文本
    sf::FloatRect textBounds = item.displayText.getLocalBounds();
    sf::Vector2f origin(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f);
    item.displayText.setOrigin(origin);
    
    // 设置位置
    item.displayText.setPosition(sf::Vector2f(
        menuPosition.x,
        menuPosition.y + menuItems.size() * itemSpacing
    ));
    
    // 设置选中状态
    item.selected = menuItems.empty();
    
    // 添加到菜单项列表
    menuItems.push_back(std::move(item));
    
    // 如果这是第一个菜单项，更新选择器位置
    if (menuItems.size() == 1) {
        updateSelectorPosition();
    }
}

void GameOverState::setFinalScore(int score) {
    finalScore = score;
    if (AssetManager::getInstance()->hasFont("arial")) {
        scoreText->setString("Final Score: " + std::to_string(finalScore));
        
        // 重新居中分数文本
        sf::FloatRect scoreBounds = scoreText->getLocalBounds();
        sf::Vector2u windowSize = game->getWindow().getSize();
        scoreText->setPosition(sf::Vector2f(
            (windowSize.x - scoreBounds.size.x) / 2.0f,
            windowSize.y * 0.3f
        ));
    }
}

void GameOverState::updateSelectorPosition() {
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