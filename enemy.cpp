#include "enemy.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <cmath>

using namespace std;

// ------------------------------------------------------------------------------
// Enemy base
// ------------------------------------------------------------------------------
Enemy::Enemy() {
    srand(static_cast<unsigned int>(time(0)));
    screenWidth = 1280;
    screenHeight = 720;

    x = 100.f + (float)(rand() % 1000);
    y = screenHeight - 100;

    velocityx = 0.0f;
    velocityy = 0.0f;

    isAlive = true;
    onGround = false;
    gravity = 800.0f;

    int right = rand() % 2;
    facingRight = (right == 1);

    snowHits = 0;
    hitsToEncase = 2;
    isEncased = false;
    EscapeTimer = 0;

    jumpForce = 550.0f;
    jumpDuration = 4.0f;
    jumpTimer = 0.0f;

    hbh = 250.0f;
    hbw = 40;
    hbx = x; hby = y;

    levelRef = nullptr;

    texLoaded = false;
    currentFrame = 0;
    frameDuration = 0.15f;

    hitboxShape.setSize(sf::Vector2f(hbw, hbh));
    hitboxShape.setFillColor(sf::Color::Transparent);
    hitboxShape.setOutlineColor(sf::Color::Red);
    hitboxShape.setOutlineThickness(2.0f);
}

void Enemy::applyGravity(float deltaTime) {
    if (!onGround)
        velocityy += gravity * deltaTime;
}

void Enemy::checkPlatformCollision(Levels& level) {
    onGround = false;

    for (int i = 0; i < level.getPlatformCount(); i++) {
        float platX = level.getPlatformX(i);
        float platY = level.getPlatformY(i);
        float platH = level.getPlatformH(i);
        float platW = level.getPlatformW(i);

        bool withinX = (x + hbw > platX) && (x < platX + platW);
        bool withinY = (y + hbh >= platY) &&
            (y + hbh <= platY + platH + 5) &&
            (velocityy >= 0);

        if (withinX && withinY) {
            y = platY - hbh;
            velocityy = 0;
            onGround = true;
            break;
        }

        bool overlapY = (y + hbh > platY + 5) && (y < platY + platH - 5);
        if (overlapY) {
            if (x + hbw > platX && x + hbw < platX + 10 && velocityx > 0) {
                x = platX - hbw;
                velocityx = 0;
            }
            else if (x < platX + platW && x > platX + platW - 10 && velocityx < 0) {
                x = platX + platW;
                velocityx = 0;
            }
        }
    }

    if (y > screenHeight - hbh) {
        y = screenHeight - hbh;
        velocityy = 0;
        onGround = true;
    }
}

void Enemy::updateHitBox() {
    hbx = x; hby = y;
    hitboxShape.setPosition(hbx, hby);
    shape.setPosition(x, y);
    if (texLoaded) {
        sprite.setPosition(x + hbw / 2.f, y + hbh / 2.f);
    }
}

void Enemy::drawHitBox(sf::RenderWindow& window) {
    if (!isAlive) return;
    window.draw(hitboxShape);
}

void Enemy::takeAttack() {
    snowHits++;
    if (snowHits >= hitsToEncase)
        encaseEnemy();
}

void Enemy::encaseEnemy() {
    isEncased = true;
    velocityx = 0;
    velocityy = 0;
    EscapeTimer = 0;
    shape.setFillColor(sf::Color::White);
}

void Enemy::rollOnn(bool PisFacingRight) {
    facingRight = PisFacingRight;
    velocityx = facingRight ? 500.f : -500.f;
}

