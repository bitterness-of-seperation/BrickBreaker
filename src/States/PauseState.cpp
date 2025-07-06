#include "States/PauseState.h"
#include "Game.h"
#include "States/PlayState.h"
#include "Managers/AssetManager.h"
#include <iostream>

PauseState::PauseState(Game* game)
    : GameState(game),
      selectedItemIndex(0),
      itemSpacing(50.0f),
      menuPosition(0.0f, 0.0f) {
}

void PauseState::init() {
    // 获取窗口大小
    sf::Vector2u windowSize = game->getWindow().getSize();
    
    // 初始化半透明背景
    background.setSize(sf::Vector2f(windowSize));
    background.setFillColor(sf::Color(255, 255, 255, 150)); // 半透明白色
    
    // 加载字体
    try {
        if (AssetManager::getInstance()->hasFont("arial")) {
            font = AssetManager::getInstance()->getFont("arial");
            
            // Initialize title - SFML 3.0.0 requires font in constructor
            titleText = std::make_unique<sf::Text>(font, "Game Paused", 50);
            titleText->setFillColor(sf::Color::White);
            titleText->setStyle(sf::Text::Bold);
            
            // 居中标题 - SFML 3.0.0使用getLocalBounds()
            sf::FloatRect titleBounds = titleText->getLocalBounds();
            titleText->setPosition(sf::Vector2f(
                (windowSize.x - titleBounds.size.x) / 2.0f,
                windowSize.y * 0.25f
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
        windowSize.y * 0.45f
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

void PauseState::initMenuItems() {
    // 清空现有菜单项
    menuItems.clear();
    
    // Add default menu items
    addMenuItem("Continue", [this]() {
        game->popState();
    });
    
    addMenuItem("Restart", [this]() {
        // Pop pause state
        game->popState();
        
        // Get PlayState and restart game
        if (auto playState = dynamic_cast<PlayState*>(game->getCurrentState())) {
            playState->startNewGame();
        }
    });
    
    addMenuItem("Main Menu", [this]() {
        // Pop pause state and game state to return to main menu
        game->popState(); // Pop pause state
        game->popState(); // Pop game state
    });
    
    // 设置默认选择项
    selectedItemIndex = 0;
    if (!menuItems.empty()) {
        menuItems[selectedItemIndex].selected = true;
    }
}

void PauseState::handleInput(const sf::Event& event) {
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
                // 继续游戏
                game->popState();
                break;
                
            default:
                break;
            }
        }
    }
}

void PauseState::update(float deltaTime) {
    // 暂停状态通常不需要每帧更新逻辑
}

void PauseState::render(sf::RenderWindow& window) {
    // 绘制半透明背景
    window.draw(background);
    
    // 绘制标题
    if (titleText) {
        window.draw(*titleText);
    }
    
    // 绘制选择器和菜单项
    window.draw(selector);
    
    for (const auto& item : menuItems) {
        window.draw(item.displayText);
    }
}

void PauseState::moveUp() {
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

void PauseState::moveDown() {
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

void PauseState::select() {
    if (menuItems.empty()) return;
    
    // 执行选中项的动作
    if (menuItems[selectedItemIndex].action) {
        menuItems[selectedItemIndex].action();
    }
}

void PauseState::addMenuItem(const std::string& text, const std::function<void()>& action) {
    PauseMenuItem item(text, action, font);
    
    // 设置文本属性
    item.displayText.setCharacterSize(30);
    item.displayText.setFillColor(menuItems.empty() ? sf::Color::Yellow : sf::Color::White);
    
    // 居中文本 - SFML 3.0.0使用getLocalBounds()
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

void PauseState::updateSelectorPosition() {
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