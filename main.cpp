#include <iostream>
#include <cstdlib>
#include <ctime>
#include "types.h"
#include "player.h"
#include "dungeon.h"
#include "combat.h"
#include "enemy.h"
#include "puzzle.h"
#include "ui.h"
#include "fileio.h"
#include "items.h"

using namespace std;

int main() {
    srand(time(NULL));

    // load lore fragments from file once at startup
    int loreCount = 0;
    LoreFragment* loreFragments = loadLoreFragments(loreCount);

    bool running = true;
    while (running) {

        char titleChoice = showTitleScreen();

        if (titleChoice == 'Q') {
            running = false;
            break;
        }

        if (titleChoice == 'S') {
            showLeaderboard();
            continue;
        }

        Player player;
        string difficultyStr;

        if (titleChoice == 'L') {
            // load existing save
            if (!saveExists()) {
                cout << RED << "  No save file found." << RESET << endl;
                cin.ignore(10000, '\n');
                continue;
            }
            if (!loadGame(player, difficultyStr)) {
                cout << RED << "  Save file corrupted." << RESET << endl;
                cin.ignore(10000, '\n');
                continue;
            }
            // convert string to enum
            if (difficultyStr == "EASY")      player.difficulty = EASY;
            else if (difficultyStr == "HARD") player.difficulty = HARD;
            else                              player.difficulty = NORMAL;

        } else {
            // new game
            Difficulty diff = showDifficultySelect();
            string name = showNameEntry();
            initPlayer(player, name, diff);

            if (diff == EASY)        difficultyStr = "EASY";
            else if (diff == HARD)   difficultyStr = "HARD";
            else                     difficultyStr = "NORMAL";
        }

        bool gameOver = false;
        bool won = false;

        while (!gameOver && !won) {

            // generate this floor
            Floor* floor = generateFloor(player.floor, player.difficulty);

            // show floor entry with lore fragment header
            if (loreFragments != nullptr && player.floor <= loreCount) {
                showFloorEntry(player.floor, loreFragments[player.floor - 1]);
            }

            bool floorDone = false;

            while (!floorDone && !gameOver && !won) {

                clearScreen();
                showHUD(player);
                renderMap(floor, floor->playerX, floor->playerY);

                cout << "  > ";
                char input;
                cin >> input;
                cin.ignore(10000, '\n');

                // inventory
                if (input == 'i') {
                    int result = showInventory(player);
                    if (result >= 0 && result < player.inventorySize) {
                        Item* used_item = player.inventory[result];
                        switch (used_item->type) {
                            case WEAPON: player.attack += used_item->value; break;
                            case SHIELD: player.defense += used_item->value; break;
                            case POTION:
                                player.hp += used_item->value;
                                if (player.hp > player.maxHp) player.hp = player.maxHp;
                                break;
                            case LOOT: player.gold += used_item->value; break;
                            default: break;
                        }
                        removeItem(player, result);
                    } else if (result <= 2) {
                        removeItem(player, -(result + 2)); // drop item
                    }
                    continue;
                }

                // save and quit
                if (input == 'x') {
                    if (player.difficulty == HARD) {
                        cout << RED << "  Hard mode: no saving." << RESET << endl;
                        cin.ignore(10000, '\n');
                    } else {
                        saveGame(player, difficultyStr);
                        cout << GREEN << "  Game saved. Goodbye." << RESET << endl;
                        gameOver = true;
                    }
                    continue;
                }

                // movement
                int nx = floor->playerX;
                int ny = floor->playerY;
                if (input == 'w')      ny--;
                else if (input == 's') ny++;
                else if (input == 'a') nx--;
                else if (input == 'd') nx++;
                else continue;

                if (!isValidMove(floor, nx, ny)) continue;

                floor->playerX = nx;
                floor->playerY = ny;
                markVisited(floor, nx, ny);
                revealHiddenPassage(floor, nx, ny);

                Room* room = getRoom(floor, nx, ny);
                if (!room) continue;

                // handle room
                switch (room->type) {

                    case MONSTER: {
                        Enemy enemy = getEnemyForFloor(player.floor);
                        encounter_combat(player, enemy);
                        if (isDead(player)) gameOver = true;
                        else room->type = EMPTY;
                        break;
                    }

                    case TREASURE: {
                        int idx = rand() % NUM_ITEMS;
                        Item* loot = new Item(ITEMS_LIST[idx]);
                        addItem(player, loot);
                        player.gold += 30;
                        cout << " Press Enter to open the treasure chest...";
                        cin.ignore(10000, '\n');
                        cout << GREEN << "  You found " << loot->name
                             << " and 30 gold!" << RESET << endl;
                        cin.ignore(10000, '\n');
                        room->type = EMPTY;
                        break;
                    }

                    case TRAP: {
                        takeDamage(player, 20);
                        cout << " Something feels wrong..." << endl;
                        cin.ignore(10000, '\n');
                        cout << RED << "  A trap! You take 20 damage." << RESET << endl;
                        cin.ignore(10000, '\n');
                        if (isDead(player)) gameOver = true;
                        else room->type = EMPTY;
                        break;
                    }

                    case LORE: {
                        // show lore if not already found on this floor
                        if (player.floor <= 9 && !player.loreFound[player.floor - 1]
                            && loreFragments != nullptr) {
                            player.loreFound[player.floor - 1] = true;
                            player.loreCount++;
                            showLoreScreen(loreFragments[player.floor - 1], true);

                            bool cipherOpensExit = false;
                            PuzzleResult cr = runLoreCipher(player.floor,
                                player.difficulty, cipherOpensExit);
                            if (cr == SKIPPED) {
                                int penalty = getSkipPenalty(player.difficulty);
                                player.gold = (player.gold >= penalty)
                                    ? player.gold - penalty : 0;
                            }
                        }
                        room->type = EMPTY;
                        break;
                    }

                    case PUZZLE: {
                        PuzzleResult pr = runPuzzle(player.floor, player.difficulty);
                        if (pr == SOLVED) {
                            int idx = rand() % NUM_ITEMS;
                            Item* reward = new Item(ITEMS_LIST[idx]);
                            addItem(player, reward);
                            cout << GREEN << "  You found " << reward->name
                                 << "!" << RESET << endl;
                            cin.ignore(10000, '\n');
                        } else if (pr == SKIPPED) {
                            int penalty = getSkipPenalty(player.difficulty);
                            player.gold = (player.gold >= penalty)
                                ? player.gold - penalty : 0;
                        }
                        room->type = EMPTY;
                        break;
                    }

                    case EXIT: {
                        if (player.floor == TOTAL_FLOORS) {
                            // boss fight on floor 10
                            Enemy cerberus = getEnemyForFloor(10);
                            encounter_combat(player, cerberus);
                            if (isDead(player)) {
                                gameOver = true;
                            } else {
                                won = true;
                            }
                        } else {
                            // next floor
                            player.floor++;
                            floorDone = true;
                        }
                        break;
                    }

                    default:
                        break;
                }
            }

            freeFloor(floor);
        }

        // game over
        if (gameOver) {
            if (player.difficulty == HARD) deleteSave();
            showGameOver(player);

            char retry;
            cin >> retry;
            cin.ignore(10000, '\n');
            // R retries, anything else goes back to title
        }

        // victory
        if (won) {
            deleteSave();
            showVictory(player);

            // save score to leaderboard
            ScoreEntry entry;
            entry.playerName    = player.name;
            entry.score         = player.gold * player.floor;
            entry.floorsCleared = player.floor;
            entry.difficulty    = difficultyStr;
            saveScore(entry);

            showLeaderboard();
        }
    }

    // free lore fragments
    delete[] loreFragments;
    return 0;
}