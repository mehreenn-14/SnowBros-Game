#pragma once
#include<iostream>
#include <SFML/Graphics.hpp>  
using namespace std;

class Game
{
	sf::RenderWindow window;
	bool isRunning;
	sf::Clock clock;
	float deltaTime;

	void ProcessInput();
	void update();
	void render();

public:
	Game();
	void run();

};

class InputManager
{
	bool keyHeld;          // if key is being held
	bool keyPressed;       // if key is just pressed


	void update();


};