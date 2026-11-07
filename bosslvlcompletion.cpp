#include "screen.h"
#include <iomanip>
#include <sstream>

using namespace std;
extern MusicManager gMusic;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

BossLevelCompleteScreen::BossLevelCompleteScreen(sf::RenderWindow& win) : Screen(win) 
{
    nextScreen = 12;
    gemsAwarded = false;
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");
    if (itemsTex.loadFromFile("assets/Images/Items.png")) {
        gemSprite.setTexture(itemsTex);
        gemSprite.setTextureRect(sf::IntRect(419, 804, 42, 40));
        gemSprite.setScale(1.5f, 1.5f);
    }
}

void BossLevelCompleteScreen::onEnter() 
{ 
    gMusic.stop();
    gMusic.play("assets/Sounds/bossLevel");
    isActive = true; 
    nextScreen = 12; // default 
    gemsAwarded = false;
}

void BossLevelCompleteScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            // ACCEPT button
            if (isMouseOver(window, 540, 500, 200, 50)) {
                if ((gameLevel == 5 || gameLevel == 10) && !gemsAwarded) {
                    gameGems += (gameLevel == 5) ? 200 : 500;
                    gemsAwarded = true;
                    saveProgress(currentUser, gameLevel, gameLives, gameGems, gameScore);
                }
                gameLevel++;
                if (gameLevel > 10) {
                    nextScreen = 8; // Game Over / Victory
                } else {
                    nextScreen = 7; // Back to Game
                }
            }
        }
    }
}

void BossLevelCompleteScreen::update(float time) {}

void BossLevelCompleteScreen::draw() 
{
    drawSnowBrosBackground(window);
    drawUser(window, font);
    
    
    sf::RectangleShape overlay(sf::Vector2f(1280, 720));
    overlay.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(overlay);

    drawPanel(window, 340, 120, 600, 480, sf::Color::White, BTN_COLOR);
    drawText(window, font, "BOSS DEFEATED!", 640, 180, 48, BTN_COLOR_OK, true);
    
    drawText(window, font, "Score: " + intToString(levelScore), 640, 260, 24, TEXT_NAVY, true);
    drawText(window, font, "Gems Collected: " + intToString(levelGems), 640, 310, 24, TEXT_NAVY, true);
    
    // Format time as MM:SS
    int mins = static_cast<int>(levelTime) / 60;
    int secs = static_cast<int>(levelTime) % 60;
    stringstream ss;
    ss << "Time: " << mins << ":" << (secs < 10 ? "0" : "") << secs;
    drawText(window, font, ss.str(), 640, 360, 24, TEXT_NAVY, true);

    if (gameLevel == 5 || gameLevel == 10) {
        int reward = (gameLevel == 5) ? 200 : 500;
        
        gemSprite.setPosition(520, 410);
        window.draw(gemSprite);
        
        drawText(window, font, "+ " + intToString(reward), 600, 420, 32, sf::Color(255, 215, 0), false);
    }

    bool hovNext = isMouseOver(window, 540, 500, 200, 50);
    drawRoundButton(window, 540, 500, 200, 50, BTN_COLOR_OK, hovNext);
    drawText(window, font, "ACCEPT", 640, 525, 20, TEXT_NAVY, true);
}
