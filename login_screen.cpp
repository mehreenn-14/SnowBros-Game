#include "screen.h"

using namespace std;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

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
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");
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
        // Register tab confirm password field + register button
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
