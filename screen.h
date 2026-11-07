#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

using namespace std;

const int VAL_OK = 0;
const int VAL_USERNAME_EMPTY = 1;
const int VAL_USERNAME_SHORT = 2;
const int VAL_USERNAME_SPACES = 3;
const int VAL_PASSWORD_EMPTY = 4;
const int VAL_PASSWORD_SHORT = 5;
const int VAL_PASSWORD_SPACES = 6;
const int VAL_PASSWORDS_MISMATCH = 7;
const int VAL_DUPLICATE_USER = 8;
const int VAL_LOGIN_FAILED = 9;
//-----------------------------FOR MUSIC-------------------
class MusicManager {
private:
    sf::Music music;
    string    currentTrack;
    bool isRunning;

public:
    void play(const string& filename) {
        if (filename == "assets/Sounds/themeSong.ogg") isRunning = true;
        else isRunning = false;
        if (currentTrack == filename) return;

        music.stop();
        if (music.openFromFile(filename)) {
            music.setLoop(true);
            music.setVolume(40.f);
            music.play();
            currentTrack = filename;
        }
    }
    bool getIsRunning() { return isRunning; }
    void stop() {
        music.stop();
        currentTrack = "";
    }

    void pause() { music.pause(); }
    void resume() { music.play(); }

    void setVolume(float v) { music.setVolume(v); }
};

extern MusicManager gMusic;
//-----------------------------------------------------------------------------------------
class AuthManager {
private:
    string hashPassword(const string& password);
public:
    bool loginUser(const string& username, const string& password);
    int registerUser(const string& username, const string& password, const string& confirmPassword);
};

class HudScreen; // Forward Declaration

void drawRoundButton(sf::RenderWindow& win, float x, float y, float w, float h, sf::Color color, bool hovered);
void drawText(sf::RenderWindow& win, const sf::Font& font, const string& str, float x, float y, unsigned int size, sf::Color color, bool centered);
void drawPanel(sf::RenderWindow& win, float x, float y, float w, float h, sf::Color fill, sf::Color outline);
bool isMouseOver(sf::RenderWindow& win, float x, float y, float w, float h);
string intToString(int value);
void drawSnowBrosBackground(sf::RenderWindow& win);
void drawUser(sf::RenderWindow& win, sf::Font& font);
void saveProgress(const string& username, int level, int lives, int gems, int highScore);
bool loadProgress(const string& username, int& level, int& lives, int& gems, int& highScore);

extern string currentUser;
extern bool isMultiplayer;
extern int shopReturnScreen;
extern int gameLevel;
extern int gameLives;
extern int gameGems;
extern int gameScore;
extern string activePowerUpName;
extern float  activePowerUpTime;
extern float  bossHealthPercent;
extern int levelScore;
extern int levelGems;
extern float levelTime;
extern int pendingShopPowerUp;

extern int selectedCharP1;
extern int selectedCharP2;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

class Screen {
protected:
    sf::RenderWindow& window;
    bool isActive;

public:
    Screen(sf::RenderWindow& win) : window(win), isActive(false) {}
    virtual ~Screen() = default;

    virtual void draw() = 0;
    virtual void handleInput() = 0;
    virtual void update(float time) = 0;
    virtual int getNextScreen() const = 0;

    virtual void onEnter() { isActive = true; }
    virtual void onExit() { isActive = false; }
};
//--------------------------LOGIN SCREEN--------------------------------------
class LoginScreen : public Screen {
    sf::Font font;
    string username;
    string password;
    string confirmPassword;
    string usernameError;
    string passwordError;
    string confirmError;
    string loginMessage;
    AuthManager authManager;
    int focus;        // 0=username, 1=password, 2=confirmPassword
    int activeTab;    // 0=Login, 1=Register
    int nextScreen;

    string getValidationMessage(int result);

public:
    LoginScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};
//---------------------------MAIN MENU SCREEN------------------------------
class MainMenuScreen : public Screen {
    sf::Font font;
    int nextScreen;
    bool hasSavedProgress;
public:
    MainMenuScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};
//--------------------------MODE SELECTION SCREEN--------------------------------
class ModeSelectionScreen : public Screen {
    sf::Font font;
    int nextScreen;
public:
    ModeSelectionScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};
//-----------------------------LEADERBOARD SCREEN-------------------------
class LeaderboardScreen : public Screen {
    sf::Font font;
    int nextScreen;
    string entries[10];
    string entryUsers[10];
    int entryCount;
public:
    LeaderboardScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};
//--------------------------------INSTRUCTION SCREEN---------------------
class InstructionsScreen : public Screen {
    sf::Font font;
    int nextScreen;
public:
    InstructionsScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};
//------------------------PAUSE SCREEN---------------------
class PauseScreen : public Screen {
    sf::Font font;
    int nextScreen;
    bool showSavedMsg;
public:
    PauseScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};
//--------------------------------CHARACHTER SELECT------------------------
class CharacterSelectScreen : public Screen {
    sf::Font font;
    sf::Texture texNick, texRed, texGreen, texYellow, texBlue;
    sf::Sprite spNick, spRed, spGreen, spYellow, spBlue;

    // To track which 3 options are currently visible
    int visibleOptions[3];

    bool pickingPlayer2;
    int nextScreen;
public:
    CharacterSelectScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};
//--------------------------------SHOP-----------------------
class ShopScreen : public Screen {
    sf::Font font;
    int nextScreen;
    int gems;
    string shopMessage;
    int pendingItemIndex;
    int pendingPurchasedPowerUp;  // -1 = none, 1-4 = powerup type bought
public:
    int pollPurchasedPowerUp() {
        int p = pendingPurchasedPowerUp;
        pendingPurchasedPowerUp = -1;
        return p;
    }
    ShopScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};
//--------------hud--------------------------------
class HudScreen : public Screen {
    sf::Font font;
    sf::Texture itemsTex;
    sf::Sprite lifeSprite;
    int nextScreen;
public:
    HudScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};
//-----------------------game over screen--------------------------------------
class GameOverScreen : public Screen {
    sf::Font font;
    int nextScreen;
    sf::Texture gameOverSnapshot;
    sf::Sprite  gameOverBg;
public:
    GameOverScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};
//-----------------------level complete screen-------------------------------------
class LevelCompleteScreen : public Screen {
    sf::Font font;
    int nextScreen;
public:
    LevelCompleteScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};



//-----------------------boss level screen------------------------------------
class BossLevelCompleteScreen : public Screen {
    sf::Font font;
    sf::Texture itemsTex;
    sf::Sprite gemSprite;
    bool gemsAwarded;
    int nextScreen;
public:
    BossLevelCompleteScreen(sf::RenderWindow& win);
    void draw() override;
    void handleInput() override;
    void update(float time) override;
    int getNextScreen() const override { return nextScreen; }
    void onEnter() override;
};

extern class HudScreen* globalHud;