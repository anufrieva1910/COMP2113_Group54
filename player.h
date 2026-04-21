#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

//creates a new player, can choose a name and difficulty
//difficulty determines the starting stats
void initPlayer(Player &player, std::string name, Difficulty difficulty);

//adds an item
//does nothing if inventory is full
void addItem(Player &player, Item *item);

//removes item at a given index
//shifts everything after it left, frees the memory
void removeItem(Player &player, int index);

void takeDamage(Player &player, int damage);

void heal(Player &player, int amount);

bool isDead(const Player &player);

#endif