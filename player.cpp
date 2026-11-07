#include "player.h"
#include <iostream>
//
using namespace std;
//
Player::Player(int playerNumber, int option) {
    playerNum = playerNumber;
    charOption = option;
    x = 0;
    y = 0;
    currentFrame = 0;
    frameDuration = 0.15f;

    extern int gameGems;
    gems = gameGems;

    if (playerNumber == 1) { x = 100.0f; y = 500.0f; }
    else { x = 300.0f; y = 500.0f; }

    velocityx = 0.0f; velocityy = 0.0f;
    speed = 200.0f; jumpForce = 550.0f; gravity = 800.0f;
    onGround = true; facingRight = (playerNumber == 1);
    isMoving = false;
    screenWidth = 1280; screenHeight = 720;
    if (playerNumber == 1) {
        extern int gameLives;
        lives = (gameLives > 0) ? gameLives : 3;
    }
    else {
        lives = 3;
    }
    score = 0;
    isAttacking = false;
    attackTimer = 0;
    spawnX = x;
    spawnY = y;
    isInvincible = false;
    invincibleTimer = 0.0f;
    invincibleDuration = 2.0f;
    speedMult = 1.0f;
    hasInstantEncase = false;
    hasInfiniteDistance = false;
    isBalloon = false;
    speedTimer = 0.0f;
    balloonTimer = 0.0f;

    if (charOption == 0) texture.loadFromFile("assets/Images/Nick.png");
    else if (charOption == 1) texture.loadFromFile("assets/Images/Player_Red.png");
    else if (charOption == 2) texture.loadFromFile("assets/Images/Player_Green.png");
    else if (charOption == 3) texture.loadFromFile("assets/Images/Player_Yellow.png");
    else if (charOption == 4) texture.loadFromFile("assets/Images/Player_Blue.png");

    sprite.setTexture(texture);
    if (charOption == 0) {
        sprite.setTextureRect(sf::IntRect(124, 664, 206, 271));
        currentScale = 0.25f;
    }
    else {
        sprite.setTextureRect(sf::IntRect(15, 4, 61, 73));
        currentScale = 1.0f;
    }

    sf::FloatRect gb = sprite.getGlobalBounds();
    hitboxShape.setSize(sf::Vector2f(gb.width, gb.height));
    hitboxShape.setPosition(gb.left, gb.top);

    sf::FloatRect b = sprite.getLocalBounds();
    sprite.setOrigin(b.width / 2.f, b.height / 2.f);
    sprite.setScale(currentScale, currentScale);
    sprite.setPosition(x, y);
}

void Player::moveRight() {
    velocityx = speed * speedMult;
    facingRight = true;
    isMoving = true;
}

void Player::moveLeft() {
    velocityx = -speed * speedMult;
    facingRight = false;
    isMoving = true;
}
void Player::stopMoving() { velocityx = 0; isMoving = false; }
void Player::moveUp() { if (onGround) { velocityy = -jumpForce; onGround = false; } }

void Player::landOnPlatform(float platformTopY) {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    y = platformTopY - (bounds.height / 2.f);
    velocityy = 0;
    onGround = true;
}