// -------------------------------------------------------------------------------
// Bottom
// -----------------------------------------------------------------------------
Bottom::Bottom() {
    speed = 150.0f;
    walkTimer = 0;
    walkDuration = 2.0f;

    hbw = 70;
    hbh = 80;

    shape.setSize(sf::Vector2f(hbw, hbh));
    shape.setFillColor(sf::Color(200, 50, 50));
    shape.setPosition(x, y);

    texLoaded = tex.loadFromFile("assets/Images/Botom_Pink.png");
    if (texLoaded) {
        sprite.setTexture(tex);
        sprite.setTextureRect(sf::IntRect(8, 136, 86, 87));
        sprite.setScale(1.0f, 1.0f);

        sf::FloatRect b = sprite.getLocalBounds();
        sprite.setOrigin(b.width / 2.f, b.height / 2.f);
    }

    frameDuration = 0.25f;
    hitboxShape.setSize(sf::Vector2f(hbw, hbh));
    updateHitBox();
}

void Bottom::move() {
    if (!isEncased)
        velocityx = facingRight ? speed : -speed;
}

void Bottom::update(float deltaTime) {
    if (!isAlive) return;

    if (animClock.getElapsedTime().asSeconds() >= frameDuration) {
        currentFrame++;
        animClock.restart();
    }

    if (isEncased) {
        EscapeTimer += deltaTime;
        if (EscapeTimer >= 4.0f) {
            isEncased = false;
            snowHits = 0;
            shape.setFillColor(sf::Color(200, 50, 50));
        }

        applyGravity(deltaTime);
        x += velocityx * deltaTime;
        y += velocityy * deltaTime;

        if (x > screenWidth + 60 || x < -60) {
            isAlive = false;
            return;
        }

        if (levelRef != nullptr) checkPlatformCollision(*levelRef);
        updateHitBox();

        if (texLoaded) {
            if (velocityx != 0.f) {
                sf::IntRect frames[2] = {
                    sf::IntRect(4,  712, 93, 91),
                    sf::IntRect(98, 711, 93, 91)
                };
                sprite.setTextureRect(frames[currentFrame % 2]);
            }
            else {
                if (EscapeTimer >= 3.0f) {
                    sf::IntRect frames[3] = {
                        sf::IntRect(1, 824, 93, 91),
                        sf::IntRect(97, 824, 93, 91),
                        sf::IntRect(195, 823, 93, 91)
                    };
                    sprite.setTextureRect(frames[currentFrame % 3]);
                }
                else {
                    sf::IntRect frames[2] = {
                        sf::IntRect(4, 596, 93, 94),
                        sf::IntRect(4, 712, 93, 91)
                    };
                    sprite.setTextureRect(frames[currentFrame % 2]);
                }
            }
            float sc = 1.0f;
            sprite.setScale(facingRight ? -sc : sc, sc);
        }
        return;
    }

    move();
    applyGravity(deltaTime);
    x += velocityx * deltaTime;
    y += velocityy * deltaTime;

    if (x > screenWidth - 95) { x = screenWidth - 95; facingRight = false; }
    if (x < 10) { x = 10; facingRight = true; }

    walkTimer += deltaTime;
    if (walkTimer >= walkDuration) {
        facingRight = (rand() % 2 == 0);
        walkDuration = 1.5f + (rand() % 2);
        walkTimer = 0;
    }

    jumpTimer += deltaTime;
    if (jumpTimer >= jumpDuration) {
        velocityy = -jumpForce;
        jumpTimer = 0;
    }

    if (levelRef != nullptr) checkPlatformCollision(*levelRef);
    updateHitBox();

    if (texLoaded) {
        sf::IntRect frames[3] = {
            sf::IntRect(3, 366, 94, 87),
            sf::IntRect(95, 364, 94, 87),
            sf::IntRect(192, 367, 94, 87)
        };
        sprite.setTextureRect(frames[currentFrame % 3]);

        float sc = 1.0f;
        sprite.setScale(facingRight ? -sc : sc, sc);
    }
}

void Bottom::draw(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (texLoaded) window.draw(sprite);
    else           window.draw(shape);
}

