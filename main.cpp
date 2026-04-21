#include "enemy.h"
#include <iostream>
using namespace std;

// Declare functions from enemy.cpp
extern Enemy getEnemyForFloor(int floor);
extern Enemy* spawnEnemies(int floor, int difficulty, int& count);
extern void cleanupEnemies(Enemy* enemies);
extern void updateCerberusPhase(Enemy& cerberus);

int main() {
    int count;
    Enemy* floorEnemies = spawnEnemies(5, 2, count);

    cout << "Enemies spawned: " << count << endl;
    for (int i = 0; i < count; i++) {
        cout << floorEnemies[i].name << " (HP: " 
             << floorEnemies[i].hp << ", ATK: " 
             << floorEnemies[i].attack << ", DEF: " 
             << floorEnemies[i].defense << ")" << endl;
    }

    // Example: simulate Cerberus fight
    Enemy cerberus = getEnemyForFloor(10);
    cerberus.hp = 90; // drop HP to trigger phase 2
    updateCerberusPhase(cerberus);

    cerberus.hp = 40; // drop HP to trigger phase 3
    updateCerberusPhase(cerberus);

    cleanupEnemies(floorEnemies);
    return 0;
}
