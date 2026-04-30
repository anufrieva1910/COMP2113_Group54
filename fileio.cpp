#include "fileio.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdio>   // remove()

// ═══════════════════════════════════════════════════════════
//  Internal helpers
// ═══════════════════════════════════════════════════════════

// Ensure the data/ directory exists (best-effort; no external libs).
static void ensureDataDir() {
    // On Linux/macOS the directory is created by the Makefile.
    // This is a no-op fallback so the code compiles everywhere.
}

// ═══════════════════════════════════════════════════════════
//  Save / Load
// ═══════════════════════════════════════════════════════════

bool saveGame(const Player& player, const std::string& difficulty) {
    ensureDataDir();
    std::ofstream out(SAVE_FILE);
    if (!out.is_open()) {
        std::cerr << "[fileio] ERROR: Cannot open save file for writing.\n";
        return false;
    }

    // ── Header ──
    out << "CERBERUS_SAVE_V1\n";
    out << "difficulty=" << difficulty << "\n";

    // ── Player core stats ──
    out << "name="     << player.name     << "\n";
    out << "hp="       << player.hp       << "\n";
    out << "maxHp="    << player.maxHp    << "\n";
    out << "attack="   << player.attack   << "\n";
    out << "defense="  << player.defense  << "\n";
    out << "gold="     << player.gold     << "\n";
    out << "floor="    << player.floor    << "\n";
    out << "score="    << player.score    << "\n";

    // ── Inventory ──
    out << "inventorySize=" << player.inventorySize << "\n";
    for (int i = 0; i < player.inventorySize; ++i) {
        out << "item=" << player.inventory[i]->type
            << ","     << player.inventory[i]->name
            << ","     << player.inventory[i]->value << "\n";
    }

    // ── Lore fragments found ──
    out << "loreCount=" << player.loreCount << "\n";
    for (int i = 0; i < TOTAL_FLOORS; ++i) {
        out << "lore" << i << "=" << (player.loreFound[i] ? 1 : 0) << "\n";
    }

    out.close();
    return true;
}

bool loadGame(Player& player, std::string& difficulty) {
    std::ifstream in(SAVE_FILE);
    if (!in.is_open()) return false;

    std::string line;

    // Check header
    std::getline(in, line);
    if (line != "CERBERUS_SAVE_V1") {
        std::cerr << "[fileio] WARNING: Save file format unrecognised.\n";
        return false;
    }

    // Temporary inventory storage before heap allocation
    std::vector<Item> tempInventory;
    bool tempLore[TOTAL_FLOORS] = {};

    while (std::getline(in, line)) {
        if (line.empty()) continue;

        // Split "key=value"
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key   = line.substr(0, eq);
        std::string value = line.substr(eq + 1);

        if      (key == "difficulty")     difficulty       = value;
        else if (key == "name")           player.name      = value;
        else if (key == "hp")             player.hp        = std::stoi(value);
        else if (key == "maxHp")          player.maxHp     = std::stoi(value);
        else if (key == "attack")         player.attack    = std::stoi(value);
        else if (key == "defense")        player.defense   = std::stoi(value);
        else if (key == "gold")           player.gold      = std::stoi(value);
        else if (key == "floor")          player.floor     = std::stoi(value);
        else if (key == "score")          player.score     = std::stoi(value);
        else if (key == "loreCount")      player.loreCount = std::stoi(value);
        else if (key == "item") {
            // "type,name,value"
            std::istringstream ss(value);
            std::string part;
            Item it;
            std::getline(ss, part, ','); it.type     = static_cast<ItemType>(std::stoi(part));
            std::getline(ss, part, ','); it.name     = part;
            std::getline(ss, part, ','); it.value    = std::stoi(part);
            tempInventory.push_back(it);
        }
        else if (key.substr(0, 4) == "lore" && key != "loreCount") {
            // "loreN" where N is 0–9
            int idx = std::stoi(key.substr(4));
            if (idx >= 0 && idx < TOTAL_FLOORS)
                tempLore[idx] = (std::stoi(value) == 1);
        }
    }
    in.close();

    // ── Allocate heap arrays ──
    player.inventorySize     = static_cast<int>(tempInventory.size());
    for (int i = 0; i < player.inventorySize; ++i) {
        player.inventory[i] = new Item(tempInventory[i]);
    }

    for (int i = 0; i < TOTAL_FLOORS; ++i)
        player.loreFound[i] = tempLore[i];

    return true;
}

bool saveExists() {
    std::ifstream in(SAVE_FILE);
    return in.is_open();
}

void deleteSave() {
    if (std::remove(SAVE_FILE.c_str()) != 0)
        std::cerr << "[fileio] WARNING: Could not delete save file.\n";
}

// ═══════════════════════════════════════════════════════════
//  Leaderboard
// ═══════════════════════════════════════════════════════════

void saveScore(const ScoreEntry& entry) {
    // Load existing scores, append new one, sort, keep top N, rewrite.
    std::vector<ScoreEntry> scores = loadScores();
    scores.push_back(entry);

    std::sort(scores.begin(), scores.end(),
        [](const ScoreEntry& a, const ScoreEntry& b) {
            return a.score > b.score;
        });

    if (static_cast<int>(scores.size()) > MAX_LEADERBOARD_ENTRIES)
        scores.resize(MAX_LEADERBOARD_ENTRIES);

    ensureDataDir();
    std::ofstream out(SCORES_FILE);
    if (!out.is_open()) {
        std::cerr << "[fileio] ERROR: Cannot write leaderboard.\n";
        return;
    }

    for (const ScoreEntry& s : scores) {
        out << s.playerName   << ","
            << s.score        << ","
            << s.floorsCleared << ","
            << s.difficulty   << "\n";
    }
    out.close();
}

