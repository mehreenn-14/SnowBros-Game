#include "screen.h"
#include <SFML/Audio.hpp>

using namespace std;
extern MusicManager gMusic;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

GameOverScreen::GameOverScreen(sf::RenderWindow& win) : Screen(win)
{
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");
}
void GameOverScreen::onEnter()
{
    gMusic.stop();
   
    if (gameLevel > 10)
        gMusic.play("assets/Sounds/themeSong.ogg");
    else
        gMusic.play("assets/Sounds/gameOver.ogg");

    isActive = true;
    nextScreen = 8;

    // WritING score 
    ofstream fileOut("leaderboard.txt", ios::app);
    fileOut << currentUser << " " << gameScore << " " << gameLevel << " " << "2026-05-01" << "\n";
    fileOut.close();
}
void GameOverScreen::handleInput()
{
    sf::Event e;
    while (window.pollEvent(e))
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed)
        {
            // Victory screen button 
            if (gameLevel > 10 && isMouseOver(window, 490, 470, 300, 50))
                nextScreen = 1;
            // Game Over screen
            if (gameLevel <= 10 && isMouseOver(window, 540, 460, 200, 44))
                nextScreen = 1;
        }
    }
}
void GameOverScreen::update(float time) {

}
void GameOverScreen::draw()
{
    drawSnowBrosBackground(window);
    drawUser(window, font);

    if (gameLevel > 10) {
        //------------------------------------ VICTORY SCREEN---------------------------------------------
        // Gold shimmer panel
        sf::RectangleShape panel(sf::Vector2f(720, 340));
        panel.setPosition(280, 160);
        panel.setFillColor(sf::Color(30, 20, 0, 200));
        panel.setOutlineColor(sf::Color(255, 215, 0));
        panel.setOutlineThickness(4.f);
        window.draw(panel);

        drawText(window, font, "YOU WIN!", 640, 210, 72, sf::Color(255, 215, 0), true);
        drawText(window, font, "Congratulations!", 640, 300, 28, sf::Color::White, true);
        drawText(window, font, "Final Score: " + intToString(gameScore), 640, 360, 26, sf::Color(255, 215, 0), true);
        drawText(window, font, "All 10 Levels Cleared!", 640, 410, 22, sf::Color(200, 255, 200), true);

        bool hovered = isMouseOver(window, 490, 470, 300, 50);
        drawRoundButton(window, 490, 470, 300, 50, BTN_COLOR_OK, hovered);
        drawText(window, font, "Back to Main Menu", 640, 495, 20, TEXT_NAVY, true);
    }
    else {
        // ------------------------ GAME OVER SCREEN --------------------------------------------
        drawText(window, font, "GAME OVER", 640, 260, 64, BTN_COLOR_DANGER, true);
        drawText(window, font, "Final Score: " + intToString(gameScore), 640, 360, 28, sf::Color::Yellow, true);
        drawText(window, font, "Level Reached: " + intToString(gameLevel), 640, 410, 22, sf::Color::Yellow, true);
        bool hovered = isMouseOver(window, 540, 460, 200, 44);
        drawRoundButton(window, 540, 460, 200, 44, BTN_COLOR, hovered);
        drawText(window, font, "Main Menu", 640, 482, 20, sf::Color(50, 50, 50), true);
    }
}