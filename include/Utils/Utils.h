#pragma once

#include <SFML/Graphics.hpp>
#include <random>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <chrono>

namespace Utils {
    // Random number generation
    class Random {
    private:
        static std::mt19937 generator;
        
    public:
        // Initialize random number generator
        static void init();
        
        // Generate integer within specified range
        static int getInt(int min, int max);
        
        // Generate float within specified range
        static float getFloat(float min, float max);
        
        // Generate random color
        static sf::Color getColor(bool randomAlpha = false);
        
        // Randomly select an item from vector
        template<typename T>
        static const T& getItem(const std::vector<T>& items);
        
        // Random boolean with specified probability
        static bool getBool(float trueProbability = 0.5f);
    };
    
    // String operations
    class String {
    public:
        // Convert to lowercase
        static std::string toLower(const std::string& str);
        
        // Convert to uppercase
        static std::string toUpper(const std::string& str);
        
        // Split string
        static std::vector<std::string> split(const std::string& str, char delimiter);
        
        // Trim whitespace
        static std::string trim(const std::string& str);
        
        // Format string
        template<typename... Args>
        static std::string format(const std::string& format, Args... args);
    };
    
    // Geometry calculations
    class Math {
    public:
        // Calculate distance between two points
        static float distance(const sf::Vector2f& a, const sf::Vector2f& b);
        
        // Calculate vector length
        static float length(const sf::Vector2f& vector);
        
        // Normalize vector
        static sf::Vector2f normalize(const sf::Vector2f& vector);
        
        // Clamp value within range
        static float clamp(float value, float min, float max);
        
        // Linear interpolation
        static float lerp(float a, float b, float t);
        
        // Convert radians to degrees
        static float radToDeg(float radians);
        
        // Convert degrees to radians
        static float degToRad(float degrees);
        
        // Calculate collision point between two rectangles
        static sf::Vector2f getCollisionPoint(const sf::FloatRect& a, const sf::FloatRect& b);
        
        // Check if point is inside rectangle
        static bool pointInRect(const sf::Vector2f& point, const sf::FloatRect& rect);
    };
    
    // Time related
    class Time {
    private:
        static std::chrono::steady_clock::time_point startTime;
        
    public:
        // Initialize time
        static void init();
        
        // Get elapsed time since initialization (seconds)
        static float getElapsedTime();
        
        // Get current timestamp
        static std::string getTimestamp();
        
        // Create simple timer
        static std::function<bool()> createTimer(float duration);
    };
    
    // File operations
    class File {
    public:
        // Check if file exists
        static bool exists(const std::string& path);
        
        // Read text file content
        static std::string readTextFile(const std::string& path);
        
        // Write text file
        static bool writeTextFile(const std::string& path, const std::string& content);
        
        // Get file extension
        static std::string getExtension(const std::string& path);
        
        // Create directory (if it doesn't exist)
        static bool createDirectory(const std::string& path);
    };
    
    // UI helper functions
    class UI {
    public:
        // Center text
        static void centerText(sf::Text& text, float x, float y);
        
        // Center text horizontally
        static void centerTextHorizontally(sf::Text& text, float x);
        
        // Center text vertically
        static void centerTextVertically(sf::Text& text, float y);
        
        // Set text origin to center
        static void setTextOriginToCenter(sf::Text& text);
        
        // Create text with background
        static std::pair<sf::Text, sf::RectangleShape> createTextWithBackground(
            const sf::Font& font, 
            const std::string& content, 
            unsigned int characterSize, 
            const sf::Color& textColor, 
            const sf::Color& backgroundColor, 
            float padding = 10.0f
        );
    };
}

// 模板函数实现
template<typename T>
const T& Utils::Random::getItem(const std::vector<T>& items) {
    if (items.empty()) {
        throw std::runtime_error("Attempted to randomly select from empty vector");
    }
    
    std::uniform_int_distribution<size_t> distribution(0, items.size() - 1);
    return items[distribution(generator)];
}

template<typename... Args>
std::string Utils::String::format(const std::string& format, Args... args) {
    int size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for null terminator
    if (size <= 0) {
        throw std::runtime_error("Error formatting string");
    }
    
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    
    return std::string(buf.get(), buf.get() + size - 1); // Excluding null terminator
}