#include "Managers/LevelManager.h"
#include "Managers/AssetManager.h"
#include <iostream>
#include <fstream>

LevelManager::LevelManager() 
    : currentLevel(1), totalLevels(3), rows(8), columns(10) {
    brickSize = sf::Vector2f(80.0f, 30.0f);
    brickPadding = sf::Vector2f(2.0f, 2.0f);
    levelPosition = sf::Vector2f(50.0f, 100.0f);
    levelSize = sf::Vector2f(800.0f, 240.0f);
}

void LevelManager::init(const std::vector<std::string>& levelFiles, 
                       const sf::Vector2f& levelPos, 
                       const sf::Vector2f& levelSize) {
    this->levelFiles = levelFiles;
    this->levelPosition = levelPos;
    this->levelSize = levelSize;
    this->totalLevels = static_cast<int>(levelFiles.size());
}

std::vector<std::unique_ptr<Brick>> LevelManager::loadLevel(int levelNumber) {
    std::vector<std::unique_ptr<Brick>> bricks;
    
    if (levelNumber < 0 || levelNumber >= totalLevels) {
        std::cerr << "Invalid level number: " << levelNumber << std::endl;
        return bricks;
    }
    
    std::string filename = "resources/levels/level" + std::to_string(levelNumber + 1) + ".txt";
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Cannot open level file: " << filename << std::endl;
        return bricks;
    }
    
    std::string line;
    int row = 0;
    
    while (std::getline(file, line) && row < rows) {
        for (size_t col = 0; col < line.length() && col < static_cast<size_t>(columns); ++col) {
            char brickType = line[col];
            if (brickType != ' ') {
                // Create brick
                auto brick = std::make_unique<Brick>();
                brick->setPosition(sf::Vector2f(
                    levelPosition.x + col * (brickSize.x + brickPadding.x),
                    levelPosition.y + row * (brickSize.y + brickPadding.y)
                ));
                brick->setSize(sf::Vector2f(brickSize.x, brickSize.y));
                
                // Set brick properties based on character
                switch (brickType) {
                    case '1':
                        brick->setHitPoints(1);
                        brick->setColor(sf::Color::Red);
                        break;
                    case '2':
                        brick->setHitPoints(2);
                        brick->setColor(sf::Color::Yellow);
                        break;
                    case '3':
                        brick->setHitPoints(3);
                        brick->setColor(sf::Color::Green);
                        break;
                    default:
                        brick->setHitPoints(1);
                        brick->setColor(sf::Color::Blue);
                        break;
                }
                
                // Set brick texture
                if (AssetManager::getInstance()->hasTexture("brick")) {
                    brick->setTexture(AssetManager::getInstance()->getTexture("brick"));
                }
                
                bricks.push_back(std::move(brick));
            }
        }
        row++;
    }
    
    file.close();
    currentLevel = levelNumber;
    std::cout << "Level " << levelNumber + 1 << " loaded successfully, brick count: " << bricks.size() << std::endl;
    return bricks;
}

bool LevelManager::loadLevel(int levelNumber, std::vector<std::unique_ptr<Brick>>& bricks) {
    if (levelNumber < 1 || levelNumber > totalLevels) {
        std::cerr << "Invalid level number: " << levelNumber << std::endl;
        return false;
    }
    
    std::string filename = "resources/levels/level" + std::to_string(levelNumber) + ".txt";
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Cannot open level file: " << filename << std::endl;
        return false;
    }
    
    // Clear existing bricks
    bricks.clear();
    
    std::string line;
    int row = 0;
    float brickWidth = 80.0f;
    float brickHeight = 30.0f;
    float startX = 50.0f;
    float startY = 100.0f;
    
    while (std::getline(file, line) && row < 8) {
        for (size_t col = 0; col < line.length() && col < 10; ++col) {
            char brickType = line[col];
            if (brickType != ' ') {
                // Create brick
                auto brick = std::make_unique<Brick>();
                brick->setPosition(sf::Vector2f(startX + col * brickWidth, startY + row * brickHeight));
                brick->setSize(sf::Vector2f(brickWidth - 2, brickHeight - 2));
                
                // Set brick properties based on character
                switch (brickType) {
                    case '1':
                        brick->setHitPoints(1);
                        brick->setColor(sf::Color::Red);
                        break;
                    case '2':
                        brick->setHitPoints(2);
                        brick->setColor(sf::Color::Yellow);
                        break;
                    case '3':
                        brick->setHitPoints(3);
                        brick->setColor(sf::Color::Green);
                        break;
                    default:
                        brick->setHitPoints(1);
                        brick->setColor(sf::Color::Blue);
                        break;
                }
                
                // Set brick texture
                if (AssetManager::getInstance()->hasTexture("brick")) {
                    brick->setTexture(AssetManager::getInstance()->getTexture("brick"));
                }
                
                bricks.push_back(std::move(brick));
            }
        }
        row++;
    }
    
    file.close();
    std::cout << "Level " << levelNumber << " loaded successfully, brick count: " << bricks.size() << std::endl;
    return true;
}

