

#pragma once
#include "player.h"
#include "attack.h"
#include "screen.h"
#include "enemy.h"
#include "levelManager.h"
#include "powerups.h"
#include <SFML/Graphics.hpp>
using namespace sf;

const int MAX_ENEMIES = 20;
const int MAX_POWERUPS = 10;

class GameScreen : public Screen {
private:
    RenderWindow& window;

    Texture backgroundTexture;
    Sprite  backgroundSprite;

    float screenWidth;
    float screenHeight;

    Player* player1;
    Player* player2;

    Attack a1;
    Attack a2;

    Enemy* enemies[MAX_ENEMIES];
    int    enemyCount;

    PowerUp* powerUps[MAX_POWERUPS];
    int      powerUpCount;

    Levels* levelManager;

    bool isPaused;
    bool isGameOver;
    bool showHitBox;
    int  currentlvl;

    int nextScreen;

    Clock clock;
    float deltaTime;

    // Star Bonus
    bool isStarMenuOpen;
    int  starOptions[3];
    int  starLevel;
    bool starSpawned;
    bool isBonusPhase;
    float bonusTimer;

    bool freshStart;

public:
    GameScreen(sf::RenderWindow& window,
        Player* p1,
        Player* p2 = nullptr);
    ~GameScreen();

    void onEnter() override {
        isActive = true;
        isGameOver = false;
        nextScreen = 7;
        isPaused = false;
        isGameOver = false;   
        if (freshStart) {
           
            resetGame();
        }
        else {
            
            freshStart = true; 
            isStarMenuOpen = false;
            isBonusPhase = false;
            bonusTimer = 0.0f;
            currentlvl = gameLevel;
            loadLevel(currentlvl);
        }
    }
    void draw() override { render(); }
    void handleInput() override { handleInputs(); }
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }

    void init();
    void handleInputs();
    void render();
    void pause();
    void resume();
    void spawnEnemies();
    void checkCollision();
    void checkDeadEnemies();
    void checkLevelCleared();
    void nextLevel();
    void resetGame();
    void loadLevel(int levelNum);
    void renderHitBoxes();

    void startBonusPhase(); // Helper for transitions
};