#include "screen.h"

using namespace std;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

LeaderboardScreen::LeaderboardScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");
    entryCount = 0;
}
void LeaderboardScreen::onEnter() 
{ 
    isActive = true; 
    nextScreen = 2; 
    entryCount = 0;
    
    // Read all entries 
    string tempUsers[100];
    int tempScores[100];
    string tempLevels[100];
    int totalEntries = 0;
    
    ifstream file("leaderboard.txt");
    if (file.is_open()) 
    {
        string user_id, score, level_reached, recorded_at;
        while (file >> user_id >> score >> level_reached >> recorded_at && totalEntries < 100) 
        {
            tempUsers[totalEntries] = user_id;
            int scoreVal = 0;
            for (size_t i = 0; i < score.length(); i++)
            {
                if (score[i] >= '0' && score[i] <= '9')
                    scoreVal = scoreVal * 10 + (score[i] - '0');
            }
            tempScores[totalEntries] = scoreVal;
            tempLevels[totalEntries] = level_reached;
            totalEntries++;
        }
        file.close();
    }
    
    // Sort by score 
    for (int i = 0; i < totalEntries - 1; i++)
    {
        for (int j = 0; j < totalEntries - i - 1; j++)
        {
            if (tempScores[j] < tempScores[j + 1])
            {
                //SCORES
                int ts = tempScores[j];
                tempScores[j] = tempScores[j + 1];
                tempScores[j + 1] = ts;
                // USERS
                string tu = tempUsers[j];
                tempUsers[j] = tempUsers[j + 1];
                tempUsers[j + 1] = tu;
                // LEVELS
                string tl = tempLevels[j];
                tempLevels[j] = tempLevels[j + 1];
                tempLevels[j + 1] = tl;
            }
        }
    }
    
    for (int i = 0; i < totalEntries && i < 10; i++)
    {
        entries[i] = tempUsers[i] + " - Score: " + intToString(tempScores[i]) + " Level: " + tempLevels[i];
        entryUsers[i] = tempUsers[i];
        entryCount++;
    }
}
void LeaderboardScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            if (isMouseOver(window, 540, 560, 200, 44)) nextScreen = 1;
        }
    }
}
void LeaderboardScreen::update(float time) {}
void LeaderboardScreen::draw() 
{
    drawSnowBrosBackground(window);
    drawUser(window, font);
    drawPanel(window, 390, 110, 500, 420, sf::Color::White, BTN_COLOR);
    drawText(window, font, "LEADERBOARD", 640, 150, 36, TEXT_NAVY, true);
    
    if (entryCount == 0) 
    {
        drawText(window, font, "No entries yet!", 640, 250, 24, TEXT_NAVY, true);
    } 
    else 
    {
        for (int i = 0; i < entryCount && i < 8; ++i) 
        {
            // Highlight current user's row
            bool isCurrentUser = (entryUsers[i] == currentUser);
            sf::Color rowColor = isCurrentUser ? sf::Color(255, 215, 0) : TEXT_NAVY;
            
            if (isCurrentUser)
            {
                sf::RectangleShape highlight(sf::Vector2f(460, 32));
                highlight.setPosition(410, 205 + i * 38);
                highlight.setFillColor(sf::Color(255, 255, 200, 100));
                window.draw(highlight);
            }
            
            drawText(window, font, intToString(i + 1) + ". " + entries[i], 420, 210 + i * 38, 18, rowColor, false);
        }
    }
    
    bool hov = isMouseOver(window, 540, 560, 200, 44);
    drawRoundButton(window, 540, 560, 200, 44, BTN_COLOR, hov);
    drawText(window, font, "Back", 640, 582, 20, TEXT_NAVY, true);
}
