#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <map>

class AssetManager {
private:
    // 单例实例
    static AssetManager* s_instance;
    
    // 资源容器
    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::Font> fonts;
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::map<std::string, sf::Sound> sounds;
    
    AssetManager();

public:
    ~AssetManager();
    
    // 获取单例实例
    static AssetManager* getInstance();
    
    // 初始化资源
    void init();
    
    // 加载资源
    void loadTexture(const std::string& name, const std::string& filename);
    bool loadFont(const std::string& name, const std::string& filename);
    void loadSoundBuffer(const std::string& name, const std::string& filename);
    
    // 检查资源是否存在
    bool hasTexture(const std::string& name);
    bool hasFont(const std::string& name);
    
    // 获取资源
    sf::Texture& getTexture(const std::string& name);
    sf::Font& getFont(const std::string& name);
    sf::SoundBuffer& getSoundBuffer(const std::string& name);
    
    // 创建音效
    void createSound(const std::string& soundName, const std::string& bufferName);
    
    // 播放音效
    void playSound(const std::string& name);
};