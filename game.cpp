#include "game.h"
#include "screen.h"
#include "gameScreen.h"

#include <iostream>
using namespace std;

Game::Game() {
    Screen* currentScreen = new LoginScreen(window);
    currentScreen->onEnter();

    currentScreen->handleInput();
    currentScreen->update(deltaTime);

    int nextID = currentScreen->getNextScreen();
    if (nextID != 0) { 
        currentScreen->onExit();
        delete currentScreen;

        if (nextID == 1) currentScreen = new MainMenuScreen(window);
        else if (nextID == 2) currentScreen = new CharacterSelectScreen(window);
        else if (nextID == 3) currentScreen = new GameScreen(window, player1, player2);

        currentScreen->onEnter();
    }

    window.clear();
    currentScreen->draw();
    window.display();
}


Game::~Game() {
    delete screen;
    delete player1;
    delete player2;
}

void Game::run() {
    Screen* currentScreen = new LoginScreen(window);
    currentScreen->onEnter();

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        currentScreen->handleInput();

        currentScreen->update(dt);

        int nextID = currentScreen->getNextScreen();
        if (nextID != 0) {
            currentScreen->onExit();
            delete currentScreen;

            if (nextID == 1)      currentScreen = new MainMenuScreen(window);
            else if (nextID == 2) currentScreen = new CharacterSelectScreen(window);
            else if (nextID == 3) currentScreen = new GameScreen(window, player1, player2);
            else if (nextID == 4) currentScreen = new GameOverScreen(window);

            currentScreen->onEnter();
        }

        window.clear();
        currentScreen->draw();
        window.display();
    }
    delete currentScreen;
}