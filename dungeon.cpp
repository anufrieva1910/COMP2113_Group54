#include "dungeon.h"
#include <iostream>
#include <cstdlib>   // rand()
#include <cstring>   // memset

/*
 * dungeon.cpp
 * Implements procedural floor generation and ASCII map rendering.
 * Author: Tarishi
 *
 * Design notes:
 *   - The dungeon is a 2D array of Room structs (FLOOR_WIDTH x FLOOR_HEIGHT).
 *   - The outer border (row 0, row H-1, col 0, col W-1) is always EMPTY
 *     with visited=false so it renders as '#' (wall). This naturally
 *     boxes the playable area.
 *   - Room type probabilities scale with floorNumber so later floors
 *     have more monsters and traps.
 *   - Hidden rooms are placed randomly and only reveal when the player
 *     searches (revealHiddenPassage).
 */

/* ---------------------------------------------------------------
 * Internal helper: assign a random room type for interior cells.
 * Inputs:  floorNumber (1-10), difficulty
 * Output:  a RoomType value (never EXIT — exit is placed separately)
 * --------------------------------------------------------------- */
static RoomType randomRoomType(int floorNumber, Difficulty difficulty) {
    /*
     * Probability bands (out of 100):
     *   0-39  : EMPTY
     *   40-54 : MONSTER   (widens on higher floors)
     *   55-64 : TREASURE
     *   65-74 : TRAP
     *   75-80 : LORE
     *   81-86 : HIDDEN
     *   87-93 : PUZZLE
     *   94-99 : EMPTY (padding)
     *
     * On HARD, monster band widens by 5; on EASY it shrinks by 5.
     */
    int roll = rand() % 100;

    int monsterCap = 54 + floorNumber;           // grows with depth
    if (difficulty == HARD) monsterCap += 5;
    if (difficulty == EASY) monsterCap -= 5;

    if (roll < 40)                    return EMPTY;
    if (roll <= monsterCap)           return MONSTER;
    if (roll <= monsterCap + 10)      return TREASURE;
    if (roll <= monsterCap + 20)      return TRAP;
    if (roll <= monsterCap + 25)      return LORE;
    if (roll <= monsterCap + 31)      return HIDDEN;
    if (roll <= monsterCap + 37)      return PUZZLE;
    return EMPTY;
}

/* ---------------------------------------------------------------
 * generateFloor
 * Inputs:  floorNumber (1-10), difficulty
 * Output:  heap-allocated Floor*; caller must call freeFloor()
 *
 * Algorithm:
 *   1. new Floor, new 2D grid.
 *   2. Fill every cell as EMPTY / unvisited.
 *   3. Assign random types to interior cells.
 *   4. Force-place EXIT at bottom-right corner region.
 *   5. Force-place at least one LORE room (story requirement).
 *   6. Set player start at (1,1).
 * --------------------------------------------------------------- */
Floor* generateFloor(int floorNumber, Difficulty difficulty) {
    Floor* floor = new Floor;
    floor->width       = FLOOR_WIDTH;
    floor->height      = FLOOR_HEIGHT;
    floor->floorNumber = floorNumber;
    floor->enemyCount  = 0;
    floor->enemies     = nullptr;

    /* Allocate 2D grid: floor->grid[x][y] */
    floor->grid = new Room*[FLOOR_WIDTH];
    for (int x = 0; x < FLOOR_WIDTH; x++) {
        floor->grid[x] = new Room[FLOOR_HEIGHT];
        for (int y = 0; y < FLOOR_HEIGHT; y++) {
            /* Initialise every room to safe defaults */
            floor->grid[x][y].type        = EMPTY;
            floor->grid[x][y].visited     = false;
            floor->grid[x][y].revealed    = false;
            floor->grid[x][y].enemyIndex  = -1;
            floor->grid[x][y].loreIndex   = -1;
            floor->grid[x][y].gold        = 0;
        }
    }

    /* Assign types to interior (non-border) cells */
    for (int x = 1; x < FLOOR_WIDTH - 1; x++) {
        for (int y = 1; y < FLOOR_HEIGHT - 1; y++) {
            floor->grid[x][y].type = randomRoomType(floorNumber, difficulty);
        }
    }

    /* Place EXIT near bottom-right corner.
     * Walk inward from (W-2, H-2) until we find a non-HIDDEN cell. */
    floor->grid[FLOOR_WIDTH - 2][FLOOR_HEIGHT - 2].type = EXIT;

    /* Guarantee at least one LORE room per floor (narrative requirement) */
    bool lorePlaced = false;
    for (int x = 1; x < FLOOR_WIDTH - 1 && !lorePlaced; x++) {
        for (int y = 1; y < FLOOR_HEIGHT - 1 && !lorePlaced; y++) {
            if (floor->grid[x][y].type == LORE) {
                lorePlaced = true;
            }
        }
    }
    if (!lorePlaced) {
        /* Force one in a random interior position */
        int lx = 1 + rand() % (FLOOR_WIDTH - 2);
        int ly = 1 + rand() % (FLOOR_HEIGHT - 2);
        floor->grid[lx][ly].type = LORE;
    }

    /* Set player start — top-left interior corner */
    floor->playerX = 1;
    floor->playerY = 1;
    floor->grid[1][1].type    = EMPTY;   // start is always safe
    floor->grid[1][1].visited = true;

    return floor;
}

