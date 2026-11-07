#include "screen.h"

using namespace std;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

ShopScreen::ShopScreen(sf::RenderWindow& win) : Screen(win)
{
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");
}
void ShopScreen::onEnter()
{
    isActive = true;
    nextScreen = 6;
    gems = gameGems; 
    shopMessage = "";
    pendingItemIndex = -1;
    pendingPurchasedPowerUp = -1;
}
void ShopScreen::handleInput()
{
    sf::Event e;
    while (window.pollEvent(e))
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed)
        {
            if (pendingItemIndex != -1)
            {
                if (isMouseOver(window, 480, 400, 120, 44))
                {
                    int costs[] = { 0, 20, 30, 25, 35 };
                    string msgs[] = { "Purchased Extra Life!", "Purchased Speed Boost!", "Purchased Snowball Power!", "Purchased Distance Increase!", "Purchased Balloon Mode!" };

                    if (gems >= costs[pendingItemIndex]) {
                        gems -= costs[pendingItemIndex];
                        gameGems = gems; 

                        if (pendingItemIndex == 0) {
                            gameLives++;              // extra Life apply immediately
                        }
                        else {
                            pendingShopPowerUp = pendingItemIndex; // 1-4: apply in-game
                        }

                        shopMessage = msgs[pendingItemIndex];
                    }
                    else {
                        shopMessage = "Not enough gems!";
                    }
                    pendingItemIndex = -1;
                }
                else if (isMouseOver(window, 680, 400, 120, 44))
                {
                    pendingItemIndex = -1;
                    shopMessage = "Purchase cancelled.";
                }
            }
            else
            {
                if (isMouseOver(window, 540, 600, 200, 44)) nextScreen = shopReturnScreen;

                for (int i = 0; i < 5; i++) {
                    float y = 200 + i * 70;
                    if (isMouseOver(window, 300, y, 680, 50)) {
                        pendingItemIndex = i;
                        shopMessage = ""; // Clear previous message
                    }
                }
            }
        }
    }
}
void ShopScreen::update(float time) {}
void ShopScreen::draw()
{
    drawSnowBrosBackground(window);
    drawUser(window, font);
    drawPanel(window, 280, 100, 720, 480, sf::Color::White, BTN_COLOR);
    drawText(window, font, "GEMS SHOP", 640, 130, 32, TEXT_NAVY, true);
    drawText(window, font, "Gems: " + intToString(gems), 640, 170, 24, sf::Color(0, 100, 0), true);

    string items[] = { "Extra Life", "Speed Boost (30s)", "Snowball Power", "Distance Increase", "Balloon Mode (30s)" };
    int costs[] = { 0, 20, 30, 25, 35 };

    for (int i = 0; i < 5; i++)
    {
        float y = 200 + i * 70;
        bool hov = isMouseOver(window, 300, y, 680, 50);
        drawRoundButton(window, 300, y, 680, 50, sf::Color(240, 240, 240), hov);
        drawText(window, font, items[i], 320, y + 13, 20, TEXT_NAVY, false);
        drawText(window, font, intToString(costs[i]) + " Gems", 900, y + 13, 20, sf::Color(0, 100, 0), false);
    }

    drawText(window, font, shopMessage, 640, 560, 20, TEXT_NAVY, true);

    bool hovB = isMouseOver(window, 540, 600, 200, 44);
    drawRoundButton(window, 540, 600, 200, 44, BTN_COLOR_DANGER, hovB);
    drawText(window, font, "Back", 640, 622, 20, TEXT_NAVY, true);

    if (pendingItemIndex != -1)
    {
        sf::RectangleShape overlay(sf::Vector2f(1280, 720));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        drawPanel(window, 440, 300, 400, 180, sf::Color::White, BTN_COLOR);
        drawText(window, font, "Buy " + items[pendingItemIndex] + "?", 640, 340, 20, TEXT_NAVY, true);

        drawRoundButton(window, 480, 400, 120, 44, BTN_COLOR_OK, isMouseOver(window, 480, 400, 120, 44));
        drawText(window, font, "Yes", 540, 422, 20, TEXT_NAVY, true);

        drawRoundButton(window, 680, 400, 120, 44, BTN_COLOR_DANGER, isMouseOver(window, 680, 400, 120, 44));
        drawText(window, font, "No", 740, 422, 20, TEXT_NAVY, true);
    }
}