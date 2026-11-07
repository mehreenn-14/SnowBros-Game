#include "gameScreen.h"
#include "enemy.h"
#include "EnemyFactory.h"
#include "helper_functions.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

extern MusicManager gMusic;

GameScreen::GameScreen(sf::RenderWindow& win, Player* p1, Player* p2)
    : Screen(win), window(win), a1(p1, (float)win.getSize().x), a2(p2 ? p2 : p1, (float)win.getSize().x)
{
    isPaused = false;
    isGameOver = false;
    showHitBox = false;
    currentlvl = 1;
    deltaTime = 0;
    powerUpCount = 0;
    for (int i = 0; i < MAX_POWERUPS; i++) powerUps[i] = nullptr;

    isStarMenuOpen = false;
    starSpawned = false;
    starLevel = (rand() % 4) + 1;

    isBonusPhase = false;
    bonusTimer = 0.0f;
    freshStart = true;

    screenWidth = static_cast<float>(window.getSize().x);
    screenHeight = static_cast<float>(window.getSize().y);
    player1 = p1;
    player2 = p2;
    levelManager = nullptr;
    nextScreen = 7;
    for (int i = 0; i < MAX_ENEMIES; i++) enemies[i] = nullptr;
    enemyCount = 0;
}

void GameScreen::resetGame() {
    // Reset players
    if (player1) delete player1;
    if (player2) delete player2;

    
    if (gameLives <= 0) gameLives = 10;

    player1 = new Player(1, selectedCharP1);
    if (isMultiplayer) player2 = new Player(2, selectedCharP2);
    else               player2 = nullptr;

    a1.setPlayer(player1);
    if (player2) a2.setPlayer(player2);

    // Reset enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i]) { delete enemies[i]; enemies[i] = nullptr; }
    }
    enemyCount = 0;

    // Reset powerups
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerUps[i]) { delete powerUps[i]; powerUps[i] = nullptr; }
    }
    powerUpCount = 0;
    gameLives = 10;

    currentlvl = gameLevel;
    isBonusPhase = false;
    loadLevel(currentlvl);
}

void GameScreen::nextLevel() {
    //BOSS LEVEL COMPLETION
    if (currentlvl == 5 || currentlvl == 10) {
        levelScore = player1 ? player1->getScore() : 0;
        levelGems = gameGems;
        levelTime = 0.0f;
        freshStart = false; 
        nextScreen = 12; // Boss Complete Screen
        return;
    }

    levelScore = player1 ? player1->getScore() : 0;
    levelGems = gameGems;
    levelTime = 0.0f;

   
    currentlvl++;
    gameLevel = currentlvl;

    if (gameLevel > 10) {
        isGameOver = true;
        nextScreen = 8;
        return;
    }

    // save progress
    if (!currentUser.empty()) {
        saveProgress(currentUser, gameLevel, gameLives, gameGems, gameScore);
    }

    nextScreen = 11; // Level Complete screen
    freshStart = false; 
}

GameScreen::~GameScreen() {
    delete levelManager;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i]) delete enemies[i];
    }
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerUps[i]) delete powerUps[i];
    }
}

void GameScreen::init() {
    currentlvl = 1;
    loadLevel(currentlvl);
}

void GameScreen::loadLevel(int levelNum) {
    if (levelManager) delete levelManager;
    levelManager = new Levels(levelNum, window);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i]) { delete enemies[i]; enemies[i] = nullptr; }
    }
    enemyCount = 0;

    gMusic.stop();
    gMusic.play("assets/Sounds/music_level" + to_string(levelNum) + ".ogg");

    spawnEnemies();
}

void GameScreen::spawnEnemies() {
    int count = levelManager->getEnemyCount();
    for (int i = 0; i < count && i < MAX_ENEMIES; i++) {
        string type = levelManager->getEnemyName(i);
        float ex = levelManager->getEnemyX(i);
        float ey = levelManager->getEnemyY(i);
        Enemy* e = EnemyFactory::create(type, ex, ey, levelManager);
        if (e) enemies[enemyCount++] = e;
    }

    if (currentlvl == starLevel && !starSpawned && powerUpCount < MAX_POWERUPS) {
        powerUps[powerUpCount++] = new PowerUp(PU_STAR, screenWidth / 2.f, screenHeight / 2.f);
        starSpawned = true;
    }
}

