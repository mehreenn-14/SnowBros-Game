
#define _CRT_SECURE_NO_WARNINGS
#include "screen.h"
#include <cmath>
#include <ctime>

using namespace std;
extern int pendingShopPowerUp;
string getCurrentDate()
{
    time_t now = time(0);
    char* dt = ctime(&now);
    string s = string(dt);
    if (!s.empty() && s.back() == '\n') s.pop_back();

    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == ' ') s[i] = '_';
    }
    return s;
}

string currentUser = "";
bool isMultiplayer = false;
int shopReturnScreen = 1;

int selectedCharP1 = 0;
int selectedCharP2 = -1; //NOT SELECTED

int gameLevel = 1;
int gameLives = 10;
int gameGems = 0;
int gameScore = 0;
string activePowerUpName = "None";
float  activePowerUpTime = 0.0f;
float  bossHealthPercent = 0.0f;
int levelScore = 0;
int levelGems = 0;
int pendingShopPowerUp = -1;
float levelTime = 0.0f;
HudScreen* globalHud = nullptr;

const sf::Color BTN_COLOR(176, 224, 230);
const sf::Color BTN_COLOR_DANGER(255, 105, 180);
const sf::Color BTN_COLOR_OK(152, 251, 152);
const sf::Color TEXT_NAVY(0, 0, 128);

string intToString(int value)
{
    if (value == 0)
    {
        return "0";
    }
    string result = "";
    bool isNeg = value < 0;
    if (isNeg)
    {
        value = -value;
    }
    while (value > 0)
    {
        result = char('0' + (value % 10)) + result;
        value /= 10;
    }
    return isNeg ? "-" + result : result;
}

void drawUser(sf::RenderWindow& win, sf::Font& font)
{
    if (!currentUser.empty())
    {
        drawText(win, font, "Player: " + currentUser, 20, 20, 24, sf::Color(25, 25, 112), false);
    }
}

bool isMouseOver(sf::RenderWindow& win, float x, float y, float w, float h)
{
    sf::Vector2i m = sf::Mouse::getPosition(win);
    return (m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h);
}

string AuthManager::hashPassword(const string& password)
{
    unsigned int hash = 2166136261u;
    for (int i = 0; i < password.length(); i++)
    {
        hash ^= (unsigned char)password[i];
        hash *= 16777619u;
    }
    return intToString(int(hash));
}

bool AuthManager::loginUser(const string& username, const string& password)
{
    string hashed = hashPassword(password);
    ifstream file("users.txt");
    string u, p,d;
    while (file >> u >> p>>d)
    {
        if (u == username && p == hashed) return true;
    }
    return false;
}

int AuthManager::registerUser(const string& username, const string& password, const string& confirmPassword)
{
    // Username validation
    if (username.empty()) return VAL_USERNAME_EMPTY;
    if (username.length() < 3) return VAL_USERNAME_SHORT;
    for (size_t i = 0; i < username.length(); i++) {
        if (username[i] == ' ') return VAL_USERNAME_SPACES;
    }

    // Password validation
    if (password.empty()) return VAL_PASSWORD_EMPTY;
    if (password.length() < 6) return VAL_PASSWORD_SHORT;
    for (size_t i = 0; i < password.length(); i++) {
        if (password[i] == ' ') return VAL_PASSWORD_SPACES;
    }

    // Confirm password match
    if (password != confirmPassword) return VAL_PASSWORDS_MISMATCH;

    // Duplicate user check
    ifstream fileIn("users.txt");
    string u, p;
    while (fileIn >> u >> p)
    {
        if (u == username) return VAL_DUPLICATE_USER;
    }
    fileIn.close();

    string hashed = hashPassword(password);
    string timestamp = getCurrentDate();

    ofstream fileOut("users.txt", ios::app);
    fileOut << username << " " << hashed << " " << timestamp << "\n";
    return VAL_OK;
}

