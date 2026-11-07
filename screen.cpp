#include "screen.h"
#include <cmath>

using namespace std;

string currentUser = "";
bool isMultiplayer = false;
int shopReturnScreen = 1;

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
    string u, p;
    while (file >> u >> p) 
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
    ofstream fileOut("users.txt", ios::app);
    fileOut << username << " " << hashed << "\n";
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
    
    for(int i=0; i<10; i++) {
        float a = pi + (pi/2.f) * ((float)i/9.f);
        rect.setPoint(pt++, sf::Vector2f(x + radius + radius * cos(a), y + radius + radius * sin(a)));
    }
    for(int i=0; i<10; i++) {
        float a = 1.5f*pi + (pi/2.f) * ((float)i/9.f);
        rect.setPoint(pt++, sf::Vector2f(x + w - radius + radius * cos(a), y + radius + radius * sin(a)));
    }
    for(int i=0; i<10; i++) {
        float a = 0.f + (pi/2.f) * ((float)i/9.f);
        rect.setPoint(pt++, sf::Vector2f(x + w - radius + radius * cos(a), y + h - radius + radius * sin(a)));
    }
    for(int i=0; i<10; i++) {
        float a = pi/2.f + (pi/2.f) * ((float)i/9.f);
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
        if (bgTex.loadFromFile("background_snowbros.jpeg")) 
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
    } else {
        win.clear(sf::Color(20, 30, 50));
    }
}

void drawWhiteThemeBackground(sf::RenderWindow& win)
{
    sf::VertexArray grad(sf::Quads, 4);
    sf::Vector2u sz = win.getSize();
    grad[0].position = sf::Vector2f(0.f, 0.f);
    grad[1].position = sf::Vector2f((float)sz.x, 0.f);
    grad[2].position = sf::Vector2f((float)sz.x, (float)sz.y);
    grad[3].position = sf::Vector2f(0.f, (float)sz.y);

    grad[0].color = sf::Color(240, 248, 255);
    grad[1].color = sf::Color(220, 240, 255);
    grad[2].color = sf::Color(180, 220, 255);
    grad[3].color = sf::Color(200, 230, 255);
    win.draw(grad);

    static struct Snowflake { float x, y, speed, size; } flakes[60];
    static bool initFlakes = false;
    static sf::Clock clock;

    if (!initFlakes)
    {
        for (int i = 0; i < 60; ++i)
        {
            flakes[i].x = (float)(rand() % 1280);
            flakes[i].y = (float)(rand() % 720);
            flakes[i].speed = 20.f + (rand() % 40);
            flakes[i].size = 2.f + (rand() % 4);
        }
        initFlakes = true;
    }

    float dt = clock.restart().asSeconds();
    if (dt > 0.1f) dt = 0.016f;

    sf::CircleShape circle;
    unsigned int maxX = sz.x > 0 ? sz.x : 1;
    for (int i = 0; i < 60; ++i)
    {
        flakes[i].y += flakes[i].speed * dt;
        if (flakes[i].y > (float)sz.y)
        {
            flakes[i].y = -10.f;
            flakes[i].x = (float)(rand() % maxX);
        }
        circle.setRadius(flakes[i].size);
        circle.setPosition(flakes[i].x, flakes[i].y);
        circle.setFillColor(sf::Color(100, 150, 255, 100));
        win.draw(circle);
    }
}

const sf::Color BTN_COLOR(176, 224, 230);
const sf::Color BTN_COLOR_DANGER(255, 105, 180);
const sf::Color BTN_COLOR_OK(152, 251, 152);
const sf::Color TEXT_NAVY(0, 0, 128);

string LoginScreen::getValidationMessage(int result) 
{
    if (result == VAL_USERNAME_EMPTY)    return "Username cannot be empty!";
    if (result == VAL_USERNAME_SHORT)    return "Username must be at least 3 characters!";
    if (result == VAL_USERNAME_SPACES)   return "Username cannot contain spaces!";
    if (result == VAL_PASSWORD_EMPTY)    return "Password cannot be empty!";
    if (result == VAL_PASSWORD_SHORT)    return "Password must be at least 6 characters!";
    if (result == VAL_PASSWORD_SPACES)   return "Password cannot contain spaces!";
    if (result == VAL_PASSWORDS_MISMATCH) return "Passwords do not match!";
    if (result == VAL_DUPLICATE_USER)    return "Username already taken!";
    return "";
}