void GameScreen::handleInputs() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::N)      nextLevel();
            if (event.key.code == sf::Keyboard::Escape) nextScreen = 4;
            if (event.key.code == sf::Keyboard::F1)     showHitBox = !showHitBox;
        }

        if (isStarMenuOpen && event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mp = sf::Mouse::getPosition(window);
            for (int i = 0; i < 3; i++) {
                float bx = 340.f + i * 220.f;
                if (mp.x >= bx && mp.x <= bx + 180 && mp.y >= 300 && mp.y <= 400) {
                    player1->applyPowerUp(starOptions[i]);
                    if (player2) player2->applyPowerUp(starOptions[i]);
                    isStarMenuOpen = false;
                }
            }
        }
    }

    if (!isPaused && !isGameOver && !isStarMenuOpen) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))      player1->moveLeft();
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) player1->moveRight();
        else                                                  player1->stopMoving();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) player1->moveUp();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) a1.startThrow();

        if (player2) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::J))      player2->moveLeft();
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) player2->moveRight();
            else                                                  player2->stopMoving();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))      player2->moveUp();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) a2.startThrow();
        }
    }
}

void GameScreen::update(float time) {
    if (pendingShopPowerUp != -1 && player1) {
        player1->applyPowerUp(pendingShopPowerUp);
        if (player2) player2->applyPowerUp(pendingShopPowerUp);
        pendingShopPowerUp = -1;
    }
    
    // Sync globals
    gameLevel = currentlvl;
    gameLives = player1->getLives();
    gameScore = player1->getScore();
    activePowerUpName = player1->getActivePowerUpName();
    activePowerUpTime = player1->getPowerUpTimeRemaining();

    // Health bar sync for bosses
    bossHealthPercent = 0.f;
    for (int i = 0; i < enemyCount; i++) {
        if (currentlvl == 5) {
            Mogera* m = dynamic_cast<Mogera*>(enemies[i]);
            if (m && m->getIsAlive()) {
                bossHealthPercent = (float)m->getHealth() / m->getMaxHealth();
                break;
            }
        }
        else if (currentlvl == 10) {
            Gamakichi* g = dynamic_cast<Gamakichi*>(enemies[i]);
            if (g && g->getIsAlive()) {
                bossHealthPercent = (float)g->getHealth() / g->getMaxHealth();
                break;
            }
        }
    }

    if (isStarMenuOpen) return;
    if (isPaused)       return;
    if (isGameOver)     return;

    // Bonus Phase 
    if (isBonusPhase) {
        bonusTimer -= time;
        if (bonusTimer <= 0) {
            isBonusPhase = false;
            nextLevel();
            return;
        }
        // Rain of cash bundles
        if (rand() % 40 == 0 && powerUpCount < MAX_POWERUPS) {
            powerUps[powerUpCount++] = new PowerUp(PU_CASH, (float)(100 + rand() % 1000), -50.0f);
        }
    }

    
    bool p1Dead = (player1->getLives() <= 0);
    bool p2Dead = (!player2 || player2->getLives() <= 0);
    if (p1Dead && p2Dead) {
        isGameOver = true;
        nextScreen = 8;
        return;
    }

    if (!p1Dead) {
        player1->update(time);
        player1->checkCollision(*levelManager);
    }
    if (player2 && !p2Dead) {
        player2->update(time);
        player2->checkCollision(*levelManager);
    }

    a1.throwSnowball(time);
    a2.throwSnowball(time);

    for (int i = 0; i < enemyCount; i++) {
        if (!enemies[i] || !enemies[i]->getIsAlive()) continue;

        // Boss Targeting
        Mogera* m = dynamic_cast<Mogera*>(enemies[i]);
        if (m) {
            m->playerX = player1->getX();
            m->playerY = player1->getY();
        }

        Gamakichi* g = dynamic_cast<Gamakichi*>(enemies[i]);
        if (g) {
            g->playerX = player1->getX();
            g->playerY = player1->getY();
            g->updateRockets(time);
        }

        enemies[i]->update(time);
    }

    for (int i = 0; i < powerUpCount; i++) {
        if (powerUps[i]) powerUps[i]->update(time);
    }

    checkCollision();
    checkDeadEnemies();
    checkLevelCleared();
}

