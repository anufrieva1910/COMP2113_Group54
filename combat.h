#ifndef COMBAT_H
#define COMBAT_H

#include "types.h"
#include "items.h"
#include <string>

int calculateDamage(int attack, int defense);

void turn_change(std::string &turn);

void randomize_start(std::string &turn);

void removeItem(Player &player, int itemNo);

void addItem(Player &player, Item item);

void useItem(Player &player);

void encounter_combat(Player &player, Enemy &enemy);

void random_lootdrop(Player &player, Enemy enemy);

#endif