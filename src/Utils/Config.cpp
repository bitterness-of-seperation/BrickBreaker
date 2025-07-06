#include "Utils/Config.h"
#include <algorithm>
#include <cctype>
#include <regex>

// 初始化静态成员
std::unique_ptr<Config> Config::instance = nullptr;

Config::Config() : configFilePath("config.ini") {
    initDefaults();
}

Config& Config::getInstance() {
    if (!instance) {
        instance = std::unique_ptr<Config>(new Config());
    }
    return *instance;
}

void Config::initDefaults() {
    // 窗口设置
    setValue("window.title", std::string("打砖块游戏"));
    setValue("window.width", 800);
    setValue("window.height", 600);
    setValue("window.fullscreen", false);
    setValue("window.vsync", true);
    setValue("window.framerate_limit", 60);
    
    // 游戏设置
    setValue("game.ball_speed", 300.0f);
    setValue("game.paddle_speed", 500.0f);
    setValue("game.initial_lives", 3);
    setValue("game.brick_rows", 5);
    setValue("game.brick_columns", 10);
    
    // 颜色设置
    setValue("colors.background", sf::Color(20, 20, 50));
    setValue("colors.ball", sf::Color::White);
    setValue("colors.paddle", sf::Color::White);
    setValue("colors.brick1", sf::Color::Red);
    setValue("colors.brick2", sf::Color::Yellow);
    setValue("colors.brick3", sf::Color::Green);
    setValue("colors.brick4", sf::Color::Blue);
    setValue("colors.brick5", sf::Color(128, 0, 128)); // 紫色
    
    // 音效设置
    setValue("sound.enabled", true);
    setValue("sound.volume", 100.0f);
    setValue("sound.music_volume", 80.0f);
    
    // 控制设置 - 使用SFML 3.0.0的枚举值
    setValue("controls.move_left", static_cast<int>(sf::Keyboard::Key::Left));
    setValue("controls.move_right", static_cast<int>(sf::Keyboard::Key::Right));
    setValue("controls.launch", static_cast<int>(sf::Keyboard::Key::Space));
    setValue("controls.pause", static_cast<int>(sf::Keyboard::Key::P));
}

bool Config::load(const std::string& filename) {
    std::string path = filename.empty() ? configFilePath : filename;
    return loadFromFile(path);
}

bool Config::save(const std::string& filename) const {
    std::string path = filename.empty() ? configFilePath : filename;
    return saveToFile(path);
}

bool Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开配置文件: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // 跳过空行和注释行
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        parseLine(line);
    }
    
    file.close();
    return true;
}

bool Config::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法创建配置文件: " << filename << std::endl;
        return false;
    }
    
    file << "# 打砖块游戏配置文件\n";
    file << "# 自动生成，请勿手动修改\n\n";
    
    for (const auto& pair : values) {
        file << pair.first << " = ";
        
        // 处理不同类型的值
        try {
            if (pair.second.type() == typeid(std::string)) {
                file << std::any_cast<std::string>(pair.second);
            } else if (pair.second.type() == typeid(int)) {
                file << std::any_cast<int>(pair.second);
            } else if (pair.second.type() == typeid(float)) {
                file << std::any_cast<float>(pair.second);
            } else if (pair.second.type() == typeid(double)) {
                file << std::any_cast<double>(pair.second);
            } else if (pair.second.type() == typeid(bool)) {
                file << (std::any_cast<bool>(pair.second) ? "true" : "false");
            } else if (pair.second.type() == typeid(sf::Vector2f)) {
                const auto& vec = std::any_cast<sf::Vector2f>(pair.second);
                file << vec.x << "," << vec.y;
            } else if (pair.second.type() == typeid(sf::Color)) {
                const auto& color = std::any_cast<sf::Color>(pair.second);
                file << static_cast<int>(color.r) << "," 
                     << static_cast<int>(color.g) << "," 
                     << static_cast<int>(color.b) << "," 
                     << static_cast<int>(color.a);
            } else if (pair.second.type() == typeid(sf::Keyboard::Key)) {
                file << static_cast<int>(std::any_cast<sf::Keyboard::Key>(pair.second));
            } else {
                file << "未知类型";
            }
        } catch (const std::bad_any_cast&) {
            file << "Type error";
        }
        
        file << "\n";
    }
    
    file.close();
    return true;
}

