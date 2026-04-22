#include <iostream>
#include <string>
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

void useItem(Player &player) {
    cout << player.name << " tried to use an item (not implemented yet)." << endl;
}

void encounter_combat(Player &player, Enemy &enemy) {
    string turn = "player";
    while(player.hp>0 and enemy.hp>0) {
        if (turn=="player") {
            int move;
            cin >> move;
            switch (move) {
                case 1:
                    int damage=calculateDamage(player.attack,enemy.defense);
                    enemy.hp-=damage;
                    cout << enemy.hp << endl;
                    break;
                case 2:
                    useItem(player);
                    break;
                case 3:
                    break;
                default :
                    cout << "Invalid input" << endl;
                    break;
            }
        }
        else if (turn=="enemy") {
            int damage=calculateDamage(enemy.attack,player.defense);
            player.hp-=damage;
            cout << player.hp << endl;
        }
        turn_change(turn);
    }
}