//----------------------------------------------- Flying -----------------------------------------------
Flyng::Flyng() {
    isFlying = false;
    groundTimer = 0;
    flightTimer = 0;
    groundDuration = 3.0f;
    flightDuration = 2.0f;
    flightDirX = 1.0f;
    flightDirY = -1.0f;

    hbw = 120;
    hbh = 120;

    texLoaded = tex.loadFromFile("assets/Images/FlyingFoogaFoog_Blue.png");
    if (texLoaded) {
        sprite.setTexture(tex);
        sprite.setTextureRect(sf::IntRect(44, 32, 110, 156));
        sprite.setScale(0.7f, 0.7f);

        sf::FloatRect b = sprite.getLocalBounds();
        sprite.setOrigin(b.width / 2.f, b.height / 2.f);
    }

    frameDuration = 0.25f;
    hitboxShape.setSize(sf::Vector2f(hbw, hbh));
    updateHitBox();
}

void Flyng::fly(float deltaTime) {
    if (!isFlying) return;
    x += flightDirX * speed * deltaTime;
    y += flightDirY * speed * deltaTime;
    if (x > screenWidth - hbw - 10) { x = screenWidth - hbw - 10; flightDirX = -1; }
    if (x < 10) { x = 10; flightDirX = 1; }
    if (y > screenHeight - 50) { y = screenHeight - 50; flightDirY = -1; }
    if (y < 0) { y = 0;                 flightDirY = 1; }
}

void Flyng::update(float deltaTime) {
    if (!isAlive) return;

    if (animClock.getElapsedTime().asSeconds() >= frameDuration) {
        currentFrame++;
        animClock.restart();
    }

    if (isEncased) {
        EscapeTimer += deltaTime;
        if (EscapeTimer >= 4.0f) {
            isEncased = false;
            snowHits = 0;
        }

        applyGravity(deltaTime);
        x += velocityx * deltaTime;
        y += velocityy * deltaTime;

        if (x > screenWidth + 60 || x < -60) {
            isAlive = false;
            return;
        }

        if (levelRef != nullptr) checkPlatformCollision(*levelRef);

        if (texLoaded) {
            if (EscapeTimer >= 3.0f) {
                sf::IntRect frames[2] = {
                    sf::IntRect(222, 432, 155, 123),
                    sf::IntRect(415, 664, 145,  86)
                };
                sprite.setTextureRect(frames[currentFrame % 2]);
            }
            else {
                sprite.setTextureRect(sf::IntRect(24, 611, 161, 140));
            }
            float sc = 0.7f;
            sprite.setScale(sc, sc);
        }
        updateHitBox();
        return;
    }

    if (!isFlying) {
        Bottom::update(deltaTime);
        if (texLoaded) {
            sprite.setTextureRect(sf::IntRect(44, 32, 110, 156));
            float sc = 0.7f;
            sprite.setScale(facingRight ? sc : -sc, sc);
        }
        groundTimer += deltaTime;
        if (groundTimer >= groundDuration) {
            isFlying = true;
            groundTimer = 0;
            flightDirX = (rand() % 2 == 0) ? 1.0f : -1.0f;
            flightDirY = (rand() % 2 == 0) ? 1.0f : -1.0f;
        }
    }
    else {
        fly(deltaTime);
        flightTimer += deltaTime;
        if (flightTimer >= flightDuration) {
            isFlying = false;
            flightTimer = 0;
        }
        if (texLoaded) {
            sf::IntRect frames[3] = {
                sf::IntRect(44,  32, 110, 156),
                sf::IntRect(208,  18, 176, 172),
                sf::IntRect(434,  26, 138, 154)
            };
            sprite.setTextureRect(frames[currentFrame % 3]);
            float sc = 0.7f;
            sprite.setScale(flightDirX >= 0 ? -sc : sc, sc);
        }
        updateHitBox();
    }
}

void Flyng::draw(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (texLoaded) window.draw(sprite);
    else           window.draw(shape);
}

// Tornado
Tornado::Tornado() {
    teleportTimer = 0;
    teleportDuration = 5.0f;
    shape.setFillColor(sf::Color(150, 0, 200));
    knifeTimer = 0.f;
    knifeInterval = 3.0f;
    wantsToThrow = false;
    playerX = 0.f;
    playerY = 0.f;

    hbw = 110;
    hbh = 110;

    texLoaded = tex.loadFromFile("assets/Images/Tornado_Blue.png");
    if (texLoaded) {
        sprite.setTexture(tex);
        sprite.setTextureRect(sf::IntRect(15, 38, 108, 122));
        sprite.setScale(0.85f, 0.85f);

        sf::FloatRect b = sprite.getLocalBounds();
        sprite.setOrigin(b.width / 2.f, b.height / 2.f);
    }

    frameDuration = 0.25f;
    hitboxShape.setSize(sf::Vector2f(hbw, hbh));
    updateHitBox();
}

