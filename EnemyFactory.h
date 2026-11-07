#pragma once
#include "enemy.h"
#include <string>

class EnemyFactory {
public:

    static Enemy* create(std::string type, float x, float y, Levels* level = nullptr) {
        Enemy* enemy = nullptr;

        if (type == "BOTOM") {
            enemy = new Bottom();
        }
        else if (type == "FLYNG") {
            enemy = new Flyng();
        }
        else if (type == "TORNADO") {
            enemy = new Tornado();
        }
        else if (type == "MOGERA") {
            enemy = new Mogera();
        }
        else if (type == "GAMAKICHI") {
            enemy = new Gamakichi();
        }

        if (enemy != nullptr) {
            enemy->setPosition(x, y);

            enemy->updateHitBox();

            if (level != nullptr) {
                enemy->setLevel(*level);
            }
        }

        return enemy;
    }

    static std::string variantForLevel(int level) {
        if (level <= 5)  return "Red";
        if (level <= 6)  return "Green";
        if (level <= 8)  return "Blue";
        return "Purple";
    }
};