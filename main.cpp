#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "screen.h"
#include "gameScreen.h"

// -------------------------------------------------------- Screen Manager -----------------------------------------------
class ScreenManager {
private:
    Screen* screens[15];
    int currentScreen;
    sf::RenderWindow& window;

public:
    ScreenManager(sf::RenderWindow& win) : window(win), currentScreen(0) {
        for (int i = 0; i < 15; ++i)
            screens[i] = nullptr;
    }

    void addScreen(int index, Screen* screen) {
        if (index >= 0 && index < 15)
            screens[index] = screen;
    }

    void run() {
        sf::Clock frameClock;
        if (screens[currentScreen])
            screens[currentScreen]->onEnter();

        while (window.isOpen()) {
            float deltaTime = frameClock.restart().asSeconds();

            if (screens[currentScreen]) {
                screens[currentScreen]->handleInput();
                screens[currentScreen]->update(deltaTime);
                screens[currentScreen]->draw();
            }
            else {
                window.clear(sf::Color::White);
            }

            window.display();

            if (screens[currentScreen]) {
                int next = screens[currentScreen]->getNextScreen();

                if (next == 99) {
                    window.close();
                }
                else if (next != currentScreen && next >= 0 && next < 15 && screens[next]) {
                    screens[currentScreen]->onExit();
                    currentScreen = next;
                    screens[currentScreen]->onEnter();
                }
            }
        }
    }
};


MusicManager gMusic;
//HudScreen* globalHud = nullptr;

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Snow Bros");
    window.setFramerateLimit(60);

    
    gMusic.play("assets/Sounds/themeSong.ogg");
    

    // ------------------all screens --------------------------------------
    LoginScreen              loginScreen(window);
    MainMenuScreen           mainMenu(window);
    LeaderboardScreen        leaderboard(window);
    InstructionsScreen       instructions(window);
    PauseScreen              pauseScreen(window);
    ModeSelectionScreen      modeSelection(window);
    CharacterSelectScreen    charSelect(window);
    ShopScreen               shop(window);
    HudScreen                hud(window);
    GameOverScreen           gameOver(window);
    GameScreen               gameScreen(window, nullptr, nullptr);
    LevelCompleteScreen      levelComplete(window);      // screen 11
    BossLevelCompleteScreen  bossLevelComplete(window);  // screen 12

    // --------------------Register screens ----------------------------
       ScreenManager manager(window);

    manager.addScreen(0, &loginScreen);
    manager.addScreen(1, &mainMenu);
    manager.addScreen(2, &leaderboard);
    manager.addScreen(3, &instructions);
    manager.addScreen(4, &pauseScreen);
    manager.addScreen(5, &charSelect);
    manager.addScreen(6, &shop);
    manager.addScreen(7, &gameScreen);
    manager.addScreen(8, &gameOver);
    manager.addScreen(9, &modeSelection);
    manager.addScreen(10, &gameScreen);   // kept for compatibility
    manager.addScreen(11, &levelComplete);
    manager.addScreen(12, &bossLevelComplete);

    globalHud = &hud;

    manager.run();
    return 0;
}