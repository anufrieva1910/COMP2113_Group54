#include "enemy.h"
#include <iostream>
using namespace std;

// Enemy definitions by floor range
enemy getEnemyForFloor(int floor) {
    if (floor ==1) {
        return {"Bat Swarm", 12, 6, 3, false, 1};
    } else if (floor ==2) {
        return {"Cave Rats", 15, 7, 4, false, 1};
    } else if (floor ==3) {
        return {"Stone Beetle", 30, 8, 5, false, 1};
    } else if (floor == 4) {
        return {"Cerberus", 150, 30, 20, true, 1};
    } else if (floor == 4) {
        return {"Cerberus", 150, 30, 20, true, 1};
    }
    return {"Unknown", 10, 10, 1, 1, false, 1};
}

// Dynamic spawning
enemy* spawnEnemies(int floor, int difficulty, int& count) {
    count = difficulty + (floor / 2);
    Enemy* enemies = new Enemy[count];
    for (int i = 0; i < count; i++) {
        enemies[i] = getEnemyForFloor(floor);
        // scale stats by difficulty
        enemies[i].hp += difficulty * 10;
        enemies[i].attack += difficulty * 2;
        enemies[i].defense += difficulty;
    }
    return enemies;
}

// Cleanup
void cleanupEnemies(Enemy* enemies) {
    delete[] enemies;
}

// Cerberus phase transitions
void updateCerberusPhase(Enemy& cerberus) {
    if (cerberus.isBoss) {
        if (cerberus.hp <= cerberus.maxHP * 0.66 && cerberus.phase == 1) {
            cerberus.attack += 5;
            cerberus.phase = 2;
            cout << "Cerberus enters Phase 2!" << endl;
        }
        if (cerberus.hp <= cerberus.maxHP * 0.33 && cerberus.phase == 2) {
            cerberus.attack += 5;
            cerberus.phase = 3;
            cout << "Cerberus enters Phase 3!" << endl;
        }
    }
}