void Tornado::teleport() {
    x = (float)(rand() % (int)screenWidth);
    y = (float)(rand() % (int)(screenHeight / 2));
}

void Tornado::update(float deltaTime) {
    if (!isAlive) return;

    knifeTimer += deltaTime;
    if (knifeTimer >= knifeInterval) {
        knifeTimer = 0.f;
        wantsToThrow = true;
    }

    if (animClock.getElapsedTime().asSeconds() >= frameDuration) {
        currentFrame++;
        animClock.restart();
    }

    if (isEncased) {
        EscapeTimer += deltaTime;
        if (EscapeTimer >= 4.0f) {
            isEncased = false;
            snowHits = 0;
        }
        applyGravity(deltaTime);
        x += velocityx * deltaTime;
        y += velocityy * deltaTime;

        if (x > screenWidth + 60 || x < -60) {
            isAlive = false;
            return;
        }

        if (levelRef != nullptr) checkPlatformCollision(*levelRef);

        if (texLoaded) {
            if (EscapeTimer >= 3.0f) {
                sf::IntRect frames[3] = {
                    sf::IntRect(16, 377, 118, 117),
                    sf::IntRect(159, 377, 113, 117),
                    sf::IntRect(291, 376,  97, 118)
                };
                sprite.setTextureRect(frames[currentFrame % 3]);
            }
            else {
                sf::IntRect frames[2] = {
                    sf::IntRect(158, 210, 109, 120),
                    sf::IntRect(293, 222, 112, 105)
                };
                sprite.setTextureRect(frames[currentFrame % 2]);
            }
            float sc = 0.85f;
            sprite.setScale(sc, sc);
        }
        updateHitBox();
        return;
    }

    Flyng::update(deltaTime);

    teleportTimer += deltaTime;
    if (teleportTimer >= teleportDuration) {
        teleport();
        teleportTimer = 0;
    }

    if (texLoaded) {
        if (isFlying) {
            sf::IntRect frames[3] = {
                sf::IntRect(305, 1043,  92, 123),
                sf::IntRect(417, 1046, 139, 120),
                sf::IntRect(557, 1047,  87, 128)
            };
            sprite.setTextureRect(frames[currentFrame % 3]);
        }
        else {
            sf::IntRect frames[3] = {
                sf::IntRect(147, 40, 121, 119),
                sf::IntRect(286, 40, 119, 118),
                sf::IntRect(420, 35, 129, 123)
            };
            sprite.setTextureRect(frames[currentFrame % 3]);
        }
        float sc = 0.85f;
        sprite.setScale(facingRight ? -sc : sc, sc);
    }
}

void Tornado::draw(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (texLoaded) window.draw(sprite);
    else           window.draw(shape);
}

// -------------------------------------MogeraChild------------------------------------------------
MogeraChild::MogeraChild(float startX, float startY, float direction)
    : moveDir(direction)
{
    x = startX;
    y = startY;

    speed = 120.0f;
    hbw = 40;
    hbh = 40;
    hitsToEncase = 1;

    shape.setSize(sf::Vector2f(hbw, hbh));
    shape.setFillColor(sf::Color(200, 150, 50));
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(1.f);

    texLoaded = tex.loadFromFile("assets/Images/Mogera.png");
    if (texLoaded) {
        sprite.setTexture(tex);
        sprite.setTextureRect(sf::IntRect(1797, 798, 174, 186));
        sprite.setScale(0.22f, 0.22f);

        sf::FloatRect b = sprite.getLocalBounds();
        sprite.setOrigin(b.width / 2.f, b.height / 2.f);
    }

    hitboxShape.setSize(sf::Vector2f(hbw, hbh));
    hitboxShape.setFillColor(sf::Color::Transparent);
    hitboxShape.setOutlineColor(sf::Color::Red);
    hitboxShape.setOutlineThickness(2.f);
}

