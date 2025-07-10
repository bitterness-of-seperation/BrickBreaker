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
      finalScore(score),
      showingStars(true),
      starsTimer(0.0f),
      starsDuration(5.0f) {
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
    
    // 初始化结束游戏界面
    initEndGameScreen();
    
    // 更新选择器位置
    updateSelectorPosition();
}

// 星星缩放比例
float scale = 0.0f;
void GameOverState::initEndGameScreen() {
    // 获取窗口大小
    sf::Vector2u windowSize = game->getWindow().getSize();
    
    // 初始化背景
    if (AssetManager::getInstance()->hasTexture("endGame_back")) {
        endGameBackSprite = std::make_unique<sf::Sprite>(AssetManager::getInstance()->getTexture("endGame_back"));
        
        // 调整背景图片大小以适应窗口
        sf::Vector2u textureSize = endGameBackSprite->getTexture().getSize();
        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        scale = std::min(scaleX, scaleY) * 0.8f; // 稍微缩小一点，不要占满整个窗口
        
        endGameBackSprite->setScale({scale, scale});
        
        // 居中显示
        sf::FloatRect bounds = endGameBackSprite->getLocalBounds();
        endGameBackSprite->setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        endGameBackSprite->setPosition({windowSize.x / 2.0f, windowSize.y / 2.0f});
    } 
    
    // 左侧星星
    if (AssetManager::getInstance()->hasTexture("endGame_star_left")) {
        starLeftSprite = std::make_unique<sf::Sprite>(AssetManager::getInstance()->getTexture("endGame_star_left"));
        
        // 相同比例
        starLeftSprite->setScale({scale, scale});
        
        // 居中
        sf::FloatRect bounds = starLeftSprite->getLocalBounds();
        starLeftSprite->setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        starLeftSprite->setPosition({windowSize.x / 2.0f, windowSize.y / 2.0f});
    } 
    
    // 中间星星
    if (AssetManager::getInstance()->hasTexture("endGame_star_centre")) {
        starMiddleSprite = std::make_unique<sf::Sprite>(AssetManager::getInstance()->getTexture("endGame_star_centre"));
        
        // 相同比例
        starMiddleSprite->setScale({scale, scale});
        
        // 居中
        sf::FloatRect bounds = starMiddleSprite->getLocalBounds();
        starMiddleSprite->setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        starMiddleSprite->setPosition({windowSize.x / 2.0f, windowSize.y / 2.0f});
    } 
    
    // 右侧星星
    if (AssetManager::getInstance()->hasTexture("endGame_star_right")) {
        starRightSprite = std::make_unique<sf::Sprite>(AssetManager::getInstance()->getTexture("endGame_star_right"));
        
        // 相同比例
        starRightSprite->setScale({scale, scale});
        
        // 居中
        sf::FloatRect bounds = starRightSprite->getLocalBounds();
        starRightSprite->setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        starRightSprite->setPosition({windowSize.x / 2.0f, windowSize.y / 2.0f});
    } 
    
    // 设置初始状态
    showingStars = true;
    starsTimer = 0.0f;
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
            // 如果显示星星界面，任意键跳过
            if (showingStars) {
                showingStars = false;
                return;
            }
            
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

void GameOverState::update(float deltaTime) { //更新星星界面计时器
    // 更新星星界面计时器
    if (showingStars) {
        starsTimer += deltaTime;
        if (starsTimer >= starsDuration) {
            showingStars = false;
        }
    }
}

void GameOverState::render(sf::RenderWindow& window) { //渲染背景、标题、分数、选择器和菜单项
    // 如果显示星星界面
    if (showingStars && endGameBackSprite) {
        // 绘制背景
        window.draw(*endGameBackSprite);
        
        // 根据关卡数显示星星
        int stars = 0;
        int level = GameState::currentLevel;
        if (level == 0) {
            stars = 1;
        } else if (level == 1) {
            stars = 2;
        } else {
            stars = 3;
        } 
        
        // 显示对应数量的星星
        if (stars >= 1 && starLeftSprite) {
            window.draw(*starLeftSprite);
        }
        if (stars >= 2 && starMiddleSprite) {
            window.draw(*starMiddleSprite);
        }
        if (stars >= 3 && starRightSprite) {
            window.draw(*starRightSprite);
        }
        
        // 显示分数
        if (scoreText) {
            // 更新分数文本
            scoreText->setString("FINAL SCORE: " + std::to_string(finalScore));
            
            // 设置文本样式
            scoreText->setCharacterSize(40);
            scoreText->setFillColor(sf::Color::Yellow);
            scoreText->setStyle(sf::Text::Bold);
            
            // 居中显示在背景板下方
            sf::FloatRect textBounds = scoreText->getLocalBounds();
            scoreText->setOrigin({textBounds.size.x / 2.0f, textBounds.size.y / 2.0f});
            scoreText->setPosition({window.getSize().x / 2.0f, window.getSize().y * 0.55f});
            
            window.draw(*scoreText);
        }
    }
    // 否则显示常规游戏结束界面
    else {
        // 绘制背景
        window.draw(background);
        
        // 绘制标题
        window.draw(*titleText);
        
        // 绘制分数
        // window.draw(*scoreText);
        
        // 绘制选择器和菜单项
        window.draw(selector);
        
        for (const auto& item : menuItems) {
            window.draw(item.displayText);
        }
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