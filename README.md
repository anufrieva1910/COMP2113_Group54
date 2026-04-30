# Cerberus's Cave

A story-driven terminal dungeon crawler for COMP2113 / ENGG1340, 2025-2026 Semester 2.

---

## Team Members

| Name | UID |
|------|-----|
| [Alex: Ekaterina Anufrieva] | [3036572217] |
| [Tarishi: Kundrapu Tarishi ] | [3036516990] |
| [Gavin: Leong Kuan Hou] | [3036662579] |
| [Kaivalya: Atyam Srilakshmi Kaivalya] | [3036519681] |
| [Mansi: Pamitipati Venkata Manaswini] | [3036519679] |

---

## Game Description

Cerberus's Cave is a text-based roguelike dungeon crawler played entirely in the Linux terminal. You play as a Cat Warrior descending a 10-floor cave to confront the dreaded Cerberus — a three-headed beast said to haunt its depths.

As you descend, you discover ancient wall carvings on each floor. The paintings tell a story: a once-noble Canine God, worshipped and beloved, slowly corrupted and twisted by a dark force. By the final floor, you face Cerberus — and discover the truth beneath the monster.

Each floor contains enemies to fight, traps to avoid, treasure chests, puzzles, and a lore room with an ancient carving and a lost scholar's diary. To leave a floor, you must solve a hidden cipher embedded in the wall art. The deeper you go, the harder the enemies and the darker the story.

---

## Features and Coding Requirements

### 1. Generation of Random Events

Room layouts are randomised every floor using a Fisher-Yates shuffle on all candidate room positions. Enemy encounters, item drops after combat, trap triggers, and loot contents are all determined using `rand()` seeded with `time(NULL)` at startup. The starting turn in combat (player or enemy first) is also randomised. This means no two playthroughs are the same.

**Relevant files:** `dungeon.cpp` (floor generation, room shuffle), `combat.cpp` (turn randomisation, loot drops), `main.cpp` (seed)

### 2. Data Structures for Storing Data

The game uses several custom structs defined in `types.h`:

- `Player` — stores name, HP, attack, defense, gold, floor number, difficulty, inventory (array of Item pointers), and lore discovery flags
- `Enemy` — stores name, HP, attack, defense, boss flag, and phase (for Cerberus)
- `Room` — stores room type, visited/revealed flags, and indices for enemies and lore
- `Floor` — stores a 2D grid of Rooms (`Room**`), player position, floor number, and enemy array
- `Item` — stores item type, name, and value
- `LoreFragment` — stores floor number, title, wall art text, and diary text

Arrays, enums (`RoomType`, `ItemType`, `Difficulty`), and the 2D room grid are all used throughout the game to organise and access game state.

**Relevant files:** `types.h`, `player.cpp`, `dungeon.cpp`, `enemy.cpp`

### 3. Dynamic Memory Management

The dungeon grid (`Room**`) is allocated on the heap each time a new floor is generated using `new`, and freed with `delete[]` when the player descends via `freeFloor()` in `dungeon.cpp`. Enemy arrays are heap-allocated similarly. Every item picked up by the player is heap-allocated as a `new Item`, and freed in `removeItem()` when used or dropped. Lore fragments are loaded into a heap-allocated array at startup and freed at program exit. Loaded save data also reconstructs inventory items on the heap.

**Relevant files:** `dungeon.cpp` (`generateFloor`, `freeFloor`), `player.cpp` (`addItem`, `removeItem`), `fileio.cpp` (`loadLoreFragments`, `loadGame`), `main.cpp`

### 4. File Input/Output

Three data files are used:

- `data/lore.txt` — loaded at startup into a `LoreFragment` array. Contains all 9 floor lore entries (wall art, diary, cipher answers, and the full ending sequence) in a structured format parsed by `loadLoreFragments()`.
- `data/save.txt` — written by `saveGame()` and read by `loadGame()`. Stores the full player state (stats, inventory with item types and values, lore discovery flags, difficulty, floor) so progress can be resumed across sessions. Save is deleted on death or victory.
- `data/scores.txt` — the leaderboard. `saveScore()` appends new entries, sorts by score, and keeps the top 10. `loadScores()` reads them back for display.

**Relevant files:** `fileio.cpp`, `fileio.h`

### 5. Program Code in Multiple Files

The project is split across 9 module pairs plus shared headers:

| File(s) | Responsibility |
|---------|---------------|
| `main.cpp` | Game loop, floor transitions, room event dispatch |
| `types.h` | All shared structs, enums, and constants |
| `player.h / player.cpp` | Player initialisation, inventory management, damage |
| `dungeon.h / dungeon.cpp` | Floor generation, map rendering, movement validation |
| `combat.h / combat.cpp` | Turn-based combat, boss phases, flee logic |
| `enemy.h / enemy.cpp` | Enemy definitions and difficulty scaling |
| `items.h / items.cpp` | Item list and constants |
| `puzzle.h / puzzle.cpp` | All puzzle types and the lore cipher |
| `ui.h / ui.cpp` | All screen rendering, combat log, floor entry, lore display |
| `fileio.h / fileio.cpp` | Save/load, leaderboard, lore file parsing |

All modules are compiled together via `Makefile`.

### 6. Multiple Difficulty Levels

Three difficulty levels are selectable at the start of a new game:

| | Easy | Normal | Hard |
|-|------|--------|------|
| Starting HP | 150 | 100 | 75 |
| Enemy attack | -20% | base | +50% |
| Enemies per floor | 2 | 2 | 3 |
| Treasure rooms | 1 | 1 | 1 |
| Puzzle rooms | 1 | 1 | 0 |
| Puzzle retries | 3 | 2 | 1 |
| Skip penalty | 5 gold | 15 gold | 30 gold |
| Saving | allowed | allowed | disabled (permadeath) |

**Relevant files:** `main.cpp`, `dungeon.cpp`, `enemy.cpp`, `puzzle.cpp`, `player.cpp`

---

## Non-Standard Libraries

No non-standard libraries are used. The game relies entirely on the C++ standard library (`iostream`, `fstream`, `sstream`, `string`, `vector`, `cstdlib`, `ctime`, `cstdio`, `cctype`, `unistd.h`). All of these are available on the CS department academy server without any additional installation.

---

## Compilation and Execution Instructions

### Requirements

- g++ with C++11 support
- Access to the HKU CS academy server (`academy11.cs.hku.hk`) or any Linux machine with g++

### Compile

Clone the repository and navigate to the project folder, then run:

```bash
make
```

This compiles all source files and produces an executable named `dungeon`.

To clean compiled files and recompile from scratch:

```bash
make clean
make
```

### Run

```bash
./dungeon
```

Make sure the `data/` directory is present in the same folder as the executable. It contains `lore.txt` (required for the game to load story content) and `scores.txt` (leaderboard, created automatically if missing).

### Controls

| Key | Action |
|-----|--------|
| `w` `a` `s` `d` | Move north / west / south / east |
| `i` | Open inventory |
| `x` | Save and quit (disabled on Hard) |
| `a` | Attack (in combat) |
| `i` | Use item (in combat) |
| `r` | Attempt to flee (in combat) |
