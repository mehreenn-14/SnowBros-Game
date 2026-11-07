#include "screen.h"

using namespace std;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;
extern MusicManager gMusic;

MainMenuScreen::MainMenuScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");
}
void MainMenuScreen::onEnter() 
{ 
    if (!gMusic.getIsRunning())
    {
        gMusic.stop();
        gMusic.play("assets/Sounds/themeSong.ogg");

    }
   
    isActive = true; 
    nextScreen = 1;
    
    int l, li, g, hs;
    hasSavedProgress = loadProgress(currentUser, l, li, g, hs);
}
void MainMenuScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            if (isMouseOver(window, 540, 240, 200, 44)) { 
                // Reset game state for New Game
                gameLevel = 1; 
                gameLives = 3; 
                gameScore = 0; 
                gameGems = 0;
                nextScreen = 9; 
            }
            if (hasSavedProgress && isMouseOver(window, 540, 300, 200, 44)) 
            {
                // CONTINUE LOGIC
                int level, lives, gems, highScore;
                if (loadProgress(currentUser, level, lives, gems, highScore))
                {
                    gameLevel = level;
                    gameLives = lives;
                    gameGems = gems;
                    gameScore = highScore;
                    nextScreen = 7; // GAME
                }
            }
            float offset = hasSavedProgress ? 0.f : -60.f;
            if (isMouseOver(window, 540, 360 + offset, 200, 44)) nextScreen = 2;
            if (isMouseOver(window, 540, 420 + offset, 200, 44)) { shopReturnScreen = 1; nextScreen = 6; }
            if (isMouseOver(window, 540, 480 + offset, 200, 44)) nextScreen = 3;
            if (isMouseOver(window, 540, 540 + offset, 200, 44)) nextScreen = 0;
            if (isMouseOver(window, 540, 600 + offset, 200, 44)) nextScreen = 99;
        }
    }
}
void MainMenuScreen::update(float time) {}
void MainMenuScreen::draw() 
{
    drawSnowBrosBackground(window);
    drawUser(window, font);
    drawText(window, font, "MAIN MENU", 640, 160, 48, BTN_COLOR, true);
    
    int btnIndex = 0;
    float startY = 240.f;
    
    // NEW GAME
    bool hov0 = isMouseOver(window, 540, startY, 200, 44);
    drawRoundButton(window, 540, startY, 200, 44, BTN_COLOR_OK, hov0);
    drawText(window, font, "Play", 640, startY + 22, 20, TEXT_NAVY, true);
    
    // CINTINUE
    if (hasSavedProgress)
    {
        float cy = 300.f;
        bool hovC = isMouseOver(window, 540, cy, 200, 44);
        drawRoundButton(window, 540, cy, 200, 44, sf::Color(200, 255, 200), hovC);
        drawText(window, font, "Continue", 640, cy + 22, 20, TEXT_NAVY, true);
    }
    
    float offset = hasSavedProgress ? 0.f : -60.f;

    std::string labels[] = {"Leaderboard", "Shop", "Instructions", "Logout", "Quit"};

    sf::Color colors[] = {BTN_COLOR, BTN_COLOR, BTN_COLOR, sf::Color(200,200,200), BTN_COLOR_DANGER};
    
    for (int i = 0; i < 5; ++i) 
    {
        float y = 360.f + i * 60.f + offset;
        bool hov = isMouseOver(window, 540, y, 200, 44);
        drawRoundButton(window, 540, y, 200, 44, colors[i], hov);
        drawText(window, font, labels[i], 640, y + 22, 20, TEXT_NAVY, true);
    }
}
