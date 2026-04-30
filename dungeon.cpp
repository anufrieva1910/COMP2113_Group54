#include "dungeon.h"
#include <iostream>
#include <cstdlib>
#include "ui.h"


// Allocates and returns a new floor with randomised rooms
// Caller must free with freeFloor()
Floor* generateFloor(int floorNumber, Difficulty difficulty) {
    Floor* floor = new Floor;
    floor->width       = FLOOR_WIDTH;
    floor->height      = FLOOR_HEIGHT;
    floor->floorNumber = floorNumber;
    floor->enemyCount  = 0;
    floor->enemies     = nullptr;

    floor->grid = new Room*[FLOOR_WIDTH];
    for (int x = 0; x < FLOOR_WIDTH; x++) {
        floor->grid[x] = new Room[FLOOR_HEIGHT];
        for (int y = 0; y < FLOOR_HEIGHT; y++) {
            floor->grid[x][y].type       = EMPTY;
            floor->grid[x][y].visited    = false;
            floor->grid[x][y].revealed   = false;
            floor->grid[x][y].enemyIndex = -1;
            floor->grid[x][y].loreIndex  = -1;
            floor->grid[x][y].gold       = 0;
        }
    }

    // Build a list of all interior walkable cells (not border, not corners)
    // then scatter rooms randomly across them
    const int ROOM_STEP = 4;
    const int ROOMS_X = (FLOOR_WIDTH  - 2) / ROOM_STEP;
    const int ROOMS_Y = (FLOOR_HEIGHT - 2) / ROOM_STEP;

    // carve rooms and corridors (keep the grid structure)
    for (int gx = 0; gx < ROOMS_X; gx++) {
        for (int gy = 0; gy < ROOMS_Y; gy++) {
            int rx = 1 + gx * ROOM_STEP;
            int ry = 1 + gy * ROOM_STEP;
            for (int dx = 0; dx < 2; dx++)
                for (int dy = 0; dy < 2; dy++)
                    floor->grid[rx + dx][ry + dy].type = EMPTY;
            if (gx + 1 < ROOMS_X) {
                int cx = rx + 2;
                floor->grid[cx][ry].type     = EMPTY;
                floor->grid[cx][ry + 1].type = EMPTY;
            }
            if (gy + 1 < ROOMS_Y) {
                int cy = ry + 2;
                floor->grid[rx][cy].type     = EMPTY;
                floor->grid[rx + 1][cy].type = EMPTY;
            }
        }
    }

    // Floor 10: boss floor — single Cerberus in centre, no exit, no lore
    if (floorNumber == 10) {
        int cx = FLOOR_WIDTH  / 2;
        int cy = FLOOR_HEIGHT / 2;
        floor->grid[cx][cy].type  = MONSTER;
        floor->playerX = 1;
        floor->playerY = 1;
        floor->grid[1][1].type    = EMPTY;
        floor->grid[1][1].visited = true;
        for (int dx = 0; dx < 2; dx++)
            for (int dy = 0; dy < 2; dy++)
                floor->grid[1 + dx][1 + dy].visited = true;
        return floor;
    }

    // Collect all candidate room anchor cells (top-left of each 2x2 room block)
    // excluding player start (gx==0,gy==0) and exit cell (last gx,gy)
    int candX[20], candY[20];
    int candCount = 0;
    for (int gx = 0; gx < ROOMS_X; gx++) {
        for (int gy = 0; gy < ROOMS_Y; gy++) {
            if (gx == 0 && gy == 0) continue;
            if (gx == ROOMS_X - 1 && gy == ROOMS_Y - 1) continue;
            candX[candCount] = 1 + gx * ROOM_STEP;
            candY[candCount] = 1 + gy * ROOM_STEP;
            candCount++;
        }
    }

    // Shuffle candidates using Fisher-Yates
    for (int i = candCount - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tx = candX[i]; candX[i] = candX[j]; candX[j] = tx;
        int ty = candY[i]; candY[i] = candY[j]; candY[j] = ty;
    }

    // 6 candidate slots total. Lore is placed FIRST so it is always guaranteed.
    // Counts per difficulty must sum to <= 6.
    // EASY:   1 lore + 2 enemies + 1 treasure + 1 puzzle + 1 trap  = 6
    // NORMAL: 1 lore + 2 enemies + 1 treasure + 1 puzzle + 1 trap  = 6
    // HARD:   1 lore + 3 enemies + 1 treasure + 0 puzzle + 1 trap  = 6
    int numLore     = 1;
    int numEnemies  = (difficulty == HARD) ? 3 : 2;
    int numTreasure = 1;
    int numPuzzle   = (difficulty == HARD) ? 0 : 1;
    int numTrap     = 1;

    // Place rooms into shuffled candidate slots — lore first, always guaranteed
    int idx = 0;
    auto place = [&](RoomType t, int n) {
        for (int k = 0; k < n && idx < candCount; k++, idx++)
            floor->grid[candX[idx]][candY[idx]].type = t;
    };

    place(LORE,     numLore);     // placed first — guaranteed slot
    place(MONSTER,  numEnemies);
    place(TREASURE, numTreasure);
    place(PUZZLE,   numPuzzle);
    place(TRAP,     numTrap);

    // Exit at far corner
    int exitX = 1 + (ROOMS_X - 1) * ROOM_STEP + 1;
    int exitY = 1 + (ROOMS_Y - 1) * ROOM_STEP + 1;
    floor->grid[exitX][exitY].type = EXIT;

    // Player start
    floor->playerX = 1;
    floor->playerY = 1;
    floor->grid[1][1].type    = EMPTY;
    floor->grid[1][1].visited = true;
    for (int dx = 0; dx < 2; dx++)
        for (int dy = 0; dy < 2; dy++)
            floor->grid[1 + dx][1 + dy].visited = true;

    return floor;
}

