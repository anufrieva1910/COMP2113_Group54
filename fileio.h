#ifndef FILEIO_H
#define FILEIO_H

#include "structs.h"
#include <string>
#include <vector>

// ─────────────────────────────────────────────
//  File paths
// ─────────────────────────────────────────────
const std::string SAVE_FILE       = "data/save.txt";
const std::string SCORES_FILE     = "data/scores.txt";
const std::string LORE_FILE       = "data/lore.txt";

const int MAX_LEADERBOARD_ENTRIES = 10;

// ─────────────────────────────────────────────
//  Leaderboard entry
// ─────────────────────────────────────────────
struct ScoreEntry {
    std::string playerName;
    int score;
    int floorsCleared;
    std::string difficulty;
};

// ─────────────────────────────────────────────
//  Save / Load
// ─────────────────────────────────────────────

// Save player state to SAVE_FILE.
// Returns true on success, false on failure.
bool saveGame(const Player& player, const std::string& difficulty);

// Load player state from SAVE_FILE into player.
// Returns true on success, false if no save exists or file is corrupt.
bool loadGame(Player& player, std::string& difficulty);

// Returns true if a valid save file exists.
bool saveExists();

// Delete the save file (called on death or game completion).
void deleteSave();

// ─────────────────────────────────────────────
//  Leaderboard
// ─────────────────────────────────────────────

// Append a new score entry and keep the top MAX_LEADERBOARD_ENTRIES scores.
void saveScore(const ScoreEntry& entry);

// Load all leaderboard entries sorted by score descending.
std::vector<ScoreEntry> loadScores();

// Print the leaderboard to stdout (used by ui.cpp).
void printLeaderboard();

// ─────────────────────────────────────────────
//  Lore fragments
// ─────────────────────────────────────────────

// Load all lore fragments from LORE_FILE.
// Returns a heap-allocated array; caller must delete[].
LoreFragment* loadLoreFragments(int& count);

// Print a single lore fragment to stdout.
void printLoreFragment(const LoreFragment& frag);

#endif // FILEIO_H
