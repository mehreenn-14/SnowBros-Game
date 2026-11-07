#pragma once
#include <SFML/Graphics.hpp>

class Entity {
protected:
    float x, y;
    float velocityx, velocityy;
    bool  isAlive;


    sf::RectangleShape hitboxShape;

public:
    Entity() : x(0), y(0), velocityx(0), velocityy(0), isAlive(true) {}
    virtual ~Entity() {}

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    virtual void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    void setAlive(bool alive) {
        isAlive = alive;
    }

    bool getIsAlive() const {
        return isAlive;
    }

    sf::FloatRect getHitBoxBounds() const {
        return hitboxShape.getGlobalBounds();
    }

    float getX() const { return x; }
    float getY() const { return y; }
};
