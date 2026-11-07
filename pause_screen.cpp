#include "screen.h"

using namespace std;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

PauseScreen::PauseScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");
}
void PauseScreen::onEnter() 
{ 
    isActive = true; 
    nextScreen = 4; 
}
void PauseScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            if (isMouseOver(window, 540, 240, 200, 44)) nextScreen = 7;
            if (isMouseOver(window, 540, 310, 200, 44)) { shopReturnScreen = 4; nextScreen = 6; }
            if (isMouseOver(window, 540, 380, 200, 44)) {
                
                saveProgress(currentUser, gameLevel, gameLives, gameGems, gameScore);
            }
            if (isMouseOver(window, 540, 450, 200, 44)) {
                currentUser = ""; // Logout
                nextScreen = 0;
            }
            if (isMouseOver(window, 540, 520, 200, 44)) nextScreen = 1;
        }
    }
}
void PauseScreen::update(float time) {}
void PauseScreen::draw() 
{
    drawSnowBrosBackground(window);
    
    sf::RectangleShape overlay(sf::Vector2f(1280, 720));
    overlay.setFillColor(sf::Color(255, 255, 255, 180));
    window.draw(overlay);
    
    drawPanel(window, 490, 150, 300, 450, sf::Color::White, BTN_COLOR);
    drawText(window, font, "PAUSED", 640, 190, 36, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 240, 200, 44, BTN_COLOR_OK, isMouseOver(window, 540, 240, 200, 44));
    drawText(window, font, "Resume", 640, 262, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 310, 200, 44, BTN_COLOR, isMouseOver(window, 540, 310, 200, 44));
    drawText(window, font, "Open Shop", 640, 332, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 380, 200, 44, BTN_COLOR, isMouseOver(window, 540, 380, 200, 44));
    drawText(window, font, "Save Game", 640, 402, 20, TEXT_NAVY, true);

    drawRoundButton(window, 540, 450, 200, 44, sf::Color(200,200,200), isMouseOver(window, 540, 450, 200, 44));
    drawText(window, font, "Logout", 640, 472, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 520, 200, 44, BTN_COLOR_DANGER, isMouseOver(window, 540, 520, 200, 44));
    drawText(window, font, "Exit to Menu", 640, 542, 20, TEXT_NAVY, true);
}
