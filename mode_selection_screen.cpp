#include "screen.h"

using namespace std;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

ModeSelectionScreen::ModeSelectionScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");
}
void ModeSelectionScreen::onEnter() { isActive = true; nextScreen = 9; }
void ModeSelectionScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            if (isMouseOver(window, 540, 300, 200, 44)) {
                isMultiplayer = false;
                nextScreen = 5;
            }
            if (isMouseOver(window, 540, 370, 200, 44)) {
                isMultiplayer = true;
                nextScreen = 5;
            }
            if (isMouseOver(window, 540, 440, 200, 44)) nextScreen = 1;
        }
    }
}
void ModeSelectionScreen::update(float time) {}
void ModeSelectionScreen::draw() 
{
    drawSnowBrosBackground(window);
    drawUser(window, font);
    drawPanel(window, 490, 200, 300, 320, sf::Color::White, BTN_COLOR);
    drawText(window, font, "GAME MODE", 640, 250, 36, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 300, 200, 44, BTN_COLOR, isMouseOver(window, 540, 300, 200, 44));
    drawText(window, font, "Single Player", 640, 322, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 370, 200, 44, BTN_COLOR, isMouseOver(window, 540, 370, 200, 44));
    drawText(window, font, "Multiplayer", 640, 392, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 440, 200, 44, BTN_COLOR_DANGER, isMouseOver(window, 540, 440, 200, 44));
    drawText(window, font, "Back", 640, 462, 20, TEXT_NAVY, true);
}
