#ifndef UI_H
#define UI_H

#include "types.h"
#include <string>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define BOLD    "\033[1m"
#define DARK_GREY "\033[90m"
#define RESET   "\033[0m"

// typewriter effect. prints text one character at a time
// delayMs controls speed (15 = fast, 25 = atmospheric)
void typewrite(const std::string &text, int delayMs = 15);

// title screen : Bastet and Golden Hound on either side, game name in middle
char showTitleScreen();

// difficulty selection 
Difficulty showDifficultySelect();

// name entry 
std::string showNameEntry();

// HUD 
void showHUD(const Player &player);

// floor entry 
void showFloorEntry(int floorNum, const LoreFragment &fragment);

// map 
void showMap(const Floor &floor);

// combat screen 
void showCombatScreen(const Player &player, const Enemy &enemy, const std::string &enemyArt);

// combat log 
void addCombatLog(const std::string &line);
void showCombatLog();

// returns 'C' if cipher attempted, 'X' to leave
void showLoreScreen(const LoreFragment &fragment, bool cipherAvailable);
void clearCombatLog();

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
std::string getEnemyArt(const std::string &enemyName);

#endif