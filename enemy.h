#pragma once
#include "Entity.h"
#include "levelManager.h"
#include <SFML/Graphics.hpp>

const int MAX_CHILDREN = 20;
const int MAX_ROCKETS = 6;

class Enemy : public Entity {
protected:
    float speed;
    float gravity;
    bool  facingRight;
    bool  onGround;
    float screenWidth;
    float screenHeight;

    // Jumping variables from user snippet
    float jumpForce;
    float jumpDuration;
    float jumpTimer;

    // Visuals
    sf::RectangleShape shape;
    sf::Sprite sprite;
    sf::Texture tex;
    bool texLoaded;
    sf::Clock animClock;
    int currentFrame;
    float frameDuration;

    float hbx, hby, hbh, hbw;

    int   snowHits;
    int   hitsToEncase;
    bool  isEncased;
    float EscapeTimer;
    int   rollingHitCount;

    Levels* levelRef;

public:
    Enemy();

    void setLevel(Levels& level) { levelRef = &level; }

    virtual void update(float deltaTime) override = 0;
    virtual void draw(sf::RenderWindow& window) override = 0;

    void applyGravity(float deltaTime);
    void checkPlatformCollision(Levels& level);
    void updateHitBox();
    virtual void drawHitBox(sf::RenderWindow& window);
    virtual void takeAttack();
    void encaseEnemy();

    void setPosition(float newX, float newY) override {
        x = newX;
        y = newY;
    }

    // GETTERS
    float getX() { return x; }
    float getY() { return y; }
    float getHBX() { return hbx; }
    float getHBY() { return hby; }
    float getHBW() { return hbw; }
    float getHBH() { return hbh; }
    float getvelocityx() { return velocityx; }
    float getSpeed() { return speed; }
    float getVelocityy() { return velocityy; }
    float getGravity() { return gravity; }
    bool  getFacingRight() { return facingRight; }
    bool  getonGround() { return onGround; }
    bool  getIsEncased() { return isEncased; }
    int   getRollingHitCount() { return rollingHitCount; }
    void  resetRollingHitCount() { rollingHitCount = 0; }
    void  incrementRollingHitCount() { rollingHitCount++; }

    // Logic from user snippet
    void rollOnn(bool PisFacingRight);
    void kill() { setAlive(false); }
    void setHitsToEncase(int n) { hitsToEncase = n; }

    virtual int getScoreValue() const = 0;
    virtual int getGemDrop()    const { return 1; }

    virtual ~Enemy() {}
};

// ---------------------------------Bottom-------------------------------------------------
class Bottom : public Enemy {
protected:
    float walkDuration;
    float walkTimer;

public:
    Bottom();
    void move();
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    int  getScoreValue() const override { return 100 + rand() % 401; }
};

//---------------------------------------------- Flyng-------------------------------------------
class Flyng : public Bottom {
protected:
    bool  isFlying;
    float groundTimer;
    float flightTimer;
    float groundDuration;
    float flightDuration;
    float flightDirX;
    float flightDirY;

public:
    Flyng();
    void fly(float deltaTime);
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    int  getScoreValue() const override { return 200 + rand() % 601; }
};

// ----------------------------------------Tornado-------------------------------------------------
class Tornado : public Flyng {
protected:
    float teleportTimer;
    float teleportDuration;
    float knifeTimer;
    float knifeInterval;
    bool  wantsToThrow;
    float playerX, playerY;

public:
    Tornado();
    void teleport();

    void setPlayerPos(float px, float py) { playerX = px; playerY = py; }

    bool pollWantsToThrow() {
        if (wantsToThrow) { wantsToThrow = false; return true; }
        return false;
    }
    float getKnifeOriginX() { return x + hbw / 2.f; }
    float getKnifeOriginY() { return y + hbh / 2.f; }

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    int  getScoreValue() const override { return 300 + rand() % 901; }
};

//--------------------------------------- MogeraChild-----------------------------------
class MogeraChild : public Enemy {
public:
   
    MogeraChild(float x, float y, float direction);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    int  getScoreValue() const override { return 50; }
    int  getGemDrop()    const override { return 0; }

private:
    float moveDir;   
};

// ------------------------------------------------Mogera --------------------------------------
class Mogera : public Enemy {
public:
    Mogera();

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    int  getScoreValue() const override { return 5000; }
    int  getGemDrop()    const override { return 200; }

    float playerX, playerY;

  

    void takeAttack() override;

    bool isDefeated() { return health <= 0; }
    int  getHealth() { return (int)health; }
    int  getMaxHealth() { return (int)maxHealth; }

    
    bool pollWantsToSpawn() {
        if (wantsToSpawn) { wantsToSpawn = false; return true; }
        return false;
    }
    float getSpawnX()   const { return spawnX; }
    float getSpawnY()   const { return spawnY; }
    float getSpawnDir() const { return spawnDir; }

