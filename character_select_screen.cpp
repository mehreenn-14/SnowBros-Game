#include "screen.h"

using namespace std;

extern const sf::Color BTN_COLOR;
extern const sf::Color BTN_COLOR_DANGER;
extern const sf::Color BTN_COLOR_OK;
extern const sf::Color TEXT_NAVY;

CharacterSelectScreen::CharacterSelectScreen(sf::RenderWindow& win) : Screen(win), pickingPlayer2(false)
{
    font.loadFromFile("assets/Fonts/SuperShiny-0v0rG.ttf");

    // Load the main sprite sheets
    texNick.loadFromFile("assets/Images/Nick.png");
    texRed.loadFromFile("assets/Images/Player_Red.png");
    texGreen.loadFromFile("assets/Images/Player_Green.png");
    texYellow.loadFromFile("assets/Images/Player_Yellow.png");
    texBlue.loadFromFile("assets/Images/Player_Blue.png");

    // Initialize Sprites with textures
    spNick.setTexture(texNick);
    spRed.setTexture(texRed);
    spGreen.setTexture(texGreen);
    spYellow.setTexture(texYellow);
    spBlue.setTexture(texBlue);

    //RECTS
    spNick.setTextureRect(sf::IntRect(124, 664, 206, 271));
    spRed.setTextureRect(sf::IntRect(15, 4, 61, 73));
    spGreen.setTextureRect(sf::IntRect(15, 4, 61, 73));
    spYellow.setTextureRect(sf::IntRect(15, 4, 61, 73));
    spBlue.setTextureRect(sf::IntRect(15, 4, 61, 73));

    // Origins and Scaling for previews
    sf::FloatRect bNick = spNick.getLocalBounds();
    spNick.setOrigin(bNick.width / 2.f, bNick.height / 2.f);
    spNick.setScale(0.5f, 0.5f);

    sf::FloatRect bColor = spRed.getLocalBounds();
    spRed.setOrigin(bColor.width / 2.f, bColor.height / 2.f);
    spRed.setScale(2.0f, 2.0f);

    spGreen.setOrigin(bColor.width / 2.f, bColor.height / 2.f);
    spGreen.setScale(2.0f, 2.0f);

    spYellow.setOrigin(bColor.width / 2.f, bColor.height / 2.f);
    spYellow.setScale(2.0f, 2.0f);

    spBlue.setOrigin(bColor.width / 2.f, bColor.height / 2.f);
    spBlue.setScale(2.0f, 2.0f);

    visibleOptions[0] = 0;
    visibleOptions[1] = 1;
    visibleOptions[2] = 2;
}

void CharacterSelectScreen::onEnter()
{
    isActive = true;
    nextScreen = 5;
    pickingPlayer2 = false;
    selectedCharP1 = -1;
    selectedCharP2 = -1;
    visibleOptions[0] = 0; visibleOptions[1] = 1; visibleOptions[2] = 2;
}

void CharacterSelectScreen::handleInput()
{
    sf::Event e;
    while (window.pollEvent(e))
    {
        if (e.type == sf::Event::Closed) nextScreen = 99;
        if (e.type == sf::Event::MouseButtonPressed)
        {
            int clickedSlot = -1;
            if (isMouseOver(window, 390, 410, 120, 40)) clickedSlot = 0;
            if (isMouseOver(window, 590, 410, 120, 40)) clickedSlot = 1;
            if (isMouseOver(window, 790, 410, 120, 40)) clickedSlot = 2;

            if (clickedSlot != -1) {
                int actualChar = visibleOptions[clickedSlot];
                if (!pickingPlayer2) selectedCharP1 = actualChar;
                else selectedCharP2 = actualChar;
            }

            if (isMouseOver(window, 440, 560, 150, 44)) {
                if (pickingPlayer2) {
                    pickingPlayer2 = false;
                    visibleOptions[0] = 0; visibleOptions[1] = 1; visibleOptions[2] = 2;
                }
                else nextScreen = 9;
            }

            if (isMouseOver(window, 690, 560, 150, 44)) {
                if (!pickingPlayer2 && isMultiplayer && selectedCharP1 != -1) {
                    pickingPlayer2 = true;
                    int p2Slots = 0;
                    for (int i = 0; i <= 2; i++) {
                        if (i != selectedCharP1) visibleOptions[p2Slots++] = i;
                    }
                    visibleOptions[2] = 3;
                    selectedCharP2 = visibleOptions[0];
                }
                else if (selectedCharP1 != -1 && (!isMultiplayer || selectedCharP2 != -1)) {
                    nextScreen = 7;
                }
            }
        }
    }
}

void CharacterSelectScreen::update(float time) {}

void CharacterSelectScreen::draw()
{
    drawSnowBrosBackground(window);
    drawUser(window, font);

    string title = pickingPlayer2 ? "PLAYER 2: CHOOSE CHARACTER" : "PLAYER 1: CHOOSE CHARACTER";
    drawText(window, font, title, 640, 100, 36, sf::Color(200, 230, 255), true);

    for (int i = 0; i < 3; ++i)
    {
        float x = 390.f + i * 200.f;
        int charIdx = visibleOptions[i];
        bool isSelected = false;
        if (!pickingPlayer2) isSelected = (selectedCharP1 == charIdx);
        else isSelected = (selectedCharP2 == charIdx);

        drawPanel(window, x - 20, 210, 160, 180, isSelected ? sf::Color(200, 255, 200) : sf::Color::White, BTN_COLOR);

        sf::Sprite* s = nullptr;
        if (charIdx == 0) s = &spNick;
        else if (charIdx == 1) s = &spRed;
        else if (charIdx == 2) s = &spGreen;
        else if (charIdx == 3) s = &spYellow;
        else if (charIdx == 4) s = &spBlue;

        if (s) {
            s->setPosition(x + 60, 300);
            window.draw(*s);
        }

        string name = "Nick";
        if (charIdx == 1) name = "Red";
        else if (charIdx == 2) name = "Green";
        else if (charIdx == 3) name = "Yellow";
        else if (charIdx == 4) name = "Blue";

        drawRoundButton(window, x, 410, 120, 40, BTN_COLOR, isMouseOver(window, x, 410, 120, 40));
        drawText(window, font, name, x + 60, 432, 18, TEXT_NAVY, true);
    }

    drawRoundButton(window, 440, 560, 150, 44, BTN_COLOR_DANGER, isMouseOver(window, 440, 560, 150, 44));
    drawText(window, font, "Back", 515, 582, 20, TEXT_NAVY, true);

    string nextBtnText = (isMultiplayer && !pickingPlayer2) ? "Next Player" : "Start Game!";
    drawRoundButton(window, 690, 560, 150, 44, BTN_COLOR_OK, isMouseOver(window, 690, 560, 150, 44));
    drawText(window, font, nextBtnText, 765, 582, 20, TEXT_NAVY, true);
}