LoginScreen::LoginScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("SuperShiny-0v0rG.ttf");
    activeTab = 0;
}
void LoginScreen::onEnter() 
{
    isActive = true;
    nextScreen = 0;
    username = "";
    password = "";
    confirmPassword = "";
    usernameError = "";
    passwordError = "";
    confirmError = "";
    loginMessage = "";
    focus = 0;
    activeTab = 0;
}
void LoginScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) 
        {
            nextScreen = 99;
        }
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            // Tab switching
            if (isMouseOver(window, 420, 140, 220, 50)) 
            {
                if (activeTab != 0) 
                {
                    activeTab = 0;
                    username = ""; password = ""; confirmPassword = "";
                    usernameError = ""; passwordError = ""; confirmError = "";
                    loginMessage = "";
                    focus = 0;
                }
            }
            else if (isMouseOver(window, 640, 140, 220, 50)) 
            {
                if (activeTab != 1) 
                {
                    activeTab = 1;
                    username = ""; password = ""; confirmPassword = "";
                    usernameError = ""; passwordError = ""; confirmError = "";
                    loginMessage = "";
                    focus = 0;
                }
            }
            
            // Field focus clicks
            if (isMouseOver(window, 460, 250, 360, 40)) focus = 0;
            else if (isMouseOver(window, 460, 330, 360, 40)) focus = 1;
            else if (activeTab == 1 && isMouseOver(window, 460, 410, 360, 40)) focus = 2;
            
            if (activeTab == 0) 
            {
                // Login button
                if (isMouseOver(window, 560, 420, 160, 44)) 
                {
                    usernameError = "";
                    passwordError = "";
                    loginMessage = "";
                    
                    bool hasError = false;
                    if (username.empty()) 
                    {
                        usernameError = "Username cannot be empty!";
                        hasError = true;
                    }
                    if (password.empty()) 
                    {
                        passwordError = "Password cannot be empty!";
                        hasError = true;
                    }
                    
                    if (!hasError) 
                    {
                        if (authManager.loginUser(username, password)) 
                        {
                            currentUser = username;
                            username = "";
                            password = "";
                            nextScreen = 1; 
                        } 
                        else 
                        {
                            loginMessage = "Invalid username or password!";
                        }
                    }
                }
            }
            else 
            {
                // Register button
                if (isMouseOver(window, 560, 500, 160, 44)) 
                {
                    usernameError = "";
                    passwordError = "";
                    confirmError = "";
                    loginMessage = "";
                    
                    int result = authManager.registerUser(username, password, confirmPassword);
                    
                    if (result == VAL_OK) 
                    {
                        activeTab = 0;
                        username = "";
                        password = "";
                        confirmPassword = "";
                        loginMessage = "Account Created! Please Login.";
                        focus = 0;
                    }
                    else 
                    {
                        string msg = getValidationMessage(result);
                        if (result == VAL_USERNAME_EMPTY || result == VAL_USERNAME_SHORT || 
                            result == VAL_USERNAME_SPACES || result == VAL_DUPLICATE_USER) 
                        {
                            usernameError = msg;
                        }
                        else if (result == VAL_PASSWORD_EMPTY || result == VAL_PASSWORD_SHORT || 
                                 result == VAL_PASSWORD_SPACES) 
                        {
                            passwordError = msg;
                        }
                        else if (result == VAL_PASSWORDS_MISMATCH) 
                        {
                            confirmError = msg;
                        }
                    }
                }
            }
        }
        if (e.type == sf::Event::TextEntered) 
        {
            int maxFocus = (activeTab == 1) ? 2 : 1;
            
            if (e.text.unicode == 9) 
            {
                focus = (focus + 1) % (maxFocus + 1);
            } 
            else if (e.text.unicode == 8) 
            {
                if (focus == 0 && username.length() > 0) 
                {
                    username.pop_back();
                } 
                else if (focus == 1 && password.length() > 0) 
                {
                    password.pop_back();
                }
                else if (focus == 2 && confirmPassword.length() > 0) 
                {
                    confirmPassword.pop_back();
                }
            } 
            else if (e.text.unicode >= 32 && e.text.unicode < 127) 
            {
                if (focus == 0 && username.length() < 30) 
                {
                    username += static_cast<char>(e.text.unicode);
                } 
                else if (focus == 1 && password.length() < 30) 
                {
                    password += static_cast<char>(e.text.unicode);
                }
                else if (focus == 2 && confirmPassword.length() < 30) 
                {
                    confirmPassword += static_cast<char>(e.text.unicode);
                }
            }
        }
    }
}
void LoginScreen::update(float time) {}
void LoginScreen::draw() 
{
    drawSnowBrosBackground(window);
    
    float panelH = (activeTab == 1) ? 500.f : 380.f;
    drawPanel(window, 420, 140, 440, panelH, sf::Color::White, BTN_COLOR);
    
    // Tab buttons
    bool hovLoginTab = isMouseOver(window, 420, 140, 220, 50);
    bool hovRegTab   = isMouseOver(window, 640, 140, 220, 50);
    
    sf::Color loginTabColor  = (activeTab == 0) ? sf::Color::White : sf::Color(220, 220, 220);
    sf::Color regTabColor    = (activeTab == 1) ? sf::Color::White : sf::Color(220, 220, 220);
    if (activeTab != 0 && hovLoginTab) loginTabColor = sf::Color(235, 235, 235);
    if (activeTab != 1 && hovRegTab)   regTabColor   = sf::Color(235, 235, 235);
    
    sf::RectangleShape loginTab(sf::Vector2f(220, 50));
    loginTab.setPosition(420, 140);
    loginTab.setFillColor(loginTabColor);
    loginTab.setOutlineColor(BTN_COLOR);
    loginTab.setOutlineThickness(activeTab == 0 ? 0.f : 1.f);
    window.draw(loginTab);
    drawText(window, font, "LOGIN", 530, 165, 22, activeTab == 0 ? TEXT_NAVY : sf::Color(120,120,120), true);
    
    sf::RectangleShape regTab(sf::Vector2f(220, 50));
    regTab.setPosition(640, 140);
    regTab.setFillColor(regTabColor);
    regTab.setOutlineColor(BTN_COLOR);
    regTab.setOutlineThickness(activeTab == 1 ? 0.f : 1.f);
    window.draw(regTab);
    drawText(window, font, "REGISTER", 750, 165, 22, activeTab == 1 ? TEXT_NAVY : sf::Color(120,120,120), true);
    
    // Active tab underline
    sf::RectangleShape underline(sf::Vector2f(220, 3));
    underline.setPosition(activeTab == 0 ? 420.f : 640.f, 188);
    underline.setFillColor(BTN_COLOR);
    window.draw(underline);
    
    // Username field (both tabs)
    drawText(window, font, "Username:", 460, 210, 20, TEXT_NAVY, false);
    drawPanel(window, 460, 240, 360, 40, sf::Color(240, 240, 240), focus == 0 ? BTN_COLOR : sf::Color::White);
    drawText(window, font, username, 470, 248, 20, TEXT_NAVY, false);
    if (!usernameError.empty()) 
    {
        drawText(window, font, usernameError, 460, 283, 14, BTN_COLOR_DANGER, false);
    }
    
    // Password field (both tabs)
    drawText(window, font, "Password:", 460, 300, 20, TEXT_NAVY, false);
    drawPanel(window, 460, 330, 360, 40, sf::Color(240, 240, 240), focus == 1 ? BTN_COLOR : sf::Color::White);
    
    std::string hiddenPass = "";
    for (size_t i = 0; i < password.length(); i++) 
    {
        hiddenPass += "*";
    }
    drawText(window, font, hiddenPass, 470, 338, 20, TEXT_NAVY, false);
    if (!passwordError.empty()) 
    {
        drawText(window, font, passwordError, 460, 373, 14, BTN_COLOR_DANGER, false);
    }
    
    if (activeTab == 0) 
    {
        // Login tab - just the login button
        bool hovLogin = isMouseOver(window, 560, 420, 160, 44);
        drawRoundButton(window, 560, 420, 160, 44, BTN_COLOR_OK, hovLogin);
        drawText(window, font, "Login", 640, 442, 20, TEXT_NAVY, true);
        
        if (!loginMessage.empty()) 
        {
            sf::Color msgColor = (loginMessage.find("Created") != string::npos) ? sf::Color(0, 150, 0) : BTN_COLOR_DANGER;
            drawText(window, font, loginMessage, 640, 485, 16, msgColor, true);
        }
    }
    else 
    {
        // Register tab - confirm password field + register button
        drawText(window, font, "Confirm Password:", 460, 390, 20, TEXT_NAVY, false);
        drawPanel(window, 460, 420, 360, 40, sf::Color(240, 240, 240), focus == 2 ? BTN_COLOR : sf::Color::White);
        
        std::string hiddenConfirm = "";
        for (size_t i = 0; i < confirmPassword.length(); i++) 
        {
            hiddenConfirm += "*";
        }
        drawText(window, font, hiddenConfirm, 470, 428, 20, TEXT_NAVY, false);
        if (!confirmError.empty()) 
        {
            drawText(window, font, confirmError, 460, 463, 14, BTN_COLOR_DANGER, false);
        }
        
        bool hovReg = isMouseOver(window, 560, 500, 160, 44);
        drawRoundButton(window, 560, 500, 160, 44, BTN_COLOR_OK, hovReg);
        drawText(window, font, "Register", 640, 522, 20, TEXT_NAVY, true);
        
        if (!loginMessage.empty()) 
        {
            drawText(window, font, loginMessage, 640, 565, 16, BTN_COLOR_DANGER, true);
        }
    }
}