void MogeraChild::update(float deltaTime) {
    if (!isAlive) return;

    if (animClock.getElapsedTime().asSeconds() >= frameDuration) {
        currentFrame++;
        animClock.restart();
    }

    if (isEncased) {
        EscapeTimer += deltaTime;
        if (EscapeTimer >= 4.0f) {
            isEncased = false;
            snowHits = 0;
        }
        updateHitBox();
        return;
    }

    velocityx = moveDir * speed;
    applyGravity(deltaTime);

    x += velocityx * deltaTime;
    y += velocityy * deltaTime;

    if (x > screenWidth - hbw) { x = screenWidth - hbw; moveDir = -1.0f; }
    if (x < 0) { x = 0;                 moveDir = 1.0f; }

    if (levelRef != nullptr) checkPlatformCollision(*levelRef);
    updateHitBox();

    if (texLoaded) {
        if (isEncased) {
            sf::IntRect frames[3] = {
                sf::IntRect(1797, 978, 156, 183),
                sf::IntRect(1971, 987, 192, 174),
                sf::IntRect(2163, 990, 159, 165)
            };
            sprite.setTextureRect(frames[currentFrame % 3]);
        }
        else {
            sf::IntRect frames[4] = {
                sf::IntRect(1797, 798, 174, 186),
                sf::IntRect(1965, 801, 186, 186),
                sf::IntRect(2133, 774, 189, 198),
                sf::IntRect(2319, 792, 150, 210)
            };
            sprite.setTextureRect(frames[currentFrame % 4]);
        }
        float sc = 0.22f;
        sprite.setScale(moveDir > 0 ? -sc : sc, sc);
    }
}

void MogeraChild::draw(sf::RenderWindow& window) {
    if (!isAlive) return;

    if (isEncased) {
        sf::CircleShape snow(12.f);
        snow.setFillColor(sf::Color(220, 240, 255));
        snow.setOutlineColor(sf::Color::White);
        snow.setOutlineThickness(1.5f);
        snow.setPosition(x, y);
        window.draw(snow);
        return;
    }

    if (texLoaded) {
        window.draw(sprite);
    }
    else {
        shape.setPosition(x, y);
        window.draw(shape);
        sf::CircleShape eye(3.f);
        eye.setFillColor(sf::Color::White);
        eye.setPosition(x + 4, y + 6); window.draw(eye);
        eye.setPosition(x + 14, y + 6); window.draw(eye);
    }
}

// ---------------------------------------Mogera ------------------------------------------
Mogera::Mogera() {
    texLoaded = tex.loadFromFile("assets/Images/Mogera.png");
    if (texLoaded) {
        sprite.setTexture(tex);
        sprite.setTextureRect(sf::IntRect(0, 3, 576, 504));
        sprite.setScale(0.49f, 0.49f);

        sf::FloatRect b = sprite.getLocalBounds();
        sprite.setOrigin(b.width / 2.f, b.height / 2.f);
    }

    hitsToEncase = 15;
    hbw = 200;
    hbh = 180;

    maxHealth = 1000.0f;
    health = maxHealth;
    childCount = 0;
    for (int i = 0; i < MAX_CHILDREN; i++) children[i] = nullptr;

    spawnTimer = 0.f;
    wantsToSpawn = false;
    spawnX = 0.f;
    spawnY = 0.f;
    spawnDir = 1.f;
}

void Mogera::spawnChild(float pX, float pY) {
    if (childCount < MAX_CHILDREN) {
        float dir = (pX > x) ? 1.0f : -1.0f;
        float sx = x + hbw / 2.f + dir * (hbw / 2.f + 10.f);
        float sy = y + hbh - 24;

        children[childCount] = new MogeraChild(sx, sy, dir);
        if (levelRef) children[childCount]->setLevel(*levelRef);
        childCount++;
    }
}

