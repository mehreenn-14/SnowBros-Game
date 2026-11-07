#include "screen.h"

using namespace std;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

HudScreen::HudScreen(sf::RenderWindow& win) : Screen(win)
{
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");
    if (itemsTex.loadFromFile("assets/Images/Player_Blue.png")) {
        lifeSprite.setTexture(itemsTex);
        lifeSprite.setTextureRect(sf::IntRect(592, 248, 38, 46));
        lifeSprite.setScale(0.8f, 0.8f);
    }
}
void HudScreen::onEnter()
{
    isActive = true;
    nextScreen = 7;
}
void HudScreen::handleInput()
{
    sf::Event e;
    while (window.pollEvent(e))
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) {
            nextScreen = 4;
        }
        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Space) {
            nextScreen = 8;
        }
    }
}
void HudScreen::update(float time) {}
void HudScreen::draw()
{
    

    // --- TOP LEFT SCORES AND LIVES---
    drawText(window, font, "SCORE: " + intToString(gameScore), 20, 20, 24, sf::Color::White, false);

    // Draw life icons
    for (int i = 0; i < gameLives; i++) {
        lifeSprite.setPosition(static_cast<float>(20 + i * 35), 55.f);
        window.draw(lifeSprite);
    }

    // --- TOP CENTER: LEVEL AND BOSS BAR---
    drawText(window, font, "LEVEL " + intToString(gameLevel), 640, 20, 28, sf::Color::Cyan, true);

    if (bossHealthPercent > 0.0f) {
        sf::RectangleShape barBg(sf::Vector2f(200, 15));
        barBg.setFillColor(sf::Color(50, 0, 0));
        barBg.setPosition(640 - 100, 55);

        sf::RectangleShape barFg(sf::Vector2f(200 * bossHealthPercent, 15));
        barFg.setFillColor(sf::Color::Red);
        barFg.setPosition(640 - 100, 55);

        window.draw(barBg);
        window.draw(barFg);
    }

    // --- RIGHT Gems & Player Names ---
    drawText(window, font, "GEMS: " + intToString(gameGems), 1130, 20, 24, sf::Color::Yellow, false);

    if (!currentUser.empty()) {
        if (!isMultiplayer) {
            // Single player: beneath gems
            drawText(window, font, "PLAYER: " + currentUser, 1130, 55, 20, sf::Color::White, false);
        }
        else {
            // Multiplayer: stacked beneath gems
            drawText(window, font, "P1: " + currentUser, 1130, 55, 18, sf::Color::White, false);
            drawText(window, font, "P2: PLAYER 2", 1130, 80, 18, sf::Color::White, false);
        }
    }

    // --- BOTTOM POWER UP---
    if (activePowerUpName != "None") {
        std::string status = activePowerUpName;
        if (activePowerUpTime > 0) status += " (" + intToString((int)activePowerUpTime) + "s)";

        sf::RectangleShape puBar(sf::Vector2f(1280, 30));
        puBar.setFillColor(sf::Color(0, 0, 0, 150));
        puBar.setPosition(0, 660);
        window.draw(puBar);

        drawText(window, font, status, 640, 675, 20, sf::Color::Cyan, true);
    }
}