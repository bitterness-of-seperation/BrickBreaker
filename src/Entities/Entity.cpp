#include "Entities/Entity.h"

Entity::Entity() : position(0, 0), size(0, 0), active(true), speed(0.0f) {
    // sprite will be initialized in setTexture
}

Entity::Entity(const sf::Vector2f& pos, const sf::Vector2f& size)
    : position(pos), size(size), active(true), speed(0.0f) {
    // sprite will be initialized in setTexture
}

void Entity::render(sf::RenderWindow& window) {
    if (active && sprite) {
        window.draw(*sprite);
    }
}

sf::FloatRect Entity::getBounds() const {
    if (sprite) {
        return sprite->getGlobalBounds();
    }
    return sf::FloatRect(position, size);
}

sf::Vector2f Entity::getPosition() const {
    return position;
}

sf::Vector2f Entity::getSize() const {
    return size;
}

bool Entity::isActive() const {
    return active;
}

void Entity::setPosition(const sf::Vector2f& pos) {
    position = pos;
    if (sprite) {
        sprite->setPosition(position);
    }
}

void Entity::setSize(const sf::Vector2f& newSize) {
    size = newSize;
    
    if (sprite) {
        // Adjust sprite size to match desired dimensions
        sf::FloatRect bounds = sprite->getLocalBounds();
        float width = bounds.size.x;
        float height = bounds.size.y;
        
        if (width > 0 && height > 0) {
            sprite->setScale(
                {
                size.x / width,
                size.y / height
                }
            );
        }
    }
}

void Entity::setActive(bool isActive) {
    active = isActive;
}

void Entity::setTexture(const sf::Texture& texture) {
    // In SFML 3.0.0, a texture must be provided when creating a Sprite
    sprite = std::make_unique<sf::Sprite>(texture);
    sprite->setPosition(position);
    setSize(size); // Update size
}