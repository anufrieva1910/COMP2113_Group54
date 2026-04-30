#ifndef COMBAT_H
#define COMBAT_H

#include "types.h"
#include "items.h"
#include <string>


// Calculate the damage taken by the defensive side for each turn
int calculateDamage(int attack, int defense);

// Switches the turn after an attack from either player or enemy
void turn_change(std::string &turn);

// Randomizes the starting side of the combat
void randomize_start(std::string &turn);

// Uses an item in the inventory chosen by player
void useItem(Player &player);

// Combat logic when player encounters the final boss
void boss_combat(Player &player, Enemy &boss);

void clearCombatLog();

// Combat logic when player encounters an enemy
void encounter_combat(Player &player, Enemy &enemy);

// Randomly roll an item to drop after player defeats enemy
void random_lootdrop(Player &player, Enemy enemy);

#endif