#include "Game.h"
#include "Utils/Config.h"
#include <iostream>
#include <exception>

int main() {
    // 创建游戏实例
    Game game;
    
    // 初始化游戏
    game.init();
    
    // 运行游戏
    game.run();
    
    return 0;
} 