void Mogera::updateChildren(float deltaTime, Levels& level) {
    for (int i = 0; i < childCount; i++) {
        if (children[i] != nullptr) {
            children[i]->update(deltaTime);
            if (!children[i]->getIsAlive()) {
                delete children[i];
                children[i] = children[childCount - 1];
                children[childCount - 1] = nullptr;
                childCount--;
                i--;
            }
        }
    }
}

void Mogera::drawChildren(sf::RenderWindow& window) {
    for (int i = 0; i < childCount; i++) {
        if (children[i] != nullptr) children[i]->draw(window);
    }
}

void Mogera::drawChildHitBoxes(sf::RenderWindow& window) {
    for (int i = 0; i < childCount; i++) {
        if (children[i] != nullptr) children[i]->drawHitBox(window);
    }
}

void Mogera::drawHitBox(sf::RenderWindow& window) {
    Enemy::drawHitBox(window);
    drawChildHitBoxes(window);
}

void Mogera::update(float deltaTime) {
    if (!isAlive) return;

    if (animClock.getElapsedTime().asSeconds() >= frameDuration) {
        currentFrame++;
        animClock.restart();
    }

    if (texLoaded) {
        sf::IntRect frames[3] = {
            sf::IntRect(0,    3, 576, 504),
            sf::IntRect(576,  3, 573, 495),
            sf::IntRect(1155, 3, 585, 483)
        };
        sprite.setTextureRect(frames[currentFrame % 3]);
        sprite.setPosition(x + hbw / 2.f, y + hbh / 2.f);
    }

    spawnTimer += deltaTime;
    if (spawnTimer >= SPAWN_INTERVAL) {
        spawnTimer = 0.f;
        wantsToSpawn = true;
        spawnChild(0, 0);
    }

    if (levelRef) updateChildren(deltaTime, *levelRef);
    updateHitBox();
}

void Mogera::takeAttack() {
    health -= 100.0f;
    if (health <= 0.0f) {
        health = 0.0f;
        setAlive(false);
    }
}

void Mogera::draw(sf::RenderWindow& window) {
    if (!isAlive) return;
    if (texLoaded) window.draw(sprite);
    else           window.draw(shape);

    drawChildren(window);
}

//------------------------------------------ GAMAKICHI ------------------------------------------------
Gamakichi::Gamakichi() {
    speed = 0.0f;
    gravity = 0.0f;
    onGround = true;

    health = 5000.0f;
    maxHealth = 5000.0f;

    attackPhase = 1;
    fireTimer = 0.0f;
    fireInterval = 2.5f;
    wantsToFire = false;
    rocketCount = 0;

    playerX = 640.0f;
    playerY = 500.0f;

    hbw = 300.0f;
    hbh = 200.0f;

    hbx = x;
    hby = y;

    hitboxShape.setSize(sf::Vector2f(hbw, hbh));
    hitboxShape.setFillColor(sf::Color::Transparent);
    hitboxShape.setOutlineColor(sf::Color::Magenta);
    hitboxShape.setOutlineThickness(3.0f);

    shape.setSize(sf::Vector2f(hbw, hbh));
    shape.setFillColor(sf::Color(30, 120, 30));
    shape.setOutlineColor(sf::Color(0, 80, 0));
    shape.setOutlineThickness(3.0f);
    shape.setPosition(x, y);

    texLoaded = tex.loadFromFile("assets/Images/Gamakichi.png");
    if (texLoaded) {
        sprite.setTexture(tex);
        sprite.setTextureRect(sf::IntRect(16, 0, 1240, 659));
        sprite.setScale(hbw / 1240.f, hbh / 659.f);
        sprite.setPosition(x, y);
    }

    updateHitBox();

    currentFrame = 0;
    frameDuration = 0.3f;
}

