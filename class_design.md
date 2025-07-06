# 打砖块游戏设计文档

## 1. 类的设计分析

### 1.1 类的结构

本打砖块游戏采用面向对象的设计方法，基于C++17和SFML 3.0.0实现。整个游戏系统主要分为以下几个核心模块：

1. **实体系统（Entities）**：游戏中的可视对象
2. **状态系统（States）**：游戏的不同状态
3. **管理器系统（Managers）**：管理游戏资源和逻辑
4. **工具系统（Utils）**：提供通用功能

#### 1.1.1 实体系统

实体系统包含游戏中所有可交互的对象：

- **Entity**：所有游戏实体的基类
- **Ball**：球，从Entity派生
- **Brick**：砖块，从Entity派生
- **Paddle**：挡板，从Entity派生

#### 1.1.2 状态系统

状态系统管理游戏的不同状态和界面：

- **GameState**：所有游戏状态的基类
- **MenuState**：主菜单状态
- **PlayState**：游戏进行状态
- **PauseState**：暂停状态
- **GameOverState**：游戏结束状态

#### 1.1.3 管理器系统

管理器系统负责管理游戏资源和逻辑：

- **AssetManager**：管理游戏资源（纹理、音效、字体）
- **CollisionManager**：处理碰撞检测
- **LevelManager**：管理游戏关卡

#### 1.1.4 工具系统

工具系统提供通用功能：

- **Config**：游戏配置管理
- **Utils**：通用工具函数

### 1.2 类之间的关系

#### 1.2.1 继承关系

- **Entity** 是所有游戏实体的基类，**Ball**、**Brick** 和 **Paddle** 都继承自它
- **GameState** 是所有游戏状态的基类，**MenuState**、**PlayState**、**PauseState** 和 **GameOverState** 都继承自它

#### 1.2.2 组合关系

- **Game** 类包含一个状态栈，管理当前活动的 **GameState**
- **PlayState** 包含 **Ball**、**Paddle** 和多个 **Brick** 实例
- **PlayState** 使用 **CollisionManager** 处理碰撞
- **PlayState** 使用 **LevelManager** 加载关卡

#### 1.2.3 依赖关系

- 所有类都依赖 **AssetManager** 加载资源
- 大多数类依赖 **Utils** 提供的工具函数
- **Game** 类依赖 **Config** 加载配置

## 2. 游戏的类的设计分析

### 2.1 游戏原型抽象

在设计打砖块游戏时，我们首先分析了游戏的核心元素：

1. **可交互对象**：球、砖块、挡板
2. **游戏流程**：菜单、游戏、暂停、结束
3. **资源管理**：纹理、音效、字体
4. **游戏逻辑**：碰撞检测、关卡管理

基于这些核心元素，我们设计了相应的类结构。

### 2.2 基类与派生类设计

#### 2.2.1 Entity基类

**Entity** 是所有游戏实体的基类，定义了共同的属性和方法：

```cpp
class Entity {
protected:
    sf::Vector2f position;
    sf::Vector2f size;
    std::unique_ptr<sf::Sprite> sprite;
    bool active;
    float speed;

public:
    // 构造函数和析构函数
    Entity();
    Entity(const sf::Vector2f& pos, const sf::Vector2f& size);
    virtual ~Entity() = default;

    // 纯虚函数，必须由派生类实现
    virtual void update(float deltaTime) = 0;
    virtual void onCollision(Entity* other) = 0;
    
    // 通用方法
    virtual void render(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;
    
    // Getter和Setter
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    bool isActive() const;
    void setPosition(const sf::Vector2f& pos);
    void setSize(const sf::Vector2f& size);
    void setActive(bool active);
    void setSpeed(float speed);
    void setTexture(const sf::Texture& texture);
};
```

#### 2.2.2 派生类

从Entity派生的类包括：

**Ball类**：

