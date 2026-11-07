#pragma once
#include "Entity.h"

// Power-Up Type Constants
const int PU_SPEED = 0;
const int PU_SNOW = 1;
const int PU_DISTANCE = 2;
const int PU_BALLOON = 3;
const int PU_STAR = 4;
const int PU_EXTRA = 5;
const int PU_GEM = 6;
const int PU_CASH = 7;

class PowerUp : public Entity {
    sf::Sprite  sprite;
    sf::Texture tex;
    bool        texLoaded;
    int         type;
    float       floatTimer;
    float       lifetime;
    float       fallSpeed;   

public:
    PowerUp(int t, float startX, float startY);
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    int  getType() const { return type; }
};