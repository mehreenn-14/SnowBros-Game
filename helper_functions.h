#pragma once
#include <string>
#include <SFML/Graphics.hpp>

std::string getCurrentDate();
void drawUser(sf::RenderWindow& win, sf::Font& font);
bool isMouseOver(sf::RenderWindow& win, float x, float y, float w, float h);
std::string intToString(int value);

extern std::string currentUser;
extern bool isMultiplayer;
extern int shopReturnScreen;
extern int selectedCharP1;
extern int selectedCharP2;
extern int gameLevel;
extern int gameLives;
extern int gameGems;
extern int gameScore;
extern std::string activePowerUpName;
extern float activePowerUpTime;
extern float bossHealthPercent;
extern int levelScore;
extern int levelGems;
extern float levelTime;
extern int pendingShopPowerUp;  // -1 = none

class HudScreen;
extern HudScreen* globalHud;