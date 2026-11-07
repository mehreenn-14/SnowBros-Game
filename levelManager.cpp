

#include "levelManager.h"
#include <iostream>
using namespace std;

Levels::Levels(int levelno, sf::RenderWindow& window) {

    platformCount = 0;
    enemyCount = 0;
    x = nullptr;
    y = nullptr;
    length = nullptr;
    height = nullptr;
    ex = nullptr;
    ey = nullptr;
    name = nullptr;
    color = nullptr;
    platformShapes = nullptr;


    ifstream in("level" + to_string(levelno) + ".txt");

    if (!in) {
        cout << "ERROR: level" << levelno
            << ".txt not found!" << endl;
        return;
    }

    cout << "File opened!" << endl;

    in >> platformCount >> enemyCount;

    cout << "Platforms: " << platformCount << endl;
    cout << "Enemies: " << enemyCount << endl;

    x = new float[platformCount];
    y = new float[platformCount];
    length = new float[platformCount];
    height = new float[platformCount];
    ex = new float[enemyCount];
    ey = new float[enemyCount];
    name = new string[enemyCount];
    color = new string[enemyCount];
    platformShapes = new sf::RectangleShape[platformCount];

    //reading data
    in >> *this;
    in.close();

    float screenW = static_cast<float>(window.getSize().x);
    float screenH = static_cast<float>(window.getSize().y);
    float scaleX = screenW / 800.0f;
    float scaleY = screenH / 600.0f;

    for (int i = 0; i < platformCount; i++) {
        x[i] = static_cast<float>(x[i] * scaleX);
        y[i] = static_cast<float>(y[i] * scaleY);
        length[i] = static_cast<float>(length[i] * scaleX);
        height[i] = static_cast<float>(height[i] * scaleY);
    }
    for (int i = 0; i < enemyCount; i++) {
        ex[i] *= scaleX;
        ey[i] *= scaleY;
    }

    // Background 
    if (!bgTexture.loadFromFile("assets/Images/level" + to_string(levelno) + ".jpg")) {
        if (!bgTexture.loadFromFile("assets/Images/level" + to_string(levelno) + ".jpeg")) {
            std::cout << "Warning: Could not load background for level " << levelno << "\n";
        }
    }
    background.setTexture(bgTexture);

    sf::Vector2u texSize = bgTexture.getSize();
    if (texSize.x > 0 && texSize.y > 0) {
        background.setScale(
            (float)window.getSize().x / texSize.x,
            (float)window.getSize().y / texSize.y
        );
    }

    setupShapes();
}

// setupShapes
void Levels::setupShapes() {
    for (int i = 0; i < platformCount; i++) {
        platformShapes[i].setPosition(x[i], y[i]);
        platformShapes[i].setSize(sf::Vector2f(length[i], height[i]));

        platformShapes[i].setFillColor(sf::Color(80, 70, 60));
        platformShapes[i].setOutlineColor(sf::Color(50, 40, 30));
        platformShapes[i].setOutlineThickness(1.0f);
    }
}

ifstream& operator>>(ifstream& in, Levels& l) {
    string word;
    int p = 0;
    int e = 0;

    while (in >> word) {
        if (word == "PLATFORM") {
            in >> l.x[p]
                >> l.y[p]
                >> l.length[p]
                >> l.height[p];
            cout << "Platform " << p
                << ": " << l.x[p]
                << "," << l.y[p] << endl;
            p++;
        }
        else if (word == "ENEMY") {
            in >> l.name[e]
                >> l.ex[e]
                >> l.ey[e]
                >> l.color[e];
            cout << "Enemy: " << l.name[e] << endl;
            e++;
        }
    }
    return in;
}

void Levels::draw(sf::RenderWindow& window) {
    window.draw(background);
    for (int i = 0; i < platformCount; i++) {
        float px = x[i], py = y[i], pw = length[i], ph = height[i];

        window.draw(platformShapes[i]);

        // snow on top
        sf::RectangleShape snow(sf::Vector2f(pw, ph * 0.35f));
        snow.setPosition(px, py);
        snow.setFillColor(sf::Color(220, 235, 255));
        window.draw(snow);

        // white sparkle highlight
        sf::RectangleShape highlight(sf::Vector2f(pw, 3.f));
        highlight.setPosition(px, py);
        highlight.setFillColor(sf::Color(255, 255, 255));
        window.draw(highlight);

        // ice layer
        sf::RectangleShape ice(sf::Vector2f(pw, ph * 0.25f));
        ice.setPosition(px, py + ph * 0.35f);
        ice.setFillColor(sf::Color(130, 160, 190));
        window.draw(ice);

        // stone details
        for (int s = 0; s < (int)(pw / 60); s++) {
            sf::RectangleShape stone(sf::Vector2f(30, ph * 0.3f));
            stone.setPosition(px + 15 + s * 60, py + ph * 0.5f);
            stone.setFillColor(sf::Color(100, 120, 150));
            stone.setOutlineColor(sf::Color(60, 80, 110));
            stone.setOutlineThickness(1.f);
            window.draw(stone);
        }

        // shadow at bottom
        sf::RectangleShape shadow(sf::Vector2f(pw, ph * 0.15f));
        shadow.setPosition(px, py + ph * 0.85f);
        shadow.setFillColor(sf::Color(30, 40, 60, 180));
        window.draw(shadow);
    }
}

Levels::~Levels() {
    delete[] x;
    delete[] y;
    delete[] length;
    delete[] height;
    delete[] ex;
    delete[] ey;
    delete[] name;
    delete[] color;
    delete[] platformShapes;
}