std::vector<std::unique_ptr<Brick>> LevelManager::loadNextLevel() {
    if (hasNextLevel()) {
        return loadLevel(currentLevel + 1);
    }
    
    std::cerr << "Warning: No more levels" << std::endl;
    return {};
}

std::vector<std::unique_ptr<Brick>> LevelManager::reloadCurrentLevel() {
    return loadLevel(currentLevel);
}

int LevelManager::getCurrentLevel() const {
    return currentLevel;
}

int LevelManager::getTotalLevels() const {
    return totalLevels;
}

bool LevelManager::hasNextLevel() const {
    return currentLevel < totalLevels;
}

void LevelManager::setBrickSize(const sf::Vector2f& size) {
    brickSize = size;
}

void LevelManager::setBrickPadding(const sf::Vector2f& padding) {
    brickPadding = padding;
}

std::vector<std::unique_ptr<Brick>> LevelManager::loadLevelFromFile(const std::string& filename) {
    std::vector<std::unique_ptr<Brick>> bricks;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Cannot open level file: " << filename << std::endl;
        return bricks;
    }
    
    // 读取关卡尺寸
    file >> rows >> columns;
    
    if (rows <= 0 || columns <= 0) {
        std::cerr << "Invalid level size: " << rows << "x" << columns << std::endl;
        return bricks;
    }
    
    // Calculate actual brick size, considering level area size and brick spacing
    float availableWidth = levelSize.x - (columns - 1) * brickPadding.x;
    float availableHeight = levelSize.y - (rows - 1) * brickPadding.y;
    
    sf::Vector2f actualBrickSize(
        availableWidth / columns,
        availableHeight / rows
    );
    
    // Read level data
    int brickType;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            if (file >> brickType) {
                    // Only create brick when brick type is greater than 0
                    sf::Vector2f position(
                        levelPosition.x + j * (actualBrickSize.x + brickPadding.x),
                        levelPosition.y + i * (actualBrickSize.y + brickPadding.y)
                    );
                    
                    // Create brick
                    auto brick = std::make_unique<Brick>();
                    brick->setPosition(position);
                    brick->setSize(actualBrickSize);
                    
                    // Set brick properties based on brick type
                    switch (brickType) {
                        case 1:
                            brick->setColor(sf::Color::Red);
                            brick->setScore(100);
                            break;
                        case 2:
                            brick->setColor(sf::Color::Yellow);
                            brick->setScore(200);
                            break;
                        case 3:
                            brick->setColor(sf::Color::Green);
                            brick->setScore(300);
                            break;
                        case 4:
                            brick->setColor(sf::Color::Blue);
                            brick->setScore(400);
                            break;
                        case 5:
                            brick->setColor(sf::Color(128, 0, 128)); // purple
                            brick->setScore(500);
                            break;
                        default:
                            brick->setColor(sf::Color::White);
                            brick->setScore(50);
                            break;
                    }
                    
                    // Set brick texture
                    AssetManager::getInstance()->loadTexture("brick", "resources/textures/brick.png");
                    if (AssetManager::getInstance()->hasTexture("brick")) {
                        brick->setTexture(AssetManager::getInstance()->getTexture("brick"));
                    }
                    
                    bricks.push_back(std::move(brick));
                }
            }
        }
    
    file.close();
    return bricks;
}