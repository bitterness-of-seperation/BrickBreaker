#include "States/MenuState.h"
#include "Game.h"
#include "States/PlayState.h"
#include "States/HelpState.h"
#include "Managers/AssetManager.h"
#include <iostream>

MenuState::MenuState(Game* game)
    : GameState(game),
    selectedItemIndex(0),
    itemSpacing(50.0f),
    menuPosition(0.0f, 0.0f){
}

void MenuState::init() { //设置背景、字体、菜单项
    // 获取窗口大小
    sf::Vector2u windowSize = game->getWindow().getSize();
    
    // 初始化背景
    background.setSize(sf::Vector2f(windowSize));
    background.setFillColor(sf::Color(20, 20, 50, 255)); // 深蓝色背景
    
    // 加载字体
    try {
        if (AssetManager::getInstance()->hasFont("arial")) {
            font = AssetManager::getInstance()->getFont("arial");
            
            // Initialize title - SFML 3.0.0 requires font in constructor
            titleText = std::make_unique<sf::Text>(font, "Brick Breaker", 60);
            titleText->setFillColor(sf::Color::White);
            titleText->setStyle(sf::Text::Bold);
            
            // Center title - SFML 3.0.0 uses getLocalBounds()
            sf::FloatRect titleBounds = titleText->getLocalBounds();
            // Set origin to center of text
            titleText->setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
            titleText->setPosition(sf::Vector2f(
                windowSize.x / 2.0f,
                windowSize.y * 0.15f
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
    selector.setFillColor(sf::Color::Yellow);
    selector.setOrigin(sf::Vector2f(10.0f, 10.0f));
    
    // 初始化菜单项
    initMenuItems();
    
    // 更新选择器位置
    updateSelectorPosition();
}

void MenuState::initMenuItems() { //设置菜单项
    // 清空现有菜单项
    menuItems.clear();
    
    // Add default menu items
    addMenuItem("Start Game", [this]() {
        // Create a new PlayState to start a fresh game from level 1
        auto playState = std::make_unique<PlayState>(game);
        game->changeState(std::move(playState));
    });
    
    addMenuItem("Help", [this]() {
        game->changeState(std::make_unique<HelpState>(game));
    });
    
    addMenuItem("Exit", [this]() {
        game->quit();
    });
    
    // 设置默认选择项
    selectedItemIndex = 0;
    if (!menuItems.empty()) {
        menuItems[selectedItemIndex].selected = true;
    }
}

void MenuState::handleInput(const sf::Event& event) { //输入事件
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

void MenuState::update(float deltaTime) { //菜单不用频繁更新
    // 菜单状态通常不需要每帧更新逻辑
}

void MenuState::render(sf::RenderWindow& window) { //渲染背景、标题、选择器和菜单项
    // Draw background
    window.draw(background);
    
    // Draw title
    if (titleText) {
        window.draw(*titleText);
    }
    
    // Draw selector and menu items
    window.draw(selector);
    
    for (const auto& item : menuItems) {
        window.draw(item.displayText);
    }
}

void MenuState::moveUp() {
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

void MenuState::moveDown() {
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

void MenuState::select() {
    if (menuItems.empty()) return;
    
    // 执行选中项的动作
    if (menuItems[selectedItemIndex].action) {
        menuItems[selectedItemIndex].action();
    }
}

void MenuState::addMenuItem(const std::string& text, const std::function<void()>& action) {
    MenuItem item(text, action, font);
    
    // Set text properties
    item.displayText.setCharacterSize(30);
    item.displayText.setFillColor(menuItems.empty() ? sf::Color::Yellow : sf::Color::White);
    
    // Center text - SFML 3.0.0 uses getLocalBounds()
    sf::FloatRect textBounds = item.displayText.getLocalBounds();
    sf::Vector2f origin(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f);
    item.displayText.setOrigin(origin);
    
    // Set position
    item.displayText.setPosition(sf::Vector2f(
        menuPosition.x,
        menuPosition.y + menuItems.size() * itemSpacing
    ));
    
    // Set selection state
    item.selected = menuItems.empty();
    
    // Add to menu items list
    menuItems.push_back(std::move(item));
    
    // 如果这是第一个菜单项，更新选择器位置
    if (menuItems.size() == 1) {
        updateSelectorPosition();
    }
}

void MenuState::setTitle(const std::string& title) {
    if (titleText) {
        titleText->setString(title);
    
        // 重新居中标题 - SFML 3.0.0中FloatRect使用size
        sf::FloatRect titleBounds = titleText->getLocalBounds();
    sf::Vector2u windowSize = game->getWindow().getSize();
        titleText->setPosition(sf::Vector2f(
            (windowSize.x - titleBounds.size.x) / 2.0f,
        windowSize.y * 0.15f
        ));
        }
}

void MenuState::updateSelectorPosition() {
    if (menuItems.empty()) return;
    
    // 获取当前选中项的位置
    sf::Vector2f itemPos = menuItems[selectedItemIndex].displayText.getPosition();
    sf::FloatRect bounds = menuItems[selectedItemIndex].displayText.getLocalBounds();
    
    // 将选择器放在菜单项的左侧 - SFML 3.0.0中FloatRect使用size
    sf::Vector2f selectorPos(
        itemPos.x - bounds.size.x / 2.0f - 30.0f,
        itemPos.y
    );
    selector.setPosition(selectorPos);
}