void drawRoundButton(sf::RenderWindow& win, float x, float y, float w, float h, sf::Color color, bool hovered)
{
    float radius = h / 2.f;

    sf::CircleShape shadowL(radius);
    shadowL.setPosition(x + 2, y + 4);
    shadowL.setFillColor(sf::Color(0, 0, 0, 30));
    sf::CircleShape shadowR(radius);
    shadowR.setPosition(x + w - h + 2, y + 4);
    shadowR.setFillColor(sf::Color(0, 0, 0, 30));
    sf::RectangleShape shadowMid(sf::Vector2f(w - h, h));
    shadowMid.setPosition(x + radius + 2, y + 4);
    shadowMid.setFillColor(sf::Color(0, 0, 0, 30));

    win.draw(shadowL); win.draw(shadowR); win.draw(shadowMid);

    sf::Color fill = hovered ? sf::Color(color.r > 20 ? color.r - 20 : 0, color.g > 20 ? color.g - 20 : 0, color.b > 20 ? color.b - 20 : 0) : color;

    sf::CircleShape left(radius);
    left.setPosition(x, y);
    left.setFillColor(fill);

    sf::CircleShape right(radius);
    right.setPosition(x + w - h, y);
    right.setFillColor(fill);

    sf::RectangleShape mid(sf::Vector2f(w - h, h));
    mid.setPosition(x + radius, y);
    mid.setFillColor(fill);

    win.draw(left); win.draw(right); win.draw(mid);
}

void drawPanel(sf::RenderWindow& win, float x, float y, float w, float h, sf::Color fill, sf::Color outline)
{
    float radius = 15.f;
    sf::ConvexShape rect;
    rect.setPointCount(40);

    float pi = 3.141592654f;
    int pt = 0;

    for (int i = 0; i < 10; i++) {
        float a = pi + (pi / 2.f) * ((float)i / 9.f);
        rect.setPoint(pt++, sf::Vector2f(x + radius + radius * cos(a), y + radius + radius * sin(a)));
    }
    for (int i = 0; i < 10; i++) {
        float a = 1.5f * pi + (pi / 2.f) * ((float)i / 9.f);
        rect.setPoint(pt++, sf::Vector2f(x + w - radius + radius * cos(a), y + radius + radius * sin(a)));
    }
    for (int i = 0; i < 10; i++) {
        float a = 0.f + (pi / 2.f) * ((float)i / 9.f);
        rect.setPoint(pt++, sf::Vector2f(x + w - radius + radius * cos(a), y + h - radius + radius * sin(a)));
    }
    for (int i = 0; i < 10; i++) {
        float a = pi / 2.f + (pi / 2.f) * ((float)i / 9.f);
        rect.setPoint(pt++, sf::Vector2f(x + radius + radius * cos(a), y + h - radius + radius * sin(a)));
    }

    rect.setFillColor(fill);
    rect.setOutlineColor(outline);
    rect.setOutlineThickness(3.f);

    sf::ConvexShape shadow = rect;
    shadow.setPosition(4, 4);
    shadow.setFillColor(sf::Color(0, 0, 0, 30));
    shadow.setOutlineThickness(0);

    win.draw(shadow);
    win.draw(rect);
}

void drawText(sf::RenderWindow& win, const sf::Font& font, const std::string& str, float x, float y, unsigned int size, sf::Color color, bool centered)
{
    sf::Text t(str, font, size);
    t.setFillColor(color);
    if (centered)
    {
        sf::FloatRect bounds = t.getLocalBounds();
        t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    }
    t.setPosition(x, y);
    win.draw(t);
}



void drawSnowBrosBackground(sf::RenderWindow& win)
{
    static sf::Texture bgTex;
    static sf::Sprite bgSprite;
    static bool loaded = false;
    static bool success = false;

    if (!loaded)
    {
        if (bgTex.loadFromFile("assets/Images/background_snowbros.jpeg"))
        {
            bgSprite.setTexture(bgTex);
            sf::Vector2u size = bgTex.getSize();
            if (size.x > 0 && size.y > 0) {
                bgSprite.setScale(1280.f / size.x, 720.f / size.y);
            }
            success = true;
        }
        loaded = true;
    }

    if (success) {
        win.draw(bgSprite);
    }
    else {
        win.clear(sf::Color(20, 30, 50));
    }
}

void saveProgress(const string& username, int level, int lives, int gems, int highScore)
{
    string timestamp = getCurrentDate();
    ofstream file("progress.txt");
    file << username << " " << level << " " << lives << " " << gems << " " << highScore << " " << timestamp << "\n";
    file.close();
}

bool loadProgress(const string& username, int& level, int& lives, int& gems, int& highScore)
{
    ifstream file("progress.txt");
    if (!file.is_open()) return false;

    string u;
    int l, li, g, hs;
    while (file >> u >> l >> li >> g >> hs)
    {
        if (u == username)
        {
            level = l;
            lives = li;
            gems = g;
            highScore = hs;
            return true;
        }
    }
    return false;
}