```cpp
class Ball : public Entity {
private:
    sf::Vector2f velocity;
    bool stuck;
    Entity* attachedEntity;

public:
    Ball();
    Ball(const sf::Vector2f& pos, const sf::Vector2f& size);

    void update(float deltaTime) override;
    void onCollision(Entity* other) override;
    
    void launch(const sf::Vector2f& initialVelocity);
    void reset(const sf::Vector2f& position);
    void attachTo(Entity* entity);
    
    sf::Vector2f getVelocity() const;
    void setVelocity(const sf::Vector2f& vel);
    bool isStuck() const;
};
```

**Brick类**：

```cpp
class Brick : public Entity {
private:
    int hitPoints;
    int score;
    bool breakable;
    sf::Color color;

public:
    Brick();
    Brick(const sf::Vector2f& pos, const sf::Vector2f& size, int hitPoints = 1, int score = 100);

    void update(float deltaTime) override;
    void onCollision(Entity* other) override;
    
    int getHitPoints() const;
    void setHitPoints(int points);
    int getScore() const;
    void setScore(int score);
    bool isBreakable() const;
    void setBreakable(bool breakable);
    void hit();
    void setColor(const sf::Color& color);
    sf::Color getColor() const;
};
```

**Paddle类**：

```cpp
class Paddle : public Entity {
private:
    float maxSpeed;
    float moveDirection;

public:
    Paddle();
    Paddle(const sf::Vector2f& pos, const sf::Vector2f& size);

    void update(float deltaTime) override;
    void onCollision(Entity* other) override;
    
    void move(float direction);
    void stop();
    void setMaxSpeed(float speed);
    float getMaxSpeed() const;
};
```

#### 2.2.3 GameState基类

**GameState** 是所有游戏状态的基类：

```cpp
class GameState {
protected:
    Game* game;

public:
    GameState(Game* game) : game(game) {}
    virtual ~GameState() = default;
    
    virtual void init() = 0;
    virtual void handleInput(sf::Event event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    // 状态切换事件
    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onPause() {}
    virtual void onResume() {}
};
```


### 2.3 管理器类设计

#### 2.3.1 AssetManager

```cpp
public:
    // 资源加载方法
    void loadTexture(const std::string& name, const std::string& filename);
    void loadFont(const std::string& name, const std::string& filename);
    void loadSound(const std::string& name, const std::string& filename);
    
    // 资源获取方法
    sf::Texture& getTexture(const std::string& name);
    sf::Font& getFont(const std::string& name);
    sf::SoundBuffer& getSoundBuffer(const std::string& name);
    
    // 资源检查方法
    bool hasTexture(const std::string& name) const;
    bool hasFont(const std::string& name) const;
    bool hasSound(const std::string& name) const;
    
    // 音效控制
    void playSound(const std::string& name, bool loop = false);
    void stopSound(const std::string& name);
    void stopAllSounds();
};
```

#### 2.3.2 CollisionManager

**CollisionManager** 负责处理游戏中的碰撞检测：

```cpp
class CollisionManager {
private:
    // 游戏窗口尺寸
    sf::Vector2u windowSize;
    
    // 检测球与窗口边界的碰撞
    void checkBallWindowCollision(Ball* ball);
    
    // 检测两个实体之间的碰撞
    bool checkEntityCollision(Entity* a, Entity* b);
    
    // 处理球与砖块的碰撞
    void handleBallBrickCollision(Ball* ball, Brick* brick);
    
    // 处理球与挡板的碰撞
    void handleBallPaddleCollision(Ball* ball, Paddle* paddle);

public:
    CollisionManager();
    CollisionManager(const sf::Vector2u& windowSize);
    
    // 设置窗口尺寸
    void setWindowSize(const sf::Vector2u& size);
    
    // 检测球与实体列表的碰撞
    void checkCollisions(Ball* ball, Paddle* paddle, std::vector<std::unique_ptr<Brick>>& bricks);
    
    // 检测球与窗口底部的碰撞（球出界）
    bool checkBallBottomCollision(Ball* ball);
};
```

#### 2.3.3 LevelManager

**LevelManager** 负责加载和管理游戏关卡：

