
#pragma once
#include "Entity.h"
#include "levelManager.h"
#include <SFML/Graphics.hpp>

class Player : public Entity {
private:
    float speed;
    float jumpForce;
    float gravity;
    bool  facingRight;
    bool  isMoving;
    bool  onGround;
    int gems;
    float screenWidth;
    float screenHeight;

    int   lives;
    int   score;

    sf::Texture texture;
    sf::Sprite sprite;
    int charOption;
    float currentScale;

    sf::Clock animationClock;
    int currentFrame;
    float frameDuration;

    int playerNum;
    bool isAttacking;
    float attackTimer;

    // Respawn data
    float spawnX;
    float spawnY;

    // Invincibility after hit
    bool  isInvincible;
    float invincibleTimer;
    float invincibleDuration;

    // Power-ups
    float speedMult;
    bool hasInstantEncase;
    bool hasInfiniteDistance;
    bool isBalloon;
    float speedTimer;
    float balloonTimer;

public:
    Player(int playerNumber, int charOption);

    void startAttack() { isAttacking = true; attackTimer = 0.45f; currentFrame = 0; }

    
    void applyPowerUp(int type);
    bool canInstantEncase() const { return hasInstantEncase; }
    bool canTravelInfinite() const { return hasInfiniteDistance; }

    std::string getActivePowerUpName() const;
    float getPowerUpTimeRemaining() const;

    // MOVEMENT
    void moveRight();
    void moveLeft();
    void moveUp();
    void stopMoving();
    void landOnPlatform(float platformTopY);

    void update(float deltaTime) override;
    void checkCollision(Levels& level);

    // ACTIONS
    void loseLife();
    void draw(sf::RenderWindow& window) override;

    //GETTERS
    int  getGems() const { return gems; }        
    void addGems(int amount) { gems += amount; }
    float getX() { return x; }
    float getY() { return y; }
    float getVelocityY() { return velocityy; }
    bool  getOnGround() { return onGround; }
    bool  getisFacingRight() { return facingRight; }
    int   getLives() { return lives; }
    int   getScore() { return score; }
    bool  getIsInvincible() { return isInvincible; }

    //SETTERS 
    void setPosition(float newX, float newY) override {
        x = newX;
        y = newY;
    }
    void addScore(int s) { score += s; }
};