MainMenuScreen::MainMenuScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("SuperShiny-0v0rG.ttf");
}
void MainMenuScreen::onEnter() { isActive = true; nextScreen = 1; }
void MainMenuScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            if (isMouseOver(window, 540, 240, 200, 44)) nextScreen = 9; // Mode Selection
            if (isMouseOver(window, 540, 300, 200, 44)) nextScreen = 2;
            if (isMouseOver(window, 540, 360, 200, 44)) { shopReturnScreen = 1; nextScreen = 6; }
            if (isMouseOver(window, 540, 420, 200, 44)) nextScreen = 3;
            if (isMouseOver(window, 540, 480, 200, 44)) nextScreen = 0;
            if (isMouseOver(window, 540, 540, 200, 44)) nextScreen = 99;
        }
    }
}
void MainMenuScreen::update(float time) {}
void MainMenuScreen::draw() 
{
    drawSnowBrosBackground(window);
    drawUser(window, font);
    drawText(window, font, "MAIN MENU", 640, 160, 48, BTN_COLOR, true);
    
    std::string labels[] = {"Play", "Leaderboard", "Shop", "Instructions", "Logout", "Quit"};
    sf::Color colors[] = {BTN_COLOR_OK, BTN_COLOR, BTN_COLOR, BTN_COLOR, sf::Color(200,200,200), BTN_COLOR_DANGER};
    
    for (int i = 0; i < 6; ++i) 
    {
        float y = 240.f + i * 60.f;
        bool hov = isMouseOver(window, 540, y, 200, 44);
        drawRoundButton(window, 540, y, 200, 44, colors[i], hov);
        drawText(window, font, labels[i], 640, y + 22, 20, TEXT_NAVY, true);
    }
}

