

#include "powerups.h"
#include <cmath>

PowerUp::PowerUp(int t, float startX, float startY) {
    type = t;
    x = startX;
    y = startY;
    isAlive = true;
    floatTimer = 0;
    texLoaded = false;
    fallSpeed = 0.f;

    // Default lifetime 12 seconds; cash bundles 3 seconds
    lifetime = (type == PU_CASH) ? 3.0f : 12.0f;

    texLoaded = tex.loadFromFile("assets/Images/Items.png");

    if (texLoaded) {
        sprite.setTexture(tex);

        switch (type) {
        case PU_SPEED:    sprite.setTextureRect(sf::IntRect(340, 10, 155, 145));   break;
        case PU_SNOW:     sprite.setTextureRect(sf::IntRect(340, 170, 155, 145));  break;
        case PU_DISTANCE: sprite.setTextureRect(sf::IntRect(340, 335, 155, 145));  break;
        case PU_BALLOON:  sprite.setTextureRect(sf::IntRect(340, 500, 155, 145));  break;
        case PU_STAR:     sprite.setTextureRect(sf::IntRect(340, 665, 155, 155));  break;
        case PU_EXTRA:    sprite.setTextureRect(sf::IntRect(510, 665, 155, 155));  break;
        case PU_GEM:      sprite.setTextureRect(sf::IntRect(10, 830, 145, 155));   break;
        case PU_CASH:     sprite.setTextureRect(sf::IntRect(50, 1000, 250, 200));  break;
        default:          sprite.setTextureRect(sf::IntRect(340, 665, 155, 155));  break;
        }

        sf::FloatRect b = sprite.getLocalBounds();
        sprite.setOrigin(b.width / 2.f, b.height / 2.f);

        // Scale down to a reasonable in-game size (approx 40x40)
        float targetSize = (type == PU_CASH) ? 60.f : 40.f;
        sprite.setScale(targetSize / b.width, targetSize / b.height);
        sprite.setPosition(x, y);

        hitboxShape.setSize(sf::Vector2f(targetSize, targetSize));
        hitboxShape.setOrigin(targetSize / 2.f, targetSize / 2.f);
        hitboxShape.setPosition(x, y);
    }
    else {
        hitboxShape.setSize(sf::Vector2f(24.f, 24.f));
        hitboxShape.setOrigin(12.f, 12.f);
        hitboxShape.setPosition(x, y);
    }
}

void PowerUp::update(float deltaTime) {
    if (!isAlive) return;

    lifetime -= deltaTime;
    if (lifetime <= 0) {
        isAlive = false;
        return;
    }

    // Coin rain gems (PU_GEM spawned with negative y) and cash bundles fall with gravity
    if (type == PU_GEM || type == PU_CASH) {
        fallSpeed += 500.f * deltaTime;   // gravity acceleration
        y += fallSpeed * deltaTime;

        // Remove if it falls off the bottom of the screen
        if (y > 800.f) { isAlive = false; return; }

        sprite.setPosition(x, y);
        hitboxShape.setPosition(x, y);
    }
    else {
        // Regular powerups: gentle floating bob in place
        floatTimer += deltaTime * 3.0f;
        float offsetY = std::sin(floatTimer) * 8.0f;
        sprite.setPosition(x, y + offsetY);
        hitboxShape.setPosition(x, y + offsetY);
    }

    // Flickering in last 1.5 s
    if (lifetime < 1.5f) {
        if (static_cast<int>(lifetime * 10) % 2 == 0) sprite.setColor(sf::Color(255, 255, 255, 150));
        else sprite.setColor(sf::Color::White);
    }
}

void PowerUp::draw(sf::RenderWindow& window) {
    if (!isAlive) return;

    if (texLoaded) {
        window.draw(sprite);
    }
    else {
        sf::CircleShape circle(12.f);
        circle.setOrigin(12.f, 12.f);
        circle.setPosition(x, y);

        switch (type) {
        case PU_SPEED:    circle.setFillColor(sf::Color::Red);              break;
        case PU_SNOW:     circle.setFillColor(sf::Color::Cyan);             break;
        case PU_DISTANCE: circle.setFillColor(sf::Color::Yellow);           break;
        case PU_BALLOON:  circle.setFillColor(sf::Color::Green);            break;
        case PU_STAR:     circle.setFillColor(sf::Color(255, 215, 0));      break;
        case PU_EXTRA:    circle.setFillColor(sf::Color(100, 255, 100));    break;
        case PU_GEM:      circle.setFillColor(sf::Color(0, 200, 255));      break;
        case PU_CASH:     circle.setFillColor(sf::Color::Magenta);          break;
        default:          circle.setFillColor(sf::Color::White);            break;
        }
        window.draw(circle);
    }
}