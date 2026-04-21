#include <iostream>
#include "player.h"

using namespace std;

void initPlayer(Player &player, string name, Difficulty difficulty) {
    player.name = name;
    player.difficulty = difficulty;
    player.floor = 1;
    player.gold = 0;
    player.inventorySize = 0;
    player.attack = 10;
    player.defense = 5;

    //inventory reset
    for (int i = 0; i < MAX_INVENTORY; i++) {
        player.inventory[i] = nullptr;
    }

    for (int i = 0; i < TOTAL_FLOORS; i++) {
        player.loreFound[i] = false;
    }

    if (difficulty == EASY) {
        player.maxHp = 150;

    } else if (difficulty == NORMAL) {
        player.maxHp = 100;

    } else{
        player.maxHp = 75;
    }
    player.hp = player.maxHp;
}

void addItem(Player &player, Item *item) {
    if (player.inventorySize >= MAX_INVENTORY) {
        cout << "Inventory is full! Cannot pick up " << item->name << "." << endl;
        return;
    }
    player.inventory[player.inventorySize] = item;
    player.inventorySize++;
}

void removeItem(Player &player, int index) {
    delete player.inventory[index];

    for (int i = index; i < player.inventorySize - 1; i++) {
        player.inventory[i] = player.inventory[i + 1];
    }

    player.inventory[player.inventorySize - 1] = nullptr;
    player.inventorySize--;
}

void takeDamage(Player &player, int damage) {
    player.hp -= damage;
    if (player.hp < 0) {
        player.hp = 0;
    }
}

void heal(Player &player, int amount) {
    player.hp += amount;
    if (player.hp > player.maxHp) {
        player.hp = player.maxHp;
    }
}

bool isDead(const Player &player) {
    return player.hp <= 0;
}

