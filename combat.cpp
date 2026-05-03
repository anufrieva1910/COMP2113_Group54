#include <iostream>
#include <string>
#include <cstdlib>
#include "combat.h"
#include "items.h"
#include "player.h"
#include "enemy.h"
#include "ui.h"
using namespace std;

// defense reduces damage by 1/3 so enemies deal real damage but gear matters
int calculateDamage(int attack, int defense) {
    int damage = attack - (defense / 3);
    return (damage < 1) ? 1 : damage;
}

void turn_change(string &turn) {
    if (turn=="player") {
        turn="enemy";
    }
    else {
        turn="player";
    }
}

// starting turn is randomized between player and enemy
void randomize_start(string &turn) {
    turn= (rand()%2) ? "player" : "enemy";
}


void useItem(Player &player) {
    // pops up inventory menu
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
    int index = rand() % NUM_ITEMS;
    const Item &loot = ITEMS_LIST[index];
    Item *itemPtr = new Item(loot);
    addItem(player, itemPtr);
    addCombatLog(GREEN + string("Dropped: ") + loot.name + RESET);
}

void boss_combat(Player &player, Enemy &boss) {
    clearCombatLog();
    string turn = "enemy";  // Cerberus always strikes first
    bool phase_two   = false;
    bool phase_three = false;

    addCombatLog(RED + string("CERBERUS emerges from the darkness!") + RESET);

    while (player.hp > 0 && boss.hp > 0) {
        if (turn == "player") {
            showCombatScreen(player, boss, getEnemyArt(boss.name));
            char move;
            cin >> move;
            cin.ignore(10000, '\n');

            switch (move) {
                case 'A': case 'a': {
                    int dmg = calculateDamage(player.attack, boss.defense);
                    boss.hp -= dmg;
                    if (boss.hp < 0) boss.hp = 0;
                    addCombatLog("You strike Cerberus for " + to_string(dmg) + " damage. Cerberus HP: " + to_string(boss.hp));
                    break;
                }
                case 'I':
                case 'i': {
                    useItem(player);
                    continue;  // using an item does not end your turn
                }
                
                default:
                    addCombatLog("Invalid action.");
                    break;
            }

            if (!phase_two && boss.hp <= boss.maxHp * 2 / 3) {
                boss.phase   = 2;
                boss.attack  = (player.difficulty == HARD) ? 68 : 45;
                boss.defense = 10;
                phase_two    = true;
                addCombatLog(RED + string("Cerberus RAGES -- Phase 2! ATK surges!") + RESET);
            }
            if (!phase_three && boss.hp <= boss.maxHp / 3) {
                boss.phase   = 3;
                boss.attack  = (player.difficulty == HARD) ? 90 : 60;
                boss.defense = 5;
                phase_three  = true;
                addCombatLog(RED + string("Cerberus is FERAL -- Phase 3! Lethal!") + RESET);
            }
        } else {
            int dmg = calculateDamage(boss.attack, player.defense);
            player.hp -= dmg;
            if (player.hp < 0) player.hp = 0;
            addCombatLog(RED + string("Cerberus attacks for ") + to_string(dmg) + " damage! Your HP: " + to_string(player.hp) + RESET);
        }
        turn_change(turn);
    }

    // post-combat result screen
    clearScreen();
    cout << "=================================================================" << endl;
    cout << endl;
    if (player.hp <= 0) {
        cout << RED << "  Cerberus stands over you. The cave goes dark." << RESET << endl;
        cout << endl;
        cout << "  You fought bravely, but the beast was too powerful." << endl;
    } else {
        cout << GREEN << "  CERBERUS HAS FALLEN!" << RESET << endl;
        cout << endl;
        cout << "  The three heads go still. Silence fills the cave." << endl;
        cout << "  Something shifts in the air..." << endl;
    }
    cout << endl;
    cout << "=================================================================" << endl;
    cout << "  Press Enter to continue...";
    cin.ignore(10000, '\n');
    clearCombatLog();
}

void encounter_combat(Player &player, Enemy &enemy) {
    if (enemy.isBoss) {
        boss_combat(player, enemy);
        return;
    }

    clearCombatLog();
    string turn;
    bool flee = false;
    randomize_start(turn);

    addCombatLog("A " + enemy.name + " appears!");

    while (player.hp > 0 && enemy.hp > 0) {
        if (turn == "player") {
            showCombatScreen(player, enemy, getEnemyArt(enemy.name));
            char move;
            cin >> move;
            cin.ignore(10000, '\n');

            switch (move) {
                case 'A': case 'a': {
                    int dmg = calculateDamage(player.attack, enemy.defense);
                    enemy.hp -= dmg;
                    if (enemy.hp < 0) enemy.hp = 0;
                    addCombatLog("You attack for " + to_string(dmg) + " damage. "
                        + enemy.name + " HP: " + to_string(enemy.hp));
                    break;
                }
                case 'I':
                case 'i': {
                    useItem(player);
                    continue;  // using an item does not end your turn
                }
                case 'R': case 'r': {
                    if (rand() % 2) {
                        flee = true;
                        addCombatLog("You fled successfully.");
                    } else {
                        addCombatLog("Failed to flee!");
                    }
                    break;
                }
                default:
                    addCombatLog("Invalid action.");
                    break;
            }
        } else {
            int dmg = calculateDamage(enemy.attack, player.defense);
            player.hp -= dmg;
            if (player.hp < 0) player.hp = 0;
            addCombatLog(RED + enemy.name + " hits for " + to_string(dmg)
                + " damage. Your HP: " + to_string(player.hp) + RESET);
        }
        turn_change(turn);
        if (flee) break;
    }

    // post-combat result screen
    clearScreen();
    cout << "=================================================================" << endl;
    cout << endl;

    if (player.hp <= 0) {
        cout << RED << "  You have been defeated by " << enemy.name << "." << RESET << endl;
    } else if (flee) {
        cout << YELLOW << "  You escaped from " << enemy.name << "." << RESET << endl;
    } else {
        cout << GREEN << "  " << enemy.name << " has been defeated!" << RESET << endl;
        cout << endl;
        random_lootdrop(player, enemy);
        showCombatLog();
    }

    cout << endl;
    cout << "=================================================================" << endl;
    cout << "  Press Enter to return to the dungeon...";
    cin.ignore(10000, '\n');
    clearCombatLog();
}
