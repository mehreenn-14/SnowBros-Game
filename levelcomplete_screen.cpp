#include "screen.h"
#include <iomanip>
#include <sstream>

using namespace std;

extern MusicManager gMusic;
extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

LevelCompleteScreen::LevelCompleteScreen(sf::RenderWindow& win) : Screen(win)
{
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");
}

void LevelCompleteScreen::onEnter()
{
    gMusic.stop();
    gMusic.play("assets/Sounds/levelComplete.ogg");
    isActive = true;
    nextScreen = 11;   
}

void LevelCompleteScreen::handleInput()
{
    sf::Event e;
    while (window.pollEvent(e))
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed)
        {
            // Next Level button 
            if (isMouseOver(window, 440, 500, 180, 50)) {
                nextScreen = 7;
            }
            // Menu button
            if (isMouseOver(window, 660, 500, 180, 50)) {
                nextScreen = 1;
            }
        }
    }
}

void LevelCompleteScreen::update(float time) {}

void LevelCompleteScreen::draw()
{
    drawSnowBrosBackground(window);
    drawUser(window, font);

    sf::RectangleShape overlay(sf::Vector2f(1280, 720));
    overlay.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(overlay);

    drawPanel(window, 340, 120, 600, 480, sf::Color::White, BTN_COLOR);
    drawText(window, font, "LEVEL COMPLETE!", 640, 180, 48, BTN_COLOR_OK, true);

    drawText(window, font, "Score: " + intToString(levelScore), 640, 280, 24, TEXT_NAVY, true);
    drawText(window, font, "Gems Collected: " + intToString(levelGems), 640, 330, 24, TEXT_NAVY, true);

    int mins = static_cast<int>(levelTime) / 60;
    int secs = static_cast<int>(levelTime) % 60;
    stringstream ss;
    ss << "Time: " << mins << ":" << (secs < 10 ? "0" : "") << secs;
    drawText(window, font, ss.str(), 640, 380, 24, TEXT_NAVY, true);

    bool hovNext = isMouseOver(window, 440, 500, 180, 50);
    drawRoundButton(window, 440, 500, 180, 50, BTN_COLOR_OK, hovNext);
    drawText(window, font, "NEXT", 530, 525, 20, TEXT_NAVY, true);

    bool hovMenu = isMouseOver(window, 660, 500, 180, 50);
    drawRoundButton(window, 660, 500, 180, 50, BTN_COLOR_DANGER, hovMenu);
    drawText(window, font, "MENU", 750, 525, 20, TEXT_NAVY, true);
}