// Frees all memory allocated by generateFloor()
void freeFloor(Floor* floor) {
    if (!floor) return;
    if (floor->grid) {
        for (int x = 0; x < floor->width; x++)
            delete[] floor->grid[x];
        delete[] floor->grid;
    }
    if (floor->enemies) delete[] floor->enemies;
    delete floor;
}

// Checks adjacent rooms for hidden passages and reveals them
// Returns true if at least one was found
bool revealHiddenPassage(Floor* floor, int x, int y) {
    int dx[] = { 0, 0, 1, -1 };
    int dy[] = { -1, 1, 0, 0 };
    bool found = false;
    for (int d = 0; d < 4; d++) {
        int nx = x + dx[d];
        int ny = y + dy[d];
        if (nx < 0 || nx >= floor->width || ny < 0 || ny >= floor->height) continue;
        Room& neighbour = floor->grid[nx][ny];
        if (neighbour.type == HIDDEN && !neighbour.revealed) {
            neighbour.revealed = true;
            found = true;
        }
    }
    return found;
}

// Returns pointer to room at (x,y), or nullptr if out of bounds
Room* getRoom(Floor* floor, int x, int y) {
    if (!floor || x < 0 || x >= floor->width || y < 0 || y >= floor->height)
        return nullptr;
    return &floor->grid[x][y];
}

// Returns true if (x,y) is a valid position the player can move to
bool isValidMove(const Floor* floor, int x, int y) {
    if (!floor || x <= 0 || x >= floor->width - 1 || y <= 0 || y >= floor->height - 1)
        return false;
    const Room& r = floor->grid[x][y];
    if (r.type == HIDDEN && !r.revealed) return false;
    return true;
}

void markVisited(Floor* floor, int x, int y) {
    Room* r = getRoom(floor, x, y);
    if (r) r->visited = true;

     // Reveal surrounding rooms so the map fills in as player explores
    int dx[] = { 0, 0, 1, -1, 1, 1, -1, -1, 2, -2, 0, 0 };
    int dy[] = { 1, -1, 0, 0, 1, -1, 1, -1, 0, 0, 2, -2 };
    for (int d = 0; d < 12; d++) {
        Room* neighbour = getRoom(floor, x + dx[d], y + dy[d]);
        if (neighbour && !neighbour->visited)
            neighbour->visited = true;
    }
}

// Returns the character symbol for a room on the map
static std::string roomSymbol(const Room& room, bool isPlayer) {
    if (isPlayer)                        return YELLOW + std::string("@") + RESET;
    if (!room.visited && !room.revealed) return "#";
    switch (room.type) {
        case MONSTER:  return RED + std::string("E") + RESET;
        case TREASURE: return GREEN + std::string("T") + RESET;
        case TRAP:     return DARK_GREY + std::string("^") + RESET;
        case LORE:     return CYAN + std::string("L") + RESET;
        case EXIT:     return BOLD + std::string(">") + RESET;
        case PUZZLE:   return MAGENTA + std::string("P") + RESET;
        case HIDDEN:   return room.revealed ? DARK_GREY + std::string("?") + RESET : "#";
        default:       return ".";
    }
}

// Prints the dungeon grid to the terminal
void renderMap(const Floor* floor, int playerX, int playerY) {
    if (!floor) return;
    std::cout << "\n";
    for (int y = 0; y < floor->height; y++) {
        std::cout << "  ";
        for (int x = 0; x < floor->width; x++) {
            if (x == 0 || x == floor->width - 1 || y == 0 || y == floor->height - 1) {
                std::cout << "# ";
                continue;
            }
            std::cout << roomSymbol(floor->grid[x][y], x == playerX && y == playerY) << ' ';
        }
        std::cout << "\n";
    }
    std::cout << "\n  @=you  E=enemy  L=lore  T=treasure  ^=trap  >=exit  P=puzzle  .=room  #=wall\n";
    std::cout << "  w=north  s=south  a=west  d=east  i=inventory  x=save & quit\n\n";
}