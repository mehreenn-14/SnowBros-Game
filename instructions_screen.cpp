#include "screen.h"

using namespace std;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

InstructionsScreen::InstructionsScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");
}
void InstructionsScreen::onEnter() 
{ 
    isActive = true; 
    nextScreen = 3; 
}
void InstructionsScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            if (isMouseOver(window, 540, 560, 200, 44)) 
            nextScreen = 1;
        }
    }
}
void InstructionsScreen::update(float time) {}
void InstructionsScreen::draw() 
{
    drawSnowBrosBackground(window);
    drawUser(window, font);
    drawPanel(window, 290, 110, 700, 440, sf::Color::White, BTN_COLOR);
    drawText(window, font, "HOW TO PLAY", 640, 150, 36, TEXT_NAVY, true);
    
    drawText(window, font, "- Throw snowballs to encase enemies - walk into them to roll", 320, 210, 17, TEXT_NAVY, false);
    drawText(window, font, "- Roll an encased enemy into others for chain kills + score bonus", 320, 250, 17, TEXT_NAVY, false);
    drawText(window, font, "- Collect gems dropped by enemies to spend in the Shop", 320, 290, 17, TEXT_NAVY, false);
    drawText(window, font, "- Clear all enemies on screen to advance to the next level", 320, 330, 17, TEXT_NAVY, false);
    drawText(window, font, "- Reach Level 5 and Level 10 to face bosses Mogera and Gamakichi", 320, 370, 17, TEXT_NAVY, false);
    drawText(window, font, "- Lives reach zero = Game Over. Pick up life icons to survive longer", 320, 410, 17, TEXT_NAVY, false);
    
    bool hov = isMouseOver(window, 540, 560, 200, 44);
    drawRoundButton(window, 540, 560, 200, 44, BTN_COLOR, hov);
    drawText(window, font, "Back", 640, 582, 20, TEXT_NAVY, true);
}
