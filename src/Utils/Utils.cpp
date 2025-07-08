#include "Utils/Utils.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <iomanip>

// 初始化静态成员
std::mt19937 Utils::Random::generator;
std::chrono::steady_clock::time_point Utils::Time::startTime;

// Random class implementation
void Utils::Random::init() {
    // Use random device as seed
    std::random_device rd;
    generator.seed(rd());
}

int Utils::Random::getInt(int min, int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

float Utils::Random::getFloat(float min, float max) {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}

sf::Color Utils::Random::getColor(bool randomAlpha) {
    uint8_t r = static_cast<uint8_t>(getInt(0, 255));
    uint8_t g = static_cast<uint8_t>(getInt(0, 255));
    uint8_t b = static_cast<uint8_t>(getInt(0, 255));
    uint8_t a = randomAlpha ? static_cast<uint8_t>(getInt(0, 255)) : 255;
    
    return sf::Color(r, g, b, a);
}

bool Utils::Random::getBool(float trueProbability) {
    return getFloat(0.0f, 1.0f) < trueProbability;
}

// String class implementation
std::string Utils::String::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string Utils::String::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::vector<std::string> Utils::String::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string Utils::String::trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(),
                                 [](unsigned char c) { return std::isspace(c); });
    auto end = std::find_if_not(str.rbegin(), str.rend(),
                               [](unsigned char c) { return std::isspace(c); }).base();
    
    return (start < end) ? std::string(start, end) : std::string();
}

// Math class implementation
float Utils::Math::distance(const sf::Vector2f& a, const sf::Vector2f& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float Utils::Math::length(const sf::Vector2f& vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f Utils::Math::normalize(const sf::Vector2f& vector) {
    float len = length(vector);
    if (len > 0) {
        return sf::Vector2f(vector.x / len, vector.y / len);
    }
    return vector;
}

float Utils::Math::clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

float Utils::Math::lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float Utils::Math::radToDeg(float radians) {
    return radians * 180.0f / 3.14159265f;
}

float Utils::Math::degToRad(float degrees) {
    return degrees * 3.14159265f / 180.0f;
}

sf::Vector2f Utils::Math::getCollisionPoint(const sf::FloatRect& a, const sf::FloatRect& b) {
    // Calculate centers of two rectangles
    sf::Vector2f centerA(a.position.x + a.size.x / 2.0f, a.position.y + a.size.y / 2.0f);
    sf::Vector2f centerB(b.position.x + b.size.x / 2.0f, b.position.y + b.size.y / 2.0f);
    
    // Calculate overlap area
    float left = std::max(a.position.x, b.position.x);
    float top = std::max(a.position.y, b.position.y);
    float right = std::min(a.position.x + a.size.x, b.position.x + b.size.x);
    float bottom = std::min(a.position.y + a.size.y, b.position.y + b.size.y);
    
    // Calculate center of overlap area
    sf::Vector2f collisionCenter((left + right) / 2.0f, (top + bottom) / 2.0f);
    
    return collisionCenter;
}

bool Utils::Math::pointInRect(const sf::Vector2f& point, const sf::FloatRect& rect) {
    return point.x >= rect.position.x && point.x <= rect.position.x + rect.size.x &&
           point.y >= rect.position.y && point.y <= rect.position.y + rect.size.y;
}

// Time class implementation
void Utils::Time::init() {
    startTime = std::chrono::steady_clock::now();
}

float Utils::Time::getElapsedTime() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime).count();
    return static_cast<float>(duration) / 1000000.0f; // Convert to seconds
}

std::string Utils::Time::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::function<bool()> Utils::Time::createTimer(float duration) {
    auto startTime = std::chrono::steady_clock::now();
    
    return [startTime, duration]() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
        return (elapsed / 1000.0f) >= duration;
    };
}

// File class implementation
bool Utils::File::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

// Continue File class implementation

std::string Utils::File::readTextFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Utils::File::writeTextFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return !file.fail();
}

std::string Utils::File::getExtension(const std::string& path) {
    std::string extension = std::filesystem::path(path).extension().string();
    
    // Remove leading dot
    if (!extension.empty() && extension[0] == '.') {
        extension = extension.substr(1);
    }
    
    return Utils::String::toLower(extension);
}

bool Utils::File::createDirectory(const std::string& path) {
    return std::filesystem::create_directories(path);
}

// UI class implementation
void Utils::UI::centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.0f, 
                               bounds.position.y + bounds.size.y / 2.0f));
    text.setPosition(sf::Vector2f(x, y));
}

void Utils::UI::centerTextHorizontally(sf::Text& text, float x) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.0f, text.getOrigin().y));
    text.setPosition(sf::Vector2f(x, text.getPosition().y));
}

void Utils::UI::centerTextVertically(sf::Text& text, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(text.getOrigin().x, bounds.position.y + bounds.size.y / 2.0f));
    text.setPosition(sf::Vector2f(text.getPosition().x, y));
}

void Utils::UI::setTextOriginToCenter(sf::Text& text) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.0f, 
                               bounds.position.y + bounds.size.y / 2.0f));
}

std::pair<sf::Text, sf::RectangleShape> Utils::UI::createTextWithBackground(
    const sf::Font& font, 
    const std::string& content, 
    unsigned int characterSize, 
    const sf::Color& textColor, 
    const sf::Color& backgroundColor, 
    float padding) {
    
    sf::Text text(font, content, characterSize);
    text.setFillColor(textColor);
    
    sf::FloatRect textBounds = text.getLocalBounds();
    
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(
        textBounds.size.x + padding * 2,
        textBounds.size.y + padding * 2
    ));
    background.setFillColor(backgroundColor);
    
    // Set background origin to align with text
    background.setOrigin(sf::Vector2f(
        textBounds.position.x - padding,
        textBounds.position.y - padding
    ));
    
    return {text, background};
}