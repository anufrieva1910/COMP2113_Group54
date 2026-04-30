#include <iostream>
#include <string>
#include <cstdlib>
#include "combat.h"
#include "items.h"
#include "player.h"
#include "enemy.h"
#include "ui.h"
using namespace std;

int calculateDamage(int attack, int defense) {
    int damage;
    damage=attack-defense;
    if (damage<=0) {
        return 1;
    }
    else {
        return damage;
    }
}

void turn_change(string &turn) {
    if (turn=="player") {
        turn="enemy";
    }
    else {
        turn="player";
    }
}

void randomize_start(string &turn) {
    turn= (rand()%2) ? "player" : "enemy";
}


void useItem(Player &player) {
    //pops up inventory menu
    int result = showInventory(player);
    if (result >= 0 && result < player.inventorySize) {
        Item* used_item = player.inventory[result];
        switch (used_item->type) {
            case WEAPON: 
                player.attack += used_item->value; 
                cout << GREEN << player.name << " equipped " << used_item->name << " (+" << used_item->value << " ATK)" << RESET << endl;
                break;
            case SHIELD: 
                player.defense += used_item->value; 
                cout << GREEN << player.name << " equipped " << used_item->name << " (+" << used_item->value << " DEF)" << RESET << endl;
                break;
            case POTION:
                player.hp += used_item->value;
                if (player.hp > player.maxHp) player.hp = player.maxHp;
                cout << GREEN << player.name << " used " << used_item->name << " (+" << used_item->value << " HP)" << RESET << endl;
                break;
            case LOOT:
                player.gold += used_item->value;
                cout << GREEN << player.name << " sold " << used_item->name << " (+" << used_item->value << " Gold)" << RESET << endl;
                break;
            default:
                cout << "Unknown item." << endl;
        }
        removeItem(player, result);
    }
}

void random_lootdrop(Player &player, Enemy enemy) {
    int index=rand() % NUM_ITEMS;
    const Item &loot = ITEMS_LIST[index];
    Item *itemPtr = new Item(loot);
    addItem(player, itemPtr);
    cout << "You found " << loot.name << " from " << enemy.name << endl;
}

void boss_combat(Player &player, Enemy &boss) {
    string turn="enemy";
    bool phase_two=false, phase_three=false;
    while(player.hp>0 and boss.hp>0) {
        if (turn=="player") {
            showCombatScreen(player, boss, getEnemyArt(boss.name));
            char move;
            cin >> move;
            cin.ignore(10000, '\n');
            switch (move) {
                case 'A':
                case 'a': {
                    int damage=calculateDamage(player.attack,boss.defense);
                    boss.hp-=damage;
                    //attack boss ui
                    break;
                }
                case 'I':
                case 'i': {
                    useItem(player);
                    break;
                }
                
            }
            if (!phase_two && boss.hp<=boss.maxHp*2/3) {
                //boss phase 2 ui
                boss.phase=2;
                boss.attack=25;
                boss.defense=15;
                phase_two=true;
            }
            if(!phase_three && boss.hp<=boss.maxHp/3) {
                //boss phase 3 ui
                boss.phase=3;
                boss.attack=30;
                boss.defense=10;
                phase_three=true;
            }
        }
        else if (turn=="enemy") {
            int damage=calculateDamage(boss.attack,player.defense);
            player.hp-=damage;
        }
        turn_change(turn);
    }
    if (player.hp<=0) {
        //game over
    }
    else {
        //end game; win ui
    }
}

void encounter_combat(Player &player, Enemy &enemy) {
    if (enemy.isBoss) {
        boss_combat(player, enemy);
        return;
    }
    string turn;
    bool flee=false;
    randomize_start(turn);
    while(player.hp>0 and enemy.hp>0) {
        if (turn=="player") {
            showCombatScreen(player, enemy, getEnemyArt(enemy.name));
            char move;
            cin >> move;
            cin.ignore(10000, '\n');
            switch (move) {
                case 'A':
                case 'a': {
                    int damage=calculateDamage(player.attack,enemy.defense);
                    enemy.hp-=damage;
                    //attack ui
                    break;
                }
                case 'I':
                case 'i': {
                    useItem(player);
                    break;
                }
                case 'R':
                case 'r': {
                    int random_flee=rand()%2;
                    if (random_flee) {
                        flee=true;
                        cout << "Successfully fleed" << endl;
                    }
                    else {
                        cout << "Failed to flee" << endl;
                    }
                    break;
                }
                default: {
                    cout << "Invalid input" << endl;
                    break;
                }
            }
        }
        else if (turn=="enemy") {
            int damage=calculateDamage(enemy.attack,player.defense);
            player.hp-=damage;
        }
        turn_change(turn);
        if (flee) {
            //back to map ui
            break;
        }
    }
    if (player.hp<=0) {
        //game over
    }
    else if (!flee) {
        random_lootdrop(player, enemy);
        //enemy defeated ui
        //back to map ui
    }
}