```cpp
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
    std::vector<std::unique_ptr<Brick>> loadLevel(int levelIndex);
    
    // 从文件加载关卡
    std::vector<std::unique_ptr<Brick>> loadLevelFromFile(const std::string& filename);
    
    // 获取当前关卡索引
    int getCurrentLevel() const;
    
    // 检查是否有下一关
    bool hasNextLevel() const;
    
    // 获取总关卡数
    int getTotalLevels() const;
    
    // 设置关卡布局参数
    void setBrickSize(const sf::Vector2f& size);
    void setBrickPadding(const sf::Vector2f& padding);
};
```

### 2.4 类之间的关系管理

#### 2.4.1 状态管理

游戏使用状态栈来管理不同的游戏状态：

```cpp
class Game {
private:
    sf::RenderWindow window;
    std::stack<std::unique_ptr<GameState>> states;
    float deltaTime;
    sf::Clock clock;
    
    void calculateDeltaTime();
    void processEvents();
    void update();
    void render();
    
public:
    Game();
    ~Game();
    
    void run();
    
    // 状态管理
    void pushState(std::unique_ptr<GameState> state);
    void popState();
    void changeState(std::unique_ptr<GameState> state);
    GameState* getCurrentState();
    
    // 获取窗口
    sf::RenderWindow& getWindow();
};
```

通过这种方式，游戏可以方便地在不同状态间切换，如从主菜单到游戏状态，或从游戏状态到暂停状态。

#### 2.4.2 实体管理

在PlayState中，实体通过智能指针进行管理：

```cpp
class PlayState : public GameState {
private:
    // 游戏实体
    std::unique_ptr<Ball> ball;
    std::unique_ptr<Paddle> paddle;
    std::vector<std::unique_ptr<Brick>> bricks;
    
    // 管理器
    CollisionManager collisionManager;
    LevelManager levelManager;
    
    // 游戏状态
    int score;
    int lives;
    bool ballLaunched;
    bool gameOver;
    bool levelCompleted;
    
    // UI元素
    sf::Font font;
    sf::Text scoreText;
    sf::Text livesText;
    sf::Text messageText;
    
    // 初始化游戏
    void initGame();
    
    // 加载关卡
    void loadLevel(int level);
    
    // 重置球和挡板位置
    void resetBallAndPaddle();
    
    // 更新UI
    void updateUI();
    
    // 移动挡板
    void movePaddle(float direction);
    
    // 发射球
    void launchBall();
    
    // 砖块被击中处理
    void onBrickHit(Brick* brick);
    
    // 球与挡板碰撞处理
    void onBallPaddleCollision();
    
    // 检查关卡是否完成
    bool checkLevelCompleted();

public:
    PlayState(Game* game);
    
    // GameState接口实现
    void init() override;
    void handleInput(sf::Event event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
    // 状态切换事件
    void onEnter() override;
    void onExit() override;
    void onPause() override;
    void onResume() override;
    
    // 获取当前分数
    int getScore() const;
};
```

通过使用智能指针，我们简化了内存管理，避免了内存泄漏的风险。

### 2.5 优化设计方法

#### 2.5.1 单例模式

对于AssetManager和Config等全局资源管理器，我们采用了单例模式：

```cpp
class AssetManager {
private:
    static std::unique_ptr<AssetManager> instance;
    // ...

public:
    static AssetManager& getInstance() {
        if (!instance) {
            instance = std::unique_ptr<AssetManager>(new AssetManager());
        }
        return *instance;
    }
    // ...
};

// 在cpp文件中初始化静态成员
std::unique_ptr<AssetManager> AssetManager::instance = nullptr;
```

这确保了整个游戏中只有一个AssetManager实例，避免了资源的重复加载和管理。



#### 2.5.2 状态模式

```cpp
// 添加状态
void Game::pushState(std::unique_ptr<GameState> state) {
    if (!states.empty()) {
        states.top()->onPause();
    }
    
    if (state) {
        state->init();
        states.push(std::move(state));
    }
}

// 移除状态
void Game::popState() {
    if (!states.empty()) {
        states.pop();
        
        if (!states.empty()) {
            states.top()->onResume();
        }
    }
}
```

通过这种方式，我们可以轻松地在菜单、游戏和暂停等状态之间切换，同时保持每个状态的独立性。