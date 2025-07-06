cmake_minimum_required(VERSION 3.16)
project(BreakoutGame VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找SFML 3.0 - 使用大写的组件名称
find_package(SFML 3.0 REQUIRED COMPONENTS Graphics Window System Audio)

# 设置源文件
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Entities/Ball.cpp
    src/Entities/Brick.cpp
    src/Entities/Entity.cpp
    src/Entities/Paddle.cpp
    src/Managers/AssetManager.cpp
    src/Managers/CollisionManager.cpp
    src/Managers/LevelManager.cpp
    src/States/GameOverState.cpp
    src/States/MenuState.cpp
    src/States/PauseState.cpp
    src/States/PlayState.cpp
    src/Utils/Config.cpp
    src/Utils/Utils.cpp
)

# 设置头文件目录
include_directories(include)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接SFML库 - 使用SFML 3.0的命名空间目标
target_link_libraries(${PROJECT_NAME} 
    SFML::Graphics 
    SFML::Window 
    SFML::System 
    SFML::Audio
)

# 复制资源文件到构建目录
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})

# 设置调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
