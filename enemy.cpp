#include "enemy.h"
#include <iostream>
using namespace std;

// Enemy definitions by floor range
Enemy getEnemyForFloor(int floor) {
    if (floor ==1) {
        return {"Bat Swarm", 12, 12, 6, 3, false, 1};
    } else if (floor ==2) {
        return {"Cave Rats", 15, 15, 7, 4, false, 1};
    } else if (floor ==3) {
        return {"Stone Beetle", 30, 30, 8, 5, false, 1};
    } else if (floor == 4) {
        return {"Stray Dogs", 20, 20, 10, 6, false, 1};
    } else if (floor == 5) {
        return {"Venomous Snake", 25, 25, 13, 8, false, 1};
    } else if (floor == 6) {
        return {"Rabid Jackal", 30, 30, 15, 10, false, 1};
    } else if (floor == 7) {
        return {"Corrupted Hound", 35, 35, 17, 13, false, 1};
    } else if (floor == 8) {
        return {"Shadow Owl", 45, 45, 20, 15, false, 1};
    } else if (floor == 9) {
        return {"Phantom Direwolf", 50, 50, 25, 17, false, 1};
    } else if (floor == 10) {
        return {"Cerberus", 150, 150, 30, 20, true, 1};
    }
    return {"Unknown", 10, 10, 1, 1, false, 1};
}