void Player::checkCollision(Levels& level) {

    bool foundPlatform = false;
    sf::FloatRect bounds = sprite.getGlobalBounds();

    float halfW = bounds.width / 2.f;
    float halfH = bounds.height / 2.f;

    for (int i = 0; i < level.getPlatformCount(); i++) {
        float pX = level.getPlatformX(i);
        float pY = level.getPlatformY(i);
        float pW = level.getPlatformW(i);
        float pH = level.getPlatformH(i);

        // ------------------------TOP COLLISION ----------------------------
        bool withinX = (x + halfW > pX) && (x - halfW < pX + pW);
        bool landing = (y + halfH >= pY) &&
            (y + halfH <= pY + pH + 5) &&
            (velocityy >= 0);

        if (withinX && landing) {
            landOnPlatform(pY);
            foundPlatform = true;
            break;
        }

        //--------------------------------SIDE COLLISION-----------------------------
       
        bool vertOverlap = (y - halfH < pY + pH - 5) &&
            (y + halfH > pY + 5);

        if (vertOverlap) {

            // hitting LEFT wall of platform 
            if (velocityy >= -50.0f &&
                velocityx > 0 &&
                x + halfW > pX &&
                x + halfW < pX + pW * 0.6f)
            {
                x = pX - halfW;
                velocityx = 0;
            }

            // hitting RIGHT wall of platform 
            else if (velocityy >= -50.0f &&
                velocityx < 0 &&
                x - halfW < pX + pW &&
                x - halfW > pX + pW * 0.4f)
            {
                x = pX + pW + halfW;
                velocityx = 0;
            }
        }
    }
    
        // not on any platform
    if (!foundPlatform && y < screenHeight - halfH)
        onGround = false;
}

void Player::update(float deltaTime) {
    //  countdown
    if (isInvincible) {
        invincibleTimer += deltaTime;
        if (invincibleTimer >= invincibleDuration)
            isInvincible = false;

        // Visual flashing effect
        if ((int)(invincibleTimer * 10) % 2 == 0)
            sprite.setColor(sf::Color(255, 255, 255, 128));
        else
            sprite.setColor(sf::Color::White);
    }
    else {
        sprite.setColor(sf::Color::White);
    }

    // Timers
    if (speedTimer > 0) {
        speedTimer -= deltaTime;
        if (speedTimer <= 0) speedMult = 1.0f;
    }
    if (balloonTimer > 0) {
        balloonTimer -= deltaTime;
        if (balloonTimer <= 0) isBalloon = false;
    }

    if (isBalloon) {
        velocityy = -100.0f; // Float upwards
        onGround = false;
    }
    else if (!onGround) {
        velocityy += gravity * deltaTime;
    }

    x += velocityx * deltaTime;
    y += velocityy * deltaTime;

    if (x > screenWidth)  x = 0;
    if (x < 0)            x = screenWidth;

    sf::FloatRect bounds = sprite.getGlobalBounds();
    if (y > screenHeight - (bounds.height / 2.f)) {
        y = screenHeight - (bounds.height / 2.f);
        velocityy = 0;
        onGround = true;
    }
    if (y < 0) { y = 0; velocityy = 0; }

    // Check for attack trigger
    if (!isAttacking) {
        if (playerNum == 1 && sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) startAttack();
        if (playerNum == 2 && sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) startAttack();
    }

    if (isAttacking) {
        attackTimer -= deltaTime;
        if (attackTimer <= 0) isAttacking = false;
    }

    if (animationClock.getElapsedTime().asSeconds() > frameDuration) {
        currentFrame++;
        animationClock.restart();
    }

    if (charOption == 0) { // Nick
        if (isAttacking) {
            sf::IntRect frames[4] = {
                sf::IntRect(130, 1280, 230, 290),
                sf::IntRect(430, 1280, 240, 295),
                sf::IntRect(740, 1280, 250, 300),
                sf::IntRect(1050, 1280, 260, 305)
            };
            sprite.setTextureRect(frames[currentFrame % 4]);
        }
        else if (isMoving) {
            if (!facingRight) {
                sf::IntRect frames[5] = {
                    sf::IntRect(110, 76, 220, 275),
                    sf::IntRect(407, 74, 196, 281),
                    sf::IntRect(702, 70, 226, 287),
                    sf::IntRect(1003, 67, 213, 276),
                    sf::IntRect(1287, 55, 243, 305)
                };
                sprite.setTextureRect(frames[currentFrame % 5]);
            }
            else {
                sf::IntRect frames[5] = {
                    sf::IntRect(115, 362, 214, 286),
                    sf::IntRect(418, 360, 201, 291),
                    sf::IntRect(705, 359, 218, 292),
                    sf::IntRect(1005, 359, 214, 289),
                    sf::IntRect(1305, 359, 221, 290)
                };
                sprite.setTextureRect(frames[currentFrame % 5]);
            }
        }
        else {
            sf::IntRect idles[3] = {
                sf::IntRect(124, 664, 206, 271),
                sf::IntRect(427, 667, 201, 273),
                sf::IntRect(727, 670, 206, 276)
            };
            sprite.setTextureRect(idles[currentFrame % 3]);
        }
    }
    else { // Others (Red, Green, Yellow, Blue)
        if (isAttacking) {
            sf::IntRect frames[7] = {
                sf::IntRect(7, 167, 68, 78),
                sf::IntRect(98, 166, 59, 80),
                sf::IntRect(178, 167, 65, 78),
                sf::IntRect(251, 171, 75, 76),
                sf::IntRect(424, 167, 75, 78),
                sf::IntRect(517, 169, 63, 77),
                sf::IntRect(601, 170, 63, 74)
            };
            sprite.setTextureRect(frames[currentFrame % 7]);
        }
        else if (isMoving) {
            if (!facingRight) {
                sf::IntRect frames[5] = {
                    sf::IntRect(15,  4, 61, 73),
                    sf::IntRect(96,  4, 52, 72),
                    sf::IntRect(171, 1, 61, 73),
                    sf::IntRect(265, 2, 53, 76),
                    sf::IntRect(346, 2, 58, 76)
                };
                sprite.setTextureRect(frames[currentFrame % 5]);
            }
            else {
                sf::IntRect frames[6] = {
                    sf::IntRect(8, 83, 61, 76),
                    sf::IntRect(94, 84, 59, 62),
                    sf::IntRect(177, 88, 60, 58),
                    sf::IntRect(266, 86, 56, 71),
                    sf::IntRect(354, 90, 57, 65),
                    sf::IntRect(436, 91, 62, 62)
                };
                sprite.setTextureRect(frames[currentFrame % 6]);
            }
        }
        else {
            sprite.setTextureRect(sf::IntRect(15, 4, 61, 73));
        }
    }

    sprite.setPosition(x, y);
    
    if (isAttacking || !isMoving) {
        if (!facingRight) sprite.setScale(currentScale, currentScale);
        else sprite.setScale(-currentScale, currentScale);
    }
    else {
        sprite.setScale(currentScale, currentScale);
    }
    sf::FloatRect gb = sprite.getGlobalBounds();
    hitboxShape.setSize(sf::Vector2f(gb.width, gb.height));
    hitboxShape.setPosition(gb.left, gb.top);
}

