#include "attack.h"
#include "player.h"
#include <iostream>
using namespace std;

Attack::Attack(Player* p, float screenWidth) {
    player = p;
    this->screenWidth = screenWidth;
    chainCount = 0;
    size = 5.0f;
    speed = 380.0f;
    maxDistance = screenWidth * 0.30f;

    isThrowing = false;
    active = false;
    distanceTravelled = 0;

    x = 0;
    y = 0;
    velocityX = 0;

    //BALL VISUAL
    if (tex.loadFromFile("assets/Images/Player_Blue.png")) {
        sprite.setTexture(tex);
        sprite.setTextureRect(sf::IntRect(16, 814, 63, 63)); 
        sprite.setOrigin(31.5f, 31.5f);
        sprite.setScale(0.5f, 0.5f);
    }

   
    //HITBOX VISUAL
    updateHitBox();
    hitboxes.setFillColor(sf::Color::Transparent);
    hitboxes.setOutlineColor(sf::Color::Yellow);
    hitboxes.setOutlineThickness(2.0f);
}

void Attack::startThrow() {
    if (player->getisFacingRight()) x = player->getX() + 45;
    else                           x = player->getX() - 45;

    y = player->getY() - 10.f; 

    velocityX = player->getisFacingRight() ? speed : -speed;

    distanceTravelled = 0;
    isThrowing = true;
    active = true;

    // visual animation
    player->startAttack();
}

void Attack::throwSnowball(float deltaTime) {
    if (!isThrowing || !active) return;

    float move = velocityX * deltaTime;
    x += move;

    distanceTravelled += abs(move);

    // don't stop based on maxDistance
    if (!player->canTravelInfinite() && distanceTravelled >= maxDistance) {
        active = false;
        isThrowing = false;
        return;
    }

    wrapAroundScreen();

    // UPDATE 
    sprite.setPosition(x, y);
    updateHitBox();
}

void Attack::wrapAroundScreen() {
    if (x > screenWidth) x = 0;
    else if (x < 0) x = screenWidth;
}

void Attack::updateHitBox() {
    hbx = x - size;
    hby = y - size;
    hbw = size * 2;
    hbh = size * 2;

    hitboxes.setPosition(hbx, hby);
    hitboxes.setSize(sf::Vector2f(hbw, hbh));
}

void Attack::draw(sf::RenderWindow& window) {
    if (!active) return;
    window.draw(sprite);
}

void Attack::drawHitBox(sf::RenderWindow& window) {
    if (!active) return;
    window.draw(hitboxes);
}

bool Attack::intersects(float ex, float ey, float ew, float eh) {
    if (!active) return false;
    return (hbx < ex + ew && hbx + hbw > ex && hby < ey + eh && hby + hbh > ey);
}