ModeSelectionScreen::ModeSelectionScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("SuperShiny-0v0rG.ttf");
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

LeaderboardScreen::LeaderboardScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("SuperShiny-0v0rG.ttf");
    entryCount = 0;
}
void LeaderboardScreen::onEnter() 
{ 
    isActive = true; 
    nextScreen = 2; 
    entryCount = 0;
    ifstream file("leaderboard.txt");
    if (file.is_open()) 
    {
        string entry_id, user_id, score, level_reached, recorded_at;
        while (file >> entry_id >> user_id >> score >> level_reached >> recorded_at && entryCount < 10) {
            entries[entryCount] = user_id + " - Score: " + score + " Level: " + level_reached;
            entryCount++;
        }
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
        for (int i = 0; i < entryCount && i < 6; ++i) 
        {
            drawText(window, font, intToString(i + 1) + ". " + entries[i], 420, 210 + i * 40, 20, TEXT_NAVY, false);
        }
    }
    
    bool hov = isMouseOver(window, 540, 560, 200, 44);
    drawRoundButton(window, 540, 560, 200, 44, BTN_COLOR, hov);
    drawText(window, font, "Back", 640, 582, 20, TEXT_NAVY, true);
}

InstructionsScreen::InstructionsScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("SuperShiny-0v0rG.ttf");
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
    drawPanel(window, 340, 110, 600, 420, sf::Color::White, BTN_COLOR);
    drawText(window, font, "HOW TO PLAY", 640, 150, 36, TEXT_NAVY, true);
    
    drawText(window, font, "- Use Arrow Keys to move.", 370, 220, 20, TEXT_NAVY, false);
    drawText(window, font, "- Press SPACE to throw snowballs.", 370, 260, 20, TEXT_NAVY, false);
    drawText(window, font, "- Trap enemies in snow to defeat them.", 370, 300, 20, TEXT_NAVY, false);
    drawText(window, font, "- Collect gems to buy upgrades in the shop.", 370, 340, 20, TEXT_NAVY, false);
    
    bool hov = isMouseOver(window, 540, 560, 200, 44);
    drawRoundButton(window, 540, 560, 200, 44, BTN_COLOR, hov);
    drawText(window, font, "Back", 640, 582, 20, TEXT_NAVY, true);
}

