#include "Game.h"
#include "Utils/Config.h"
#include <iostream>
#include <exception>

int main() {
    // 初始化配置
    Config& config = Config::getInstance();
    
    // 创建游戏实例
    Game game;
    
    // 初始化游戏
    game.init();
    
    // 运行游戏
    game.run();
    
    // 保存配置
    config.save();
    
    return 0;
}