void GameScreen::checkCollision() {
    for (int e = 0; e < enemyCount; e++) {
        if (!enemies[e] || !enemies[e]->getIsAlive()) continue;

        // --- MOGERA ---
        Mogera* mogera = dynamic_cast<Mogera*>(enemies[e]);
        if (mogera) {
            // Children collisions
            for (int c = 0; c < mogera->childCount; c++) {
                MogeraChild* ch = mogera->children[c];
                if (!ch || !ch->getIsAlive()) continue;

                // Snowball vs child
                if (a1.isActive() && a1.intersects(ch->getHBX(), ch->getHBY(), ch->getHBW(), ch->getHBH())) {
                    ch->kill(); player1->addScore(50); a1.setActive(false);
                }
                if (a2.isActive() && a2.intersects(ch->getHBX(), ch->getHBY(), ch->getHBW(), ch->getHBH())) {
                    ch->kill(); if (player2) player2->addScore(50); a2.setActive(false);
                }

                // Player vs child
                if (player1->getHitBoxBounds().intersects(sf::FloatRect(ch->getHBX(), ch->getHBY(), ch->getHBW(), ch->getHBH()))) {
                    player1->loseLife();
                }
                if (player2 && player2->getHitBoxBounds().intersects(sf::FloatRect(ch->getHBX(), ch->getHBY(), ch->getHBW(), ch->getHBH()))) {
                    player2->loseLife();
                }
            }

            if (a1.isActive() && a1.intersects(mogera->getHBX(), mogera->getHBY(), mogera->getHBW(), mogera->getHBH())) {
                mogera->takeAttack(); a1.setActive(false);
            }
            if (a2.isActive() && a2.intersects(mogera->getHBX(), mogera->getHBY(), mogera->getHBW(), mogera->getHBH())) {
                mogera->takeAttack(); a2.setActive(false);
            }

            if (player1->getHitBoxBounds().intersects(sf::FloatRect(mogera->getHBX(), mogera->getHBY(), mogera->getHBW(), mogera->getHBH()))) {
                player1->loseLife();
            }
            if (player2 && player2->getHitBoxBounds().intersects(sf::FloatRect(mogera->getHBX(), mogera->getHBY(), mogera->getHBW(), mogera->getHBH()))) {
                player2->loseLife();
            }
            continue;
        }

        // --- GAMAKICHI -------------
        Gamakichi* gama = dynamic_cast<Gamakichi*>(enemies[e]);
        if (gama) {
            if (a1.isActive() && a1.intersects(gama->getHBX(), gama->getHBY(), gama->getHBW(), gama->getHBH())) {
                gama->takeAttack(); a1.setActive(false);
            }
            if (a2.isActive() && a2.intersects(gama->getHBX(), gama->getHBY(), gama->getHBW(), gama->getHBH())) {
                gama->takeAttack(); a2.setActive(false);
            }

            // Rockets
            for (int r = 0; r < MAX_ROCKETS; r++) {
                GamaRocket& rkt = gama->rockets[r];
                if (!rkt.alive) continue;

                sf::FloatRect p1b = player1->getHitBoxBounds();
                if (rkt.travelHits(p1b.left, p1b.top, p1b.width, p1b.height)) {
                    rkt.explode(); player1->loseLife();
                }
                else if (rkt.blastHits(p1b.left, p1b.top, p1b.width, p1b.height)) {
                    player1->loseLife();
                }

                if (player2) {
                    sf::FloatRect p2b = player2->getHitBoxBounds();
                    if (rkt.travelHits(p2b.left, p2b.top, p2b.width, p2b.height)) {
                        rkt.explode(); player2->loseLife();
                    }
                    else if (rkt.blastHits(p2b.left, p2b.top, p2b.width, p2b.height)) {
                        player2->loseLife();
                    }
                }
            }

            if (player1->getHitBoxBounds().intersects(sf::FloatRect(gama->getHBX(), gama->getHBY(), gama->getHBW(), gama->getHBH()))) {
                player1->loseLife();
            }
            if (player2 && player2->getHitBoxBounds().intersects(sf::FloatRect(gama->getHBX(), gama->getHBY(), gama->getHBW(), gama->getHBH()))) {
                player2->loseLife();
            }
            continue;
        }

        // --- NORMAL ENEMIES ---
        if (a1.isActive() && a1.intersects(enemies[e]->getHBX(), enemies[e]->getHBY(), enemies[e]->getHBW(), enemies[e]->getHBH())) {
            if (player1->canInstantEncase()) enemies[e]->encaseEnemy();
            else enemies[e]->takeAttack();
            a1.setActive(false);
        }
        if (a2.isActive() && a2.intersects(enemies[e]->getHBX(), enemies[e]->getHBY(), enemies[e]->getHBW(), enemies[e]->getHBH())) {
            if (player2 && player2->canInstantEncase()) enemies[e]->encaseEnemy();
            else enemies[e]->takeAttack();
            a2.setActive(false);
        }

        if (!enemies[e]->getIsEncased()) {
            if (player1->getHitBoxBounds().intersects(sf::FloatRect(enemies[e]->getHBX(), enemies[e]->getHBY(), enemies[e]->getHBW(), enemies[e]->getHBH()))) {
                player1->loseLife();
            }
            if (player2 && player2->getHitBoxBounds().intersects(sf::FloatRect(enemies[e]->getHBX(), enemies[e]->getHBY(), enemies[e]->getHBW(), enemies[e]->getHBH()))) {
                player2->loseLife();
            }
        }
        else {
            // Kick ball
            if (enemies[e]->getvelocityx() == 0) {
                if (player1->getHitBoxBounds().intersects(sf::FloatRect(enemies[e]->getHBX(), enemies[e]->getHBY(), enemies[e]->getHBW(), enemies[e]->getHBH()))) {
                    enemies[e]->rollOnn(player1->getisFacingRight());
                    player1->addScore(50);
                }
                if (player2 && player2->getHitBoxBounds().intersects(sf::FloatRect(enemies[e]->getHBX(), enemies[e]->getHBY(), enemies[e]->getHBW(), enemies[e]->getHBH()))) {
                    enemies[e]->rollOnn(player2->getisFacingRight());
                    player2->addScore(50);
                }
            }
            else {
                // Rolling kills others
                for (int o = 0; o < enemyCount; o++) {
                    if (o == e || !enemies[o] || !enemies[o]->getIsAlive() || enemies[o]->getIsEncased()) continue;
                    if (sf::FloatRect(enemies[e]->getHBX(), enemies[e]->getHBY(), enemies[e]->getHBW(), enemies[e]->getHBH()).intersects(
                        sf::FloatRect(enemies[o]->getHBX(), enemies[o]->getHBY(), enemies[o]->getHBW(), enemies[o]->getHBH()))) {
                        enemies[o]->kill();
                        player1->addScore(200);
                    }
                }
            }
        }
    }

    // Power-ups
    for (int i = 0; i < powerUpCount; i++) {
        if (!powerUps[i] || !powerUps[i]->getIsAlive()) continue;
        if (player1->getHitBoxBounds().intersects(powerUps[i]->getHitBoxBounds())) {
            int type = powerUps[i]->getType();
            if (type == PU_STAR) {
                isStarMenuOpen = true;
                starOptions[0] = rand() % 4;
                starOptions[1] = (starOptions[0] + 1) % 4;
                starOptions[2] = (starOptions[0] + 2) % 4;
            }
            else if (type == 6) { // Gem
                gameGems++; player1->addScore(50);
            }
            else {
                player1->applyPowerUp(type);
            }
            powerUps[i]->setAlive(false);

           
            gameScore = player1->getScore();
        }
    }
}