CharacterSelectScreen::CharacterSelectScreen(sf::RenderWindow& win) : Screen(win), selectedChar(0) 
{
    font.loadFromFile("SuperShiny-0v0rG.ttf");
    
    if (texBlueEnemy.loadFromFile("FlyingFoogaFoog_Blue.png")) 
    {
        spBlueEnemy.setTexture(texBlueEnemy);
        spBlueEnemy.setTextureRect(sf::IntRect(0, 0, 64, 64)); 
    }
    if (texRedEnemy.loadFromFile("Botom_Pink.png")) 
    {
        spRedEnemy.setTexture(texRedEnemy);
        spRedEnemy.setTextureRect(sf::IntRect(0, 30, 64, 64)); 
    }
    if (texOrangeEnemy.loadFromFile("FlyingFoogaFoog_Orange.png")) 
    {
        spOrangeEnemy.setTexture(texOrangeEnemy);
        spOrangeEnemy.setTextureRect(sf::IntRect(0, 0, 64, 64));
    }
    
    spBlueEnemy.setScale(2.f, 2.f);
    spRedEnemy.setScale(2.f, 2.f);
    spOrangeEnemy.setScale(2.f, 2.f);
}
void CharacterSelectScreen::onEnter() 
{ 
    isActive = true; 
    nextScreen = 5; 
}
void CharacterSelectScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            if (isMouseOver(window, 390, 410, 120, 40)) selectedChar = 0;
            if (isMouseOver(window, 590, 410, 120, 40)) selectedChar = 1;
            if (isMouseOver(window, 790, 410, 120, 40)) selectedChar = 2;
            
            if (isMouseOver(window, 440, 560, 150, 44)) nextScreen = 9; // Go back to ModeSelection
            if (isMouseOver(window, 690, 560, 150, 44)) nextScreen = 7;
        }
    }
}
void CharacterSelectScreen::update(float time) {}
void CharacterSelectScreen::draw() 
{
    drawSnowBrosBackground(window);
    drawUser(window, font);
    drawText(window, font, "SELECT CHARACTER", 640, 140, 40, TEXT_NAVY, true);
    
    for (int i=0; i<3; ++i) 
    {
        float x = 390.f + i * 200.f;
        drawPanel(window, x - 20, 210, 160, 180, selectedChar == i ? sf::Color(200, 230, 255) : sf::Color::White, BTN_COLOR);
    }
    
    spBlueEnemy.setPosition(390, 240); window.draw(spBlueEnemy);
    spRedEnemy.setPosition(590, 240); window.draw(spRedEnemy);
    spOrangeEnemy.setPosition(790, 240); window.draw(spOrangeEnemy);
    
    drawRoundButton(window, 390, 410, 120, 40, BTN_COLOR, isMouseOver(window, 390, 410, 120, 40));
    drawText(window, font, "P1", 450, 430, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 590, 410, 120, 40, BTN_COLOR, isMouseOver(window, 590, 410, 120, 40));
    drawText(window, font, "P2", 650, 430, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 790, 410, 120, 40, BTN_COLOR, isMouseOver(window, 790, 410, 120, 40));
    drawText(window, font, "P3", 850, 430, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 440, 560, 150, 44, BTN_COLOR_DANGER, isMouseOver(window, 440, 560, 150, 44));
    drawText(window, font, "Back", 515, 582, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 690, 560, 150, 44, BTN_COLOR_OK, isMouseOver(window, 690, 560, 150, 44));
    drawText(window, font, "Start Game!", 765, 582, 20, TEXT_NAVY, true);
}

