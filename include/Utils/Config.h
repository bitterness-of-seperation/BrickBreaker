#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <any>
#include <fstream>
#include <sstream>
#include <memory>
#include <typeindex>
#include <iostream>

class Config {
private:
    // 单例实例
    static std::unique_ptr<Config> instance;
    
    // 配置数据存储
    std::unordered_map<std::string, std::any> values;
    
    // 默认配置文件路径
    std::string configFilePath;
    
    // 私有构造函数（单例模式）
    Config();
    
    // 加载配置文件
    bool loadFromFile(const std::string& filename);
    
    // 保存配置到文件
    bool saveToFile(const std::string& filename) const;
    
    // 解析配置行
    void parseLine(const std::string& line);
    
    // 将配置值写入文件的辅助函数
    void writeConfigValue(std::ofstream& file, const std::string& key, const std::any& value) const;
    
    // 将值字符串转换为具体类型
    template<typename T>
    T convertValue(const std::string& valueStr);
    
    // 特化的转换函数
    sf::Vector2f convertVector2f(const std::string& valueStr);
    sf::Color convertColor(const std::string& valueStr);

public:
    // 获取单例实例
    static Config& getInstance();
    
    // 禁止拷贝和赋值
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // 初始化默认配置
    void initDefaults();
    
    // 加载配置
    bool load(const std::string& filename = "");
    
    // 保存配置
    bool save(const std::string& filename = "") const;
    
    // 获取配置值
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const;
    
    // 设置配置值
    template<typename T>
    void setValue(const std::string& key, const T& value);
    
    // 检查配置是否存在
    bool hasValue(const std::string& key) const;
    
    // 删除配置
    void removeValue(const std::string& key);
    
    // 清空所有配置
    void clear();
    
    // 设置配置文件路径
    void setConfigFilePath(const std::string& path);
};

// 模板函数的实现
template<typename T>
T Config::getValue(const std::string& key, const T& defaultValue) const {
    auto it = values.find(key);
    if (it != values.end()) {
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast&) {
            std::cerr << "Config: Type conversion error, key: " << key << std::endl;
            return defaultValue;
        }
    }
    return defaultValue;
}

template<typename T>
void Config::setValue(const std::string& key, const T& value) {
    values[key] = value;
}

// 特化的模板函数声明
template<>
sf::Vector2f Config::convertValue<sf::Vector2f>(const std::string& valueStr);

template<>
sf::Color Config::convertValue<sf::Color>(const std::string& valueStr);