std::vector<ScoreEntry> loadScores() {
    std::vector<ScoreEntry> scores;
    std::ifstream in(SCORES_FILE);
    if (!in.is_open()) return scores;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string part;
        ScoreEntry e;
        std::getline(ss, part, ','); e.playerName    = part;
        std::getline(ss, part, ','); e.score         = std::stoi(part);
        std::getline(ss, part, ','); e.floorsCleared = std::stoi(part);
        std::getline(ss, part, ','); e.difficulty    = part;
        scores.push_back(e);
    }
    in.close();
    return scores;
}

void printLeaderboard() {
    std::vector<ScoreEntry> scores = loadScores();
    if (scores.empty()) {
        std::cout << "  No scores recorded yet.\n";
        return;
    }
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════════════╗\n";
    std::cout << "  ║            HIGH SCORES                       ║\n";
    std::cout << "  ╠══════╦══════════════╦════════╦═══════════════╣\n";
    std::cout << "  ║ Rank ║ Name         ║ Score  ║ Difficulty    ║\n";
    std::cout << "  ╠══════╬══════════════╬════════╬═══════════════╣\n";

    for (int i = 0; i < static_cast<int>(scores.size()); ++i) {
        const ScoreEntry& s = scores[i];
        // Simple printf-style formatting without iomanip
        char row[128];
        std::snprintf(row, sizeof(row),
            "  ║ %-4d ║ %-12s ║ %-6d ║ %-13s ║\n",
            i + 1,
            s.playerName.substr(0, 12).c_str(),
            s.score,
            s.difficulty.substr(0, 13).c_str());
        std::cout << row;
    }
    std::cout << "  ╚══════╩══════════════╩════════╩═══════════════╝\n\n";
}

// ═══════════════════════════════════════════════════════════
//  Lore fragments
// ═══════════════════════════════════════════════════════════

/*  Expected lore.txt format (one fragment per block, blank line separated):
 *
 *  ID=1
 *  FLOOR=1
 *  TITLE=The Noble God
 *  TEXT=The noble canine god, revered by all who dwelled in the valley below.
 *
 *  ID=2
 *  ...
 */
LoreFragment* loadLoreFragments(int& count) {
    std::ifstream in(LORE_FILE);
    count = 0;

    if (!in.is_open()) {
        std::cerr << "[fileio] WARNING: Cannot open lore file.\n";
        return nullptr;
    }

    // First pass — count fragments
    std::string line;
    int total = 0;
    while (std::getline(in, line))
        if (line.substr(0, 5) == "FLOOR") total++;

    if (total == 0) {
        return nullptr;
    }

    // Allocate
    LoreFragment* frags = new LoreFragment[total];

    // Second pass — parse
    in.clear();
    in.seekg(0);
    int idx = -1;
    bool inText = false;
    bool inWallArt = false;

    while (std::getline(in, line)) {
        if ((!inText &&line.empty()) || line[0] == '#') continue;
        if (!inText && line.substr(0, 5) == "FLOOR") {
            ++idx;
            frags[idx].floorNumber = std::stoi(line.substr(6));
            frags[idx].title = "";
            frags[idx].text  = "";
        } else if (!inText && line.substr(0, 4) == "TYPE") {
            // ignore 
        } else if (!inText && line.substr(0, 6) == "CIPHER") {
            // ignore 
        } else if (!inText && line.substr(0, 5) == "TITLE") {
            frags[idx].title = line.substr(6);
        } else if (line == "TEXT_START") {
            inText = true;
            inWallArt = false;
        } else if (line == "TEXT_END") {
            inText = false;
            inWallArt = false;
        } else if (inText) {
            if (line == "[WALL ART]") {
                inWallArt = true;
                continue;
            }
            if (line.substr(0, 13) == "[LOST SCHOLAR") {
                inWallArt = false;
                continue;
            }
            if (inWallArt) {
                if (!frags[idx].text.empty()) frags[idx].text += "\n";
                frags[idx].text += line;
            } else {
                if (!frags[idx].intro.empty()) frags[idx].intro += "\n";
                frags[idx].intro += line;
            }
        }
    }   

    in.close();
    count = total;
    return frags;
}

void printLoreFragment(const LoreFragment& frag) {
    std::cout << "\n";
    std::cout << "  ┌─────────────────────────────────────────┐\n";
    std::cout << "  │  ~ Ancient Wall Art ~                   │\n";
    std::cout << "  │                                         │\n";

    // Title line (truncate to fit box width of 41 chars)
    std::string title = frag.title.substr(0, 39);
    char titleLine[64];
    std::snprintf(titleLine, sizeof(titleLine), "  │  %-39s│\n", title.c_str());
    std::cout << titleLine;

    std::cout << "  │                                         │\n";

    // Word-wrap text to 39 chars per line
    std::string text = frag.text;
    while (!text.empty()) {
        std::string chunk;
        if (static_cast<int>(text.size()) <= 39) {
            chunk = text;
            text.clear();
        } else {
            // Find last space within 39 chars
            size_t cut = text.rfind(' ', 39);
            if (cut == std::string::npos) cut = 39;
            chunk = text.substr(0, cut);
            text  = text.substr(cut + 1);
        }
        char textLine[64];
        std::snprintf(textLine, sizeof(textLine), "  │  %-39s│\n", chunk.c_str());
        std::cout << textLine;
    }

    std::cout << "  │                                         │\n";
    std::cout << "  └─────────────────────────────────────────┘\n\n";
}