    // child
    void spawnChild(float pX, float pY);
    void updateChildren(float deltaTime, Levels& level);
    void drawChildren(sf::RenderWindow& window);
    void drawChildHitBoxes(sf::RenderWindow& window);
    void drawHitBox(sf::RenderWindow& window) override;    MogeraChild* children[MAX_CHILDREN];
    int childCount;

private:
    float health;
    float maxHealth;

    float spawnTimer;
    bool  wantsToSpawn;
    float spawnX;
    float spawnY;
    float spawnDir;

    static constexpr float SPAWN_INTERVAL = 3.0f;

public:
    ~Mogera() {
        for (int i = 0; i < MAX_CHILDREN; i++) {
            if (children[i] != nullptr) {
                delete children[i];
                children[i] = nullptr;
            }
        }
    }
};
// ROCKETS------------------------------
struct GamaRocket {
    float x, y;           
    float dx, dy;         
    float speed;          
    bool  alive;
    bool  exploding;      // explosion animation
    float explodeTimer;   
    float explodeRadius;  

    // visualS
    sf::CircleShape    body;
    sf::CircleShape    blast;

    GamaRocket()
        : x(0), y(0), dx(0), dy(0), speed(300.f),
        alive(false), exploding(false), explodeTimer(0.f),
        explodeRadius(60.f)
    {
        body.setRadius(8.f);
        body.setFillColor(sf::Color(255, 140, 0));
        body.setOutlineColor(sf::Color::Yellow);
        body.setOutlineThickness(2.f);
        body.setOrigin(8.f, 8.f);

        blast.setRadius(explodeRadius);
        blast.setFillColor(sf::Color(255, 80, 0, 160));
        blast.setOutlineColor(sf::Color::Red);
        blast.setOutlineThickness(3.f);
        blast.setOrigin(explodeRadius, explodeRadius);
    }

    void launch(float sx, float sy, float dirX, float dirY, float spd = 300.f) {
        x = sx; y = sy;
        dx = dirX; dy = dirY;
        speed = spd;
        alive = true;
        exploding = false;
        explodeTimer = 0.f;
        body.setPosition(x, y);
    }

    bool update(float dt, float screenW, float screenH) {
        if (!alive) return false;
        if (exploding) {
            explodeTimer += dt;
            blast.setPosition(x, y);
            if (explodeTimer >= 0.5f) {
                alive = false;
                exploding = false;
            }
            return alive;
        }
        x += dx * speed * dt;
        y += dy * speed * dt;
        body.setPosition(x, y);
        if (x < 0 || x > screenW || y < 0 || y > screenH)
            explode();
        return alive;
    }

    void explode() {
        if (!exploding) {
            exploding = true;
            explodeTimer = 0.f;
        }
    }

    void draw(sf::RenderWindow& win) {
        if (!alive) return;
        if (exploding) win.draw(blast);
        else           win.draw(body);
    }

    bool blastHits(float rx, float ry, float rw, float rh) const {
        if (!exploding) return false;
        float cx = x, cy = y, r = explodeRadius;
        float nearX = std::max(rx, std::min(cx, rx + rw));
        float nearY = std::max(ry, std::min(cy, ry + rh));
        float ddx = cx - nearX, ddy = cy - nearY;
        return (ddx * ddx + ddy * ddy) < (r * r);
    }

    bool travelHits(float rx, float ry, float rw, float rh) const {
        if (!alive || exploding) return false;
        return (x > rx && x < rx + rw && y > ry && y < ry + rh);
    }
};

// ----------------------------------Gamakichi ------------------------------
class Gamakichi : public Enemy {
public:
    Gamakichi();

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    void drawHitBox(sf::RenderWindow& window) override;

    int getScoreValue() const override { return 10000; }
    int getGemDrop()    const override { return 500; }

    int  getHealth()     const { return (int)health; }
    int  getMaxHealth()  const { return (int)maxHealth; }

    void takeAttack() override;

    void updateRockets(float dt);
    void drawRockets(sf::RenderWindow& window);

    bool pollWantsToFire() {
        if (wantsToFire) { wantsToFire = false; return true; }
        return false;
    }

    GamaRocket rockets[MAX_ROCKETS];
    int        rocketCount;
    float      playerX, playerY;

private:
    float health;
    float maxHealth;
    int   attackPhase;
    float fireTimer;
    float fireInterval;
    bool  wantsToFire;

    void updatePhase();
    void fireVolley();
};