#ifndef COMBAT_H
#define COMBAT_H

#include "types.h"
#include "items.h"
#include <string>
using namespace std;


int calculateDamage(int attack, int defense);

void turn_change(string &turn);

void randomize_start(string &turn);

void removeItem(Player &player, int itemNo);

void addItem(Player &player, Item item);

void useItem(Player &player);

void encounter_combat(Player &player, Enemy &enemy);

void random_lootdrop(Player &player, Enemy enemy);

#endif