

#pragma once
#include <SFML/Graphics.hpp>

class Player;
class Attack {
private:
    Player* player;

    float x;
    float y;
    float velocityX;
    float maxDistance;
    float size;
    bool  isThrowing;
    bool  active;
    float speed;
    float distanceTravelled;
    float screenWidth;
    int chainCount;

    sf::Texture tex;
    sf::Sprite sprite;
    sf::CircleShape  ball;


    sf::RectangleShape hitboxes;
    float hbx, hby, hbw, hbh;

public:
    Attack(Player* p, float screenWidth);

    void startThrow();
    bool intersects(float ex, float ey, float ew, float eh);
    void throwSnowball(float deltaTime);
    void wrapAroundScreen();
    void updateHitBox();
    void draw(sf::RenderWindow& window);
    void drawHitBox(sf::RenderWindow& window); //f1
    void grow() { size += 3.0f; if (size > 30.f) size = 30.f; }
    int  getChainCount() const { return chainCount; }

    // getters 
    bool  isActive() { return active; }
    float getHBX() { return hbx; }
    float getHBY() { return hby; }
    float getHBW() { return hbw; }
    float getHBH() { return hbh; }
    void setActive(bool status) {
        active = status;
        if (!status) isThrowing = false;
    }
    void setPlayer(Player* p) { player = p; }
};