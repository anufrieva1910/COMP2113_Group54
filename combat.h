#ifndef COMBAT_H
#define COMBAT_H

#include "types.h"
#include "items.h"
#include <string>


// calculate the damage taken by the defensive side for each turn
int calculateDamage(int attack, int defense);

// switches the turn after an attack from either player or enemy
void turn_change(std::string &turn);

// randomizes the starting side of the combat
void randomize_start(std::string &turn);

// uses an item in the inventory chosen by player
void useItem(Player &player);

// combat logic when player encounters the final boss
void boss_combat(Player &player, Enemy &boss);

// remove the combat ui after combat is done
void clearCombatLog();

// combat logic when player encounters an enemy
void encounter_combat(Player &player, Enemy &enemy);

// randomly roll an item to drop after player defeats enemy
void random_lootdrop(Player &player, Enemy enemy);

#endif