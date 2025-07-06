#pragma once

#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include "Entities/Brick.h"
#include "Utils/Config.h"

class LevelManager {
private:
    std::vector<std::string> levelFiles;
    int currentLevel;
    int totalLevels;
    
    // 关卡尺寸
    int rows;
    int columns;
    
    // 砖块尺寸和间距
    sf::Vector2f brickSize;
    sf::Vector2f brickPadding;
    
    // 关卡区域的位置和大小
    sf::Vector2f levelPosition;
    sf::Vector2f levelSize;

public:
    LevelManager();
    
    // 初始化关卡管理器
    void init(const std::vector<std::string>& levelFiles, 
              const sf::Vector2f& levelPos, 
              const sf::Vector2f& levelSize);
    
    // 加载指定关卡
    std::vector<std::unique_ptr<Brick>> loadLevel(int levelNumber);
    
    // 加载指定关卡（返回bool表示成功与否）
    bool loadLevel(int levelNumber, std::vector<std::unique_ptr<Brick>>& bricks);
    
    // 加载下一关卡
    std::vector<std::unique_ptr<Brick>> loadNextLevel();
    
    // 重新加载当前关卡
    std::vector<std::unique_ptr<Brick>> reloadCurrentLevel();
    
    // 获取当前关卡号
    int getCurrentLevel() const;
    
    // 获取总关卡数
    int getTotalLevels() const;
    
    // 是否还有下一关
    bool hasNextLevel() const;
    
    // 设置砖块尺寸和间距
    void setBrickSize(const sf::Vector2f& size);
    void setBrickPadding(const sf::Vector2f& padding);
    
    // 从文件加载关卡数据
    std::vector<std::unique_ptr<Brick>> loadLevelFromFile(const std::string& filename);
};