HudScreen::HudScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("SuperShiny-0v0rG.ttf");
}
void HudScreen::onEnter() 
{ 
    isActive = true; 
    nextScreen = 7; 
}
void HudScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) {
            nextScreen = 4;
        }
        if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Space) {
            nextScreen = 8;
        }
    }
}
void HudScreen::update(float time) {}
void HudScreen::draw() 
{
    drawSnowBrosBackground(window); 
    drawText(window, font, "(Game is running... Press Space to simulate Game Over. ESC to pause)", 640, 360, 20, TEXT_NAVY, true);
    
    sf::RectangleShape bar(sf::Vector2f(1280, 50));
    bar.setFillColor(sf::Color(255, 255, 255, 200));
    window.draw(bar);
    
    drawText(window, font, "Score: 015200", 260, 12, 24, TEXT_NAVY, false);
    drawText(window, font, "Level: 3", 590, 12, 24, TEXT_NAVY, false);
    drawText(window, font, "Lives: 3", 890, 12, 24, TEXT_NAVY, false);
}

PauseScreen::PauseScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("SuperShiny-0v0rG.ttf");
}
void PauseScreen::onEnter() 
{ 
    isActive = true; 
    nextScreen = 4; 
}
void PauseScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            if (isMouseOver(window, 540, 240, 200, 44)) nextScreen = 7;
            if (isMouseOver(window, 540, 310, 200, 44)) { shopReturnScreen = 4; nextScreen = 6; }
            if (isMouseOver(window, 540, 380, 200, 44)) {
                // Save game logic goes here
            }
            if (isMouseOver(window, 540, 450, 200, 44)) {
                currentUser = ""; // Logout
                nextScreen = 0;
            }
            if (isMouseOver(window, 540, 520, 200, 44)) nextScreen = 1;
        }
    }
}
void PauseScreen::update(float time) {}
void PauseScreen::draw() 
{
    drawSnowBrosBackground(window);
    
    sf::RectangleShape overlay(sf::Vector2f(1280, 720));
    overlay.setFillColor(sf::Color(255, 255, 255, 180));
    window.draw(overlay);
    
    drawPanel(window, 490, 150, 300, 450, sf::Color::White, BTN_COLOR);
    drawText(window, font, "PAUSED", 640, 190, 36, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 240, 200, 44, BTN_COLOR_OK, isMouseOver(window, 540, 240, 200, 44));
    drawText(window, font, "Resume", 640, 262, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 310, 200, 44, BTN_COLOR, isMouseOver(window, 540, 310, 200, 44));
    drawText(window, font, "Open Shop", 640, 332, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 380, 200, 44, BTN_COLOR, isMouseOver(window, 540, 380, 200, 44));
    drawText(window, font, "Save Game", 640, 402, 20, TEXT_NAVY, true);

    drawRoundButton(window, 540, 450, 200, 44, sf::Color(200,200,200), isMouseOver(window, 540, 450, 200, 44));
    drawText(window, font, "Logout", 640, 472, 20, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 520, 200, 44, BTN_COLOR_DANGER, isMouseOver(window, 540, 520, 200, 44));
    drawText(window, font, "Exit to Menu", 640, 542, 20, TEXT_NAVY, true);
}