void Gamakichi::updatePhase() {
    float pct = health / maxHealth;
    int   newPhase;

    if (pct > 0.66f) newPhase = 1;
    else if (pct > 0.33f) newPhase = 2;
    else                  newPhase = 3;

    if (newPhase != attackPhase) {
        attackPhase = newPhase;
        if (attackPhase == 1) fireInterval = 2.5f;
        else if (attackPhase == 2) fireInterval = 1.5f;
        else                       fireInterval = 0.8f;
    }
}

void Gamakichi::fireVolley() {
    float ox = x + hbw / 2.f;
    float oy = y + hbh;

    float tdx = playerX - ox;
    float tdy = playerY - oy;

    if (tdy < 0) tdy = 0;
    float len = std::sqrt(tdx * tdx + tdy * tdy);
    if (len < 1.f) len = 1.f;
    tdx /= len;  tdy /= len;
    if (tdy < 0.3f) tdy = 0.3f;

    len = std::sqrt(tdx * tdx + tdy * tdy);
    tdx /= len;  tdy /= len;
    if (tdy < 0.4f) tdy = 0.4f;
    float rocketSpeed = 280.f + attackPhase * 40.f;

    rocketCount = 0;

    if (attackPhase == 1) {
        rockets[rocketCount++].launch(ox, oy, -0.5f, 0.87f, rocketSpeed);
        rockets[rocketCount++].launch(ox, oy, 0.0f, 1.0f, rocketSpeed);
        rockets[rocketCount++].launch(ox, oy, 0.5f, 0.87f, rocketSpeed);
    }
    else if (attackPhase == 2) {
        rockets[rocketCount++].launch(ox, oy, -1.f, 0.f, rocketSpeed);
        rockets[rocketCount++].launch(ox, oy, 1.f, 0.f, rocketSpeed);
        rockets[rocketCount++].launch(ox, oy, tdx, tdy, rocketSpeed);
        rockets[rocketCount++].launch(ox, oy, -tdx, tdy, rocketSpeed);
    }
    else {
        rockets[rocketCount++].launch(ox, oy, -1.f, 0.f, rocketSpeed);
        rockets[rocketCount++].launch(ox, oy, 1.f, 0.f, rocketSpeed);
        rockets[rocketCount++].launch(ox, oy, tdx, tdy, rocketSpeed);
        rockets[rocketCount++].launch(ox, oy, -tdx, tdy, rocketSpeed);
        rockets[rocketCount++].launch(ox, oy, -0.5f, 0.87f, rocketSpeed);
        rockets[rocketCount++].launch(ox, oy, 0.5f, 0.87f, rocketSpeed);
    }
}

void Gamakichi::update(float deltaTime) {
    if (!getIsAlive()) return;

    updatePhase();

    if (animClock.getElapsedTime().asSeconds() >= frameDuration) {
        currentFrame++;
        animClock.restart();
    }

    if (texLoaded) {
        if (currentFrame % 2 == 0)
            sprite.setTextureRect(sf::IntRect(16, 0, 1240, 659));
        else
            sprite.setTextureRect(sf::IntRect(1340, 0, 1210, 668));
        sprite.setPosition(x, y);
    }
    shape.setPosition(x, y);

    fireTimer += deltaTime;
    if (fireTimer >= fireInterval) {
        fireTimer = 0.0f;
        wantsToFire = true;
        fireVolley();
    }

    updateHitBox();
}

void Gamakichi::updateRockets(float dt) {
    for (int i = 0; i < MAX_ROCKETS; i++)
        rockets[i].update(dt, screenWidth, screenHeight);
}

void Gamakichi::draw(sf::RenderWindow& window) {
    if (!getIsAlive()) return;
    if (texLoaded) window.draw(sprite);
    else           window.draw(shape);
    drawRockets(window);
}

void Gamakichi::drawRockets(sf::RenderWindow& window) {
    for (int i = 0; i < MAX_ROCKETS; i++)
        rockets[i].draw(window);
}

void Gamakichi::drawHitBox(sf::RenderWindow& window) {
    if (!getIsAlive()) return;
    window.draw(hitboxShape);
}

void Gamakichi::takeAttack() {
    health -= 50.0f;
    if (health <= 0.0f) {
        health = 0.0f;
        setAlive(false);
    }
}