void GameScreen::checkDeadEnemies() {
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i] && !enemies[i]->getIsAlive()) {
            // Drop powerup
            if (powerUpCount < MAX_POWERUPS) {
                int r = rand() % 100;
                if (r < 30) powerUps[powerUpCount++] = new PowerUp(rand() % 6, enemies[i]->getX(), enemies[i]->getY());
                else if (r < 70) powerUps[powerUpCount++] = new PowerUp(6, enemies[i]->getX(), enemies[i]->getY());
            }
            delete enemies[i];
            enemies[i] = enemies[enemyCount - 1];
            enemies[enemyCount - 1] = nullptr;
            enemyCount--; i--;
        }
    }
    // Cleanup powerups
    for (int i = 0; i < powerUpCount; i++) {
        if (powerUps[i] && !powerUps[i]->getIsAlive()) {
            delete powerUps[i];
            powerUps[i] = powerUps[powerUpCount - 1];
            powerUps[powerUpCount - 1] = nullptr;
            powerUpCount--; i--;
        }
    }
}

void GameScreen::checkLevelCleared() {
    if (enemyCount == 0 && !isBonusPhase) {
        nextLevel();
    }
}

void GameScreen::pause() { isPaused = true; }
void GameScreen::resume() { isPaused = false; clock.restart(); }

