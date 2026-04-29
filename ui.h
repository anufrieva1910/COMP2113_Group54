#ifndef UI_H
#define UI_H

#include "types.h"
#include <string>

// typewriter effect. prints text one character at a time
// delayMs controls speed (15 = fast, 25 = atmospheric)
void typewrite(const string &text, int delayMs = 15);

// title screen : Bastet and Golden Hound on either side, game name in middle
char showTitleScreen();

// difficulty selection 
Difficulty showDifficultySelect();

// name entry 
string showNameEntry();

// HUD 
void showHUD(const Player &player);

// floor entry 
void showFloorEntry(int floorNum, const LoreFragment &fragment);

// map 
void showMap(const Floor &floor);

// combat screen 
void showCombatScreen(const Player &player, const Enemy &enemy, const string &enemyArt);

// combat log 
void addCombatLog(const string &line);
void showCombatLog();

// lore screen 
// shows cipher prompt at the bottom
void showLoreScreen(const LoreFragment &fragment, bool cipherAvailable);

// inventory screen 
int showInventory(const Player &player);

// game over screen
void showGameOver(const Player &player);

// victory screen 
void showVictory(const Player &player);

// leaderboard screen
void showLeaderboard();

// clears the terminal
void clearScreen();

// ASCII art strings for each enemy 
string getEnemyArt(const string &enemyName);

#endif