#include "Managers/AssetManager.h"
#include <iostream>
#include <stdexcept>

// Static member initialization
AssetManager* AssetManager::s_instance = nullptr;

AssetManager::AssetManager() {
}

AssetManager::~AssetManager() {
    // Clean up resources
    textures.clear();
    fonts.clear();
    soundBuffers.clear();
    sounds.clear();
}

AssetManager* AssetManager::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new AssetManager();
    }
    return s_instance;
}

void AssetManager::init() {
    // Initialize resource manager
    std::cout << "AssetManager initialized" << std::endl;
}

void AssetManager::loadTexture(const std::string& name, const std::string& filename) {
    sf::Texture texture;
    if (texture.loadFromFile(filename)) {
        textures[name] = texture;
        std::cout << "Loaded texture: " << name << " from " << filename << std::endl;
    } else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }
}

bool AssetManager::loadFont(const std::string& name, const std::string& filename) {
    try {
        sf::Font font;
        if (font.openFromFile(filename)) {
            fonts[name] = font;
            std::cout << "Font loaded successfully: " << name << std::endl;
            return true;
        } else {
            std::cerr << "Failed to load font: " << filename << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Font loading exception: " << e.what() << std::endl;
        return false;
    }
}

void AssetManager::loadSoundBuffer(const std::string& name, const std::string& filename) {
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(filename)) {
        soundBuffers[name] = buffer;
        std::cout << "Loaded sound buffer: " << name << " from " << filename << std::endl;
    } else {
        std::cerr << "Failed to load sound buffer: " << filename << std::endl;
    }
}

bool AssetManager::hasTexture(const std::string& name) {
    return textures.find(name) != textures.end();
}

bool AssetManager::hasFont(const std::string& name) {
    return fonts.find(name) != fonts.end();
}

sf::Texture& AssetManager::getTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    throw std::runtime_error("Texture not found: " + name);
}

sf::Font& AssetManager::getFont(const std::string& name) {
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        return it->second;
    }
    throw std::runtime_error("Font not found: " + name);
}

sf::SoundBuffer& AssetManager::getSoundBuffer(const std::string& name) {
    auto it = soundBuffers.find(name);
    if (it != soundBuffers.end()) {
        return it->second;
    }
    throw std::runtime_error("Sound buffer not found: " + name);
}

void AssetManager::createSound(const std::string& soundName, const std::string& bufferName) {
    auto it = soundBuffers.find(bufferName);
    if (it != soundBuffers.end()) {
        sounds.emplace(soundName, sf::Sound(it->second));
        std::cout << "Created sound: " << soundName << " from buffer: " << bufferName << std::endl;
    } else {
        std::cerr << "Failed to create sound: " << soundName << " - buffer not found: " << bufferName << std::endl;
    }
}

void AssetManager::playSound(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        it->second.play();
    } else {
        std::cerr << "Sound not found: " << name << std::endl;
    }
}