void Config::parseLine(const std::string& line) {
    // 查找等号分隔符
    size_t equalsPos = line.find('=');
    if (equalsPos == std::string::npos) {
        return; // 没有等号，跳过这一行
    }
    
    // 提取键和值
    std::string key = line.substr(0, equalsPos);
    std::string valueStr = line.substr(equalsPos + 1);
    
    // 去除前后空格
    key.erase(0, key.find_first_not_of(" \t"));
    key.erase(key.find_last_not_of(" \t") + 1);
    valueStr.erase(0, valueStr.find_first_not_of(" \t"));
    valueStr.erase(valueStr.find_last_not_of(" \t") + 1);
    
    // 确定值的类型并存储
    if (key.find("window.title") == 0 || key.find("file.") == 0) {
        // 字符串类型
        setValue(key, valueStr);
    } else if (valueStr == "true" || valueStr == "false") {
        // 布尔类型
        setValue(key, valueStr == "true");
    } else if (valueStr.find(',') != std::string::npos) {
        // 可能是Vector2f或Color
        if (std::count(valueStr.begin(), valueStr.end(), ',') == 1) {
            // Vector2f
            setValue(key, convertVector2f(valueStr));
        } else if (std::count(valueStr.begin(), valueStr.end(), ',') == 3) {
            // Color
            setValue(key, convertColor(valueStr));
        }
    } else if (valueStr.find('.') != std::string::npos) {
        // 浮点数
        try {
            setValue(key, std::stof(valueStr));
        } catch (const std::exception&) {
            std::cerr << "配置解析错误：无法将 " << valueStr << " 转换为浮点数" << std::endl;
        }
    } else {
        // 整数
        try {
            setValue(key, std::stoi(valueStr));
        } catch (const std::exception&) {
            std::cerr << "配置解析错误：无法将 " << valueStr << " 转换为整数" << std::endl;
        }
    }
}

sf::Vector2f Config::convertVector2f(const std::string& valueStr) {
    std::istringstream iss(valueStr);
    std::string xStr, yStr;
    
    if (std::getline(iss, xStr, ',') && std::getline(iss, yStr)) {
        try {
            float x = std::stof(xStr);
            float y = std::stof(yStr);
            return sf::Vector2f(x, y);
        } catch (const std::exception&) {
            std::cerr << "配置解析错误：无法将 " << valueStr << " 转换为Vector2f" << std::endl;
        }
    }
    
    return sf::Vector2f(0, 0);
}

sf::Color Config::convertColor(const std::string& valueStr) {
    std::istringstream iss(valueStr);
    std::string rStr, gStr, bStr, aStr;
    
    if (std::getline(iss, rStr, ',') && 
        std::getline(iss, gStr, ',') && 
        std::getline(iss, bStr, ',') && 
        std::getline(iss, aStr)) {
        try {
            int r = std::stoi(rStr);
            int g = std::stoi(gStr);
            int b = std::stoi(bStr);
            int a = std::stoi(aStr);
            return sf::Color(
                static_cast<std::uint8_t>(r),
                static_cast<std::uint8_t>(g),
                static_cast<std::uint8_t>(b),
                static_cast<std::uint8_t>(a)
            );
        } catch (const std::exception&) {
            std::cerr << "配置解析错误：无法将 " << valueStr << " 转换为Color" << std::endl;
        }
    }
    
    return sf::Color::White;
}

template<>
sf::Vector2f Config::convertValue<sf::Vector2f>(const std::string& valueStr) {
    return convertVector2f(valueStr);
}

template<>
sf::Color Config::convertValue<sf::Color>(const std::string& valueStr) {
    return convertColor(valueStr);
}

bool Config::hasValue(const std::string& key) const {
    return values.find(key) != values.end();
}

void Config::removeValue(const std::string& key) {
    values.erase(key);
}

void Config::clear() {
    values.clear();
}

void Config::setConfigFilePath(const std::string& path) {
    configFilePath = path;
}