/* ---------------------------------------------------------------
 * freeFloor
 * Input:   pointer to a Floor from generateFloor
 * Output:  none — frees all heap memory in correct order
 * --------------------------------------------------------------- */
void freeFloor(Floor* floor) {
    if (!floor) return;

    /* Free the 2D grid column by column */
    if (floor->grid) {
        for (int x = 0; x < floor->width; x++) {
            delete[] floor->grid[x];
        }
        delete[] floor->grid;
    }

    /* Free the top-level enemy list if present */
    if (floor->enemies) {
        delete[] floor->enemies;
    }

    delete floor;
}

/* ---------------------------------------------------------------
 * revealHiddenPassage
 * Inputs:  floor, x and y of the room being searched
 * Output:  true if at least one hidden neighbour was revealed
 *
 * Checks the four cardinal neighbours. If any are HIDDEN and
 * not yet revealed, sets revealed = true.
 * --------------------------------------------------------------- */
bool revealHiddenPassage(Floor* floor, int x, int y) {
    /* Direction offsets: N, S, E, W */
    int dx[] = { 0, 0, 1, -1 };
    int dy[] = { -1, 1, 0, 0 };

    bool found = false;
    for (int d = 0; d < 4; d++) {
        int nx = x + dx[d];
        int ny = y + dy[d];
        if (nx < 0 || nx >= floor->width)  continue;
        if (ny < 0 || ny >= floor->height) continue;

        Room& neighbour = floor->grid[nx][ny];
        if (neighbour.type == HIDDEN && !neighbour.revealed) {
            neighbour.revealed = true;
            found = true;
        }
    }
    return found;
}

/* ---------------------------------------------------------------
 * getRoom
 * Inputs:  floor, x, y
 * Output:  pointer to Room at (x, y), or nullptr if out of bounds
 * --------------------------------------------------------------- */
Room* getRoom(Floor* floor, int x, int y) {
    if (!floor)                       return nullptr;
    if (x < 0 || x >= floor->width)  return nullptr;
    if (y < 0 || y >= floor->height) return nullptr;
    return &floor->grid[x][y];
}

/* ---------------------------------------------------------------
 * isValidMove
 * Inputs:  floor, target x, y
 * Output:  true if within bounds AND (visited OR EMPTY or any
 *          accessible type) — border cells count as walls
 * --------------------------------------------------------------- */
bool isValidMove(const Floor* floor, int x, int y) {
    if (!floor)                       return false;
    if (x < 0 || x >= floor->width)  return false;
    if (y < 0 || y >= floor->height) return false;

    /* Border cells act as walls — player cannot enter them */
    if (x == 0 || x == floor->width - 1)  return false;
    if (y == 0 || y == floor->height - 1) return false;

    /* HIDDEN rooms can only be entered after revealed */
    const Room& r = floor->grid[x][y];
    if (r.type == HIDDEN && !r.revealed) return false;

    return true;
}

/* ---------------------------------------------------------------
 * markVisited
 * Inputs:  floor, x, y
 * Output:  none — sets room.visited = true
 * --------------------------------------------------------------- */
void markVisited(Floor* floor, int x, int y) {
    Room* r = getRoom(floor, x, y);
    if (r) r->visited = true;
}

/* ---------------------------------------------------------------
 * roomSymbol (internal helper)
 * Inputs:  room, isPlayer (true if player is standing here)
 * Output:  a single char representing the room in the map
 *
 * Key:
 *   @ = player    E = monster    L = lore      T = treasure
 *   ^ = trap      > = exit       P = puzzle    . = empty (visited)
 *   ? = hidden revealed          # = wall / unvisited / hidden
 * --------------------------------------------------------------- */
static char roomSymbol(const Room& room, bool isPlayer) {
    if (isPlayer)                              return '@';
    if (!room.visited && !room.revealed)       return '#';

    switch (room.type) {
        case MONSTER:   return 'E';
        case TREASURE:  return 'T';
        case TRAP:      return '^';
        case LORE:      return 'L';
        case EXIT:      return '>';
        case PUZZLE:    return 'P';
        case HIDDEN:    return room.revealed ? '?' : '#';
        case EMPTY:     return '.';
        default:        return '.';
    }
}

/* ---------------------------------------------------------------
 * renderMap
 * Inputs:  floor (const — read only), playerX, playerY
 * Output:  none — prints the ASCII grid to stdout
 *
 * Border cells always print as '#'.
 * Interior cells print their symbol if visited or revealed,
 * otherwise '#'.
 * --------------------------------------------------------------- */
void renderMap(const Floor* floor, int playerX, int playerY) {
    if (!floor) return;

    /* Top status bar */
    std::cout << "\n";
    for (int y = 0; y < floor->height; y++) {
        std::cout << "  ";                  // left margin
        for (int x = 0; x < floor->width; x++) {
            bool isPlayer = (x == playerX && y == playerY);

            /* Border cells are always walls */
            if (x == 0 || x == floor->width - 1 ||
                y == 0 || y == floor->height - 1) {
                std::cout << "# ";
                continue;
            }

            char sym = roomSymbol(floor->grid[x][y], isPlayer);
            std::cout << sym << ' ';
        }
        std::cout << "\n";
    }

    /* Legend */
    std::cout << "\n  @=you  E=enemy  L=lore  T=treasure  "
              << "^=trap  >=exit  P=puzzle  .=room  #=wall\n";
    std::cout << "  w=north  s=south  a=west  d=east  "
              << "i=inventory  x=save & quit\n\n";
}