#include <iostream>
#include <string>
#include <cstdlib>
#include "combat.h"
using namespace std;

int calculateDamage(int attack, int defense) {
    int damage=attack-defense;
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

void removeItem(Player &player, int itemNo) {
    for (int i=itemNo-1; i<player.inventorySize-1; i++) {
        player.inventory[i]=player.inventory[i+1];
    }
    player.inventory[player.inventorySize-1]=nullptr;
    player.inventorySize--;
}

void useItem(Player &player) {
    //pops up inventory menu
    int itemNo;
    cin >> itemNo;
    if (itemNo>0 && itemNo<=player.inventorySize) {
        Item * used_item;
        used_item=player.inventory[itemNo-1];
        if (used_item->isWeapon) {
            player.attack+=used_item->value;
        }
        else {
            player.hp+=used_item->value;
            if (player.hp>player.maxHp) {
                player.hp=player.maxHp;
            }
        }
        cout << player.name << " used " << used_item->name << endl;
        removeItem(player, itemNo);
    }
    else {
        cout << "item not found" << endl;
    }
}

void encounter_combat(Player &player, Enemy &enemy) {
    string turn;
    bool flee=false;
    randomize_start(turn);
    while(player.hp>0 and enemy.hp>0) {
        if (turn=="player") {
            int move;
            cin >> move;
            switch (move) {
                case 1:
                    int damage=calculateDamage(player.attack,enemy.defense);
                    enemy.hp-=damage;
                    break;
                case 2:
                    useItem(player);
                    break;
                case 3:
                    int random_flee=rand()%2;
                    if (random_flee) {
                        flee=true;
                        cout << "successfully fleed" << endl;
                    }
                    else {
                        cout << "failed to flee" << endl;
                    }
                    break;
                default :
                    cout << "Invalid input" << endl;
                    break;
            }
        }
        else if (turn=="enemy") {
            int damage=calculateDamage(enemy.attack,player.defense);
            player.hp-=damage;
        }
        turn_change(turn);
        if (flee) {
            break;
        }
    }
    if (player.hp<0) {
        //game over
    }
    else {
        //enemy defeated ui
        //back to map ui
    }
}