ShopScreen::ShopScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("SuperShiny-0v0rG.ttf");
}
void ShopScreen::onEnter() 
{ 
    isActive = true; 
    nextScreen = 6; 
    gems = 100;
    shopMessage = "";
    pendingItemIndex = -1;
}
void ShopScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            if (pendingItemIndex != -1) 
            {
                if (isMouseOver(window, 480, 400, 120, 44)) 
                {
                    int costs[] = {50, 20, 30, 25, 35};
                    string msgs[] = {"Purchased Extra Life!", "Purchased Speed Boost!", "Purchased Snowball Power!", "Purchased Distance Increase!", "Purchased Balloon Mode!"};
                    
                    if (gems >= costs[pendingItemIndex]) {
                        gems -= costs[pendingItemIndex];
                        shopMessage = msgs[pendingItemIndex];
                    } else {
                        shopMessage = "Not enough gems!";
                    }
                    pendingItemIndex = -1;
                }
                else if (isMouseOver(window, 680, 400, 120, 44)) 
                {
                    pendingItemIndex = -1;
                    shopMessage = "Purchase cancelled.";
                }
            }
            else 
            {
                if (isMouseOver(window, 540, 600, 200, 44)) nextScreen = shopReturnScreen;
                
                for (int i = 0; i < 5; i++) {
                    float y = 200 + i * 70;
                    if (isMouseOver(window, 300, y, 680, 50)) {
                        pendingItemIndex = i;
                        shopMessage = ""; // Clear previous message
                    }
                }
            }
        }
    }
}
void ShopScreen::update(float time) {}
void ShopScreen::draw() 
{
    drawSnowBrosBackground(window);
    drawUser(window, font);
    drawPanel(window, 280, 100, 720, 480, sf::Color::White, BTN_COLOR);
    drawText(window, font, "GEMS SHOP", 640, 130, 32, TEXT_NAVY, true);
    drawText(window, font, "Gems: " + intToString(gems), 640, 170, 24, sf::Color(0, 100, 0), true);
    
    string items[] = {"Extra Life", "Speed Boost (30s)", "Snowball Power", "Distance Increase", "Balloon Mode (30s)"};
    int costs[] = {50, 20, 30, 25, 35};
    
    for (int i = 0; i < 5; i++) 
    {
        float y = 200 + i * 70;
        bool hov = isMouseOver(window, 300, y, 680, 50);
        drawRoundButton(window, 300, y, 680, 50, sf::Color(240, 240, 240), hov);
        drawText(window, font, items[i], 320, y + 13, 20, TEXT_NAVY, false);
        drawText(window, font, intToString(costs[i]) + " Gems", 900, y + 13, 20, sf::Color(0, 100, 0), false);
    }
    
    drawText(window, font, shopMessage, 640, 560, 20, TEXT_NAVY, true);
    
    bool hovB = isMouseOver(window, 540, 600, 200, 44);
    drawRoundButton(window, 540, 600, 200, 44, BTN_COLOR_DANGER, hovB);
    drawText(window, font, "Back", 640, 622, 20, TEXT_NAVY, true);
    
    if (pendingItemIndex != -1) 
    {
        sf::RectangleShape overlay(sf::Vector2f(1280, 720));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);
        
        drawPanel(window, 440, 300, 400, 180, sf::Color::White, BTN_COLOR);
        drawText(window, font, "Buy " + items[pendingItemIndex] + "?", 640, 340, 20, TEXT_NAVY, true);
        
        drawRoundButton(window, 480, 400, 120, 44, BTN_COLOR_OK, isMouseOver(window, 480, 400, 120, 44));
        drawText(window, font, "Yes", 540, 422, 20, TEXT_NAVY, true);
        
        drawRoundButton(window, 680, 400, 120, 44, BTN_COLOR_DANGER, isMouseOver(window, 680, 400, 120, 44));
        drawText(window, font, "No", 740, 422, 20, TEXT_NAVY, true);
    }
}

GameOverScreen::GameOverScreen(sf::RenderWindow& win) : Screen(win) 
{
    font.loadFromFile("SuperShiny-0v0rG.ttf");
}
void GameOverScreen::onEnter() 
{ 
    isActive = true; 
    nextScreen = 8; 
}
void GameOverScreen::handleInput() 
{
    sf::Event e;
    while (window.pollEvent(e)) 
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed) 
        {
            if (isMouseOver(window, 540, 460, 200, 44)) 
            nextScreen = 1;
        }
    }
}
void GameOverScreen::update(float time) {}
void GameOverScreen::draw() 
{
    drawWhiteThemeBackground(window);
    drawUser(window, font);
    drawText(window, font, "GAME OVER", 640, 260, 64, BTN_COLOR_DANGER, true);
    drawText(window, font, "Final Score: " + intToString(gameScore), 640, 360, 28, TEXT_NAVY, true);
    
    drawRoundButton(window, 540, 460, 200, 44, BTN_COLOR, isMouseOver(window, 540, 460, 200, 44));
    drawText(window, font, "Main Menu", 640, 482, 20, TEXT_NAVY, true);
}
