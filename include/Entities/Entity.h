#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

class Entity {
protected:
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Texture texture;
    std::unique_ptr<sf::Sprite> sprite; // 改为智能指针，以便后期初始化
    bool active;
    float speed;

public:
    Entity();
    Entity(const sf::Vector2f& pos, const sf::Vector2f& size);
    virtual ~Entity() = default;

    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    bool isActive() const;

    void setPosition(const sf::Vector2f& pos);
    void setSize(const sf::Vector2f& size);
    void setActive(bool active);
    void setTexture(const sf::Texture& texture);

    virtual void onCollision(Entity* other) {}
};