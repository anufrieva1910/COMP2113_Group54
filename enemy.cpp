#include "enemy.h"
using namespace std;

Enemy getEnemyForFloor(int floor, Difficulty difficulty) {
    Enemy e;
    switch (floor) {
        case 1:  e = {"Bat Swarm",        25,  25,  8,  3, false, 1}; break;
        case 2:  e = {"Cave Rats",         35,  35, 10,  4, false, 1}; break;
        case 3:  e = {"Stone Beetle",      50,  50, 13,  6, false, 1}; break;
        case 4:  e = {"Stray Dogs",        60,  60, 16,  7, false, 1}; break;
        case 5:  e = {"Venomous Snake",    70,  70, 19,  9, false, 1}; break;
        case 6:  e = {"Rabid Jackal",      85,  85, 22, 11, false, 1}; break;
        case 7:  e = {"Corrupted Hound",  100, 100, 26, 13, false, 1}; break;
        case 8:  e = {"Shadow Owl",       115, 115, 30, 15, false, 1}; break;
        case 9:  e = {"Phantom Direwolf", 135, 135, 35, 17, false, 1}; break;
        case 10: e = {"Cerberus",         200, 200, 40, 20, true,  1}; break;
        default: e = {"Unknown",           20,  20,  6,  2, false, 1}; break;
    }
    if (difficulty == HARD)      e.attack = e.attack * 3 / 2;
    else if (difficulty == EASY) e.attack = e.attack * 4 / 5;
    return e;
}