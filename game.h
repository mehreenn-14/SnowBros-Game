#pragma once
#include <SFML/Graphics.hpp>
#include "player.h"
class GameScreen;

class Game {
private:
    sf::RenderWindow window;  // member window
    Player* player1;
    Player* player2;
    GameScreen* screen;
    sf::Clock    clock;
    float        deltaTime;

public:
    Game();
    ~Game();
    void run();
};