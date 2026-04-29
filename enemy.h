#ifndef ENEMY_H
#define ENEMY_H

#include "types.h"

// returns the enemy for a given floor
Enemy getEnemyForFloor(int floor);

// dynamically spawns enemies for a floor, count is set by reference
Enemy* spawnEnemies(int floor, Difficulty difficulty, int& count);

// frees enemy array
void cleanupEnemies(Enemy* enemies);

// updates Cerberus phase based on hp thresholds
void updateCerberusPhase(Enemy& cerberus);

#endif