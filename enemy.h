#ifndef ENEMY_H
#define ENEMY_H

#include "types.h"

// returns the enemy for a given floor
Enemy getEnemyForFloor(int floor, Difficulty difficulty = NORMAL);

#endif