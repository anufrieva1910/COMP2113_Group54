#ifndef COMBAT_H
#define COMBAT_H

#include "types.h"
#include <string>
using namespace std;


int calculateDamage(int attack, int defense);

void turn_change(string &turn);

void useItem(Player &player);

void encounter_combat(Player &player, Enemy &enemy);

#endif