void Player::applyPowerUp(int type) {
    if (type == 0) { // Speed
        speedMult = 1.5f;
        speedTimer = 15.0f;
    }
    else if (type == 1) { // Snowball Power
        hasInstantEncase = true;
    }
    else if (type == 2) { // Distance
        hasInfiniteDistance = true;
    }
    else if (type == 3) { // Balloon
        isBalloon = true;
        balloonTimer = 10.0f;
    }
    else if (type == 5) { // Extra Life (PU_EXTRA = 5)
        lives++;
    }
}

std::string Player::getActivePowerUpName() const {
    if (speedTimer > 0) return "Speed Boost";
    if (balloonTimer > 0) return "Balloon Mode";
    if (hasInstantEncase) return "Snow Power";
    if (hasInfiniteDistance) return "Distance+";
    return "None";
}

float Player::getPowerUpTimeRemaining() const {
    if (speedTimer > 0) return speedTimer;
    if (balloonTimer > 0) return balloonTimer;
    return 0.0f;
}

void Player::draw(sf::RenderWindow& window) { window.draw(sprite); }
void Player::loseLife() {
    if (isInvincible) return;

    lives--;
    
    if (playerNum == 1) {
        extern int gameLives;
        gameLives = lives;
    }

    isInvincible = true;
    invincibleTimer = 0.0f;

    // Respawn at starting position
    x = spawnX;
    y = spawnY;
    velocityx = 0.0f;
    velocityy = 0.0f;
    onGround = true;
}


