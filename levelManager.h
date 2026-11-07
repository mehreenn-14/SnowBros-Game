#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
using namespace std;
using namespace sf;

class Levels {
private:
    int platformCount;
    int enemyCount;

    // platform data
    float* x;
    float* y;
    float* length;
    float* height;

    // enemy data
    float* ex;      
    float* ey;      
    string* name;   //TYPE 
    string* color;     

    RectangleShape* platformShapes;

    Texture *platformTexture;
    Sprite *platform;
    Texture bgTexture;
    Sprite background;

public:
    Levels(int levelno, sf::RenderWindow& window);
    ~Levels();
    void setupShapes();
    void draw(sf::RenderWindow& window);
    friend ifstream& operator>>(ifstream& in, Levels& l);


    // getters
    int   getPlatformCount() { return platformCount; }
    int   getEnemyCount() { return enemyCount; }
    float getPlatformX(int i) { return x[i]; }
    float getPlatformY(int i) { return y[i]; }
    float getPlatformW(int i) { return length[i]; }
    float getPlatformH(int i) { return height[i]; }
    float getEnemyX(int i) { return ex[i]; }
    float getEnemyY(int i) { return ey[i]; }
    string getEnemyName(int i) { return name[i]; }
    string getEnemyColor(int i) { return color[i]; }


};