void GameScreen::render() {
    window.clear(sf::Color(10, 15, 40));
    if (levelManager) levelManager->draw(window);

    for (int i = 0; i < enemyCount; i++) if (enemies[i]) enemies[i]->draw(window);
    for (int i = 0; i < powerUpCount; i++) if (powerUps[i]) powerUps[i]->draw(window);

    player1->draw(window);
    if (player2) player2->draw(window);

    a1.draw(window);
    a2.draw(window);

    if (showHitBox) renderHitBoxes();
    if (globalHud)  globalHud->draw();

    sf::Font f;
    f.loadFromFile("SuperShiny-0v0rG.ttf");

    if (isStarMenuOpen) {
        sf::RectangleShape ov(sf::Vector2f(screenWidth, screenHeight));
        ov.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(ov);
        drawText(window, f, "STAR BONUS! PICK ONE", screenWidth / 2.f, 200.f, 36, sf::Color::Yellow, true);
        string names[] = { "Speed Boost", "Snow Power", "Distance", "Balloon" };
        for (int i = 0; i < 3; i++) {
            float bx = 340.f + i * 220.f;
            bool hov = isMouseOver(window, bx, 300, 180, 100);
            drawRoundButton(window, bx, 300, 180, 100, sf::Color(50, 50, 100), hov);
            drawText(window, f, names[starOptions[i]], bx + 90, 350, 20, sf::Color::White, true);
        }
    }

    // Gamakichi health bar
    for (int i = 0; i < enemyCount; i++) {
        Gamakichi* g = dynamic_cast<Gamakichi*>(enemies[i]);
        if (g && g->getIsAlive()) {
            float bw = 400.f, bh = 22.f;
            float bx = screenWidth / 2.f - bw / 2.f, by = 48.f;
            float p = (float)g->getHealth() / g->getMaxHealth();

            sf::RectangleShape rbg(sf::Vector2f(bw, bh));
            rbg.setPosition(bx, by); rbg.setFillColor(sf::Color(80, 0, 0)); window.draw(rbg);

            sf::Color c = (p > 0.66f) ? sf::Color::Green : (p > 0.33f ? sf::Color::Yellow : sf::Color::Red);
            sf::RectangleShape rfl(sf::Vector2f(bw * p, bh));
            rfl.setPosition(bx, by); rfl.setFillColor(c); window.draw(rfl);

            drawText(window, f, "GAMAKICHI", screenWidth / 2.f, 28, 20, sf::Color::Magenta, true);
            break;
        }
    }



}

void GameScreen::renderHitBoxes() {
    for (int i = 0; i < enemyCount; i++) if (enemies[i]) enemies[i]->drawHitBox(window);
    a1.drawHitBox(window);
    a2.drawHitBox(window);
}