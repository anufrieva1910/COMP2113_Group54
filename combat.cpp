#include <iostream>
#include <string>
#include <cstdlib>
#include "combat.h"
#include "items.h"
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

void removeItem(Player &player, int itemNo) {
    delete player.inventory[itemNo-1];
    for (int i=itemNo-1; i<player.inventorySize-1; i++) {
        player.inventory[i]=player.inventory[i+1];
    }
    player.inventory[player.inventorySize-1]=nullptr;
    player.inventorySize--;
}

void addItem(Player &player, Item item) {  
    Item *itemptr=new Item(item);
    player.inventory[player.inventorySize]=itemptr;
    player.inventorySize++;
}

void useItem(Player &player) {
    //pops up inventory menu
    int itemNo;
    cin >> itemNo;
    if (itemNo>0 && itemNo<=player.inventorySize) {
        Item * used_item;
        used_item=player.inventory[itemNo-1];
        switch (used_item->type) {
            case WEAPON:
                player.attack+=used_item->value;
                cout << player.name << " gained " << used_item->value << " attack from item " << used_item->name << endl;
                break;
            case SHIELD:
                player.defense+=used_item->value;
                cout << player.name << " gained " << used_item->value << " defense from item " << used_item->name << endl;
                break;
            case POTION:
                player.hp+=used_item->value;
                if (player.hp>player.maxHp) player.hp=player.maxHp;
                cout << player.name << " gained " << used_item->value << " hp from item " << used_item->name << endl;
                break;
            case LOOT:
                player.gold+=used_item->value;
                cout << player.name << " gained " << used_item->value << " gold from item " << used_item->name << endl;
                break;
            default:
                cout << "unknown item type" << endl;
        }
        removeItem(player, itemNo);
    }
    else {
        cout << "item not found" << endl;
    }
}

void random_lootdrop(Player &player, Enemy enemy) {
    int index=rand() % NUM_ITEMS;
    const Item &loot = ITEMS_LIST[index];
    addItem(player,loot);
    cout << "You found " << loot.name << " from " << enemy.name << endl;
}

void boss_combat(Player &player, Enemy &enemy) {
    //to be designed
}

void encounter_combat(Player &player, Enemy &enemy) {
    if (enemy.isBoss) {
        boss_combat(player, enemy);
    }
    string turn;
    bool flee=false;
    randomize_start(turn);
    while(player.hp>0 and enemy.hp>0) {
        if (turn=="player") {
            int move;
            cin >> move;
            switch (move) {
                case 1: {
                    int damage=calculateDamage(player.attack,enemy.defense);
                    enemy.hp-=damage;
                    break;
                }
                case 2: {
                    useItem(player);
                    break;
                }
                case 3: {
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
            break;
        }
    }
    if (player.hp<0) {
        //game over
    }
    else {
        random_lootdrop(player, enemy);
        //enemy defeated ui
        //back to map ui
    }
}

