#include "dungeon.h"
#include <iostream>
#include <cstdlib>
#include "ui.h"

//search if the exit is reachable by algorithm
bool isReachable(Floor* floor) {
    if (!floor) {
        return false;
    }
    const int startX = 1;
    const int startY = 1;
    const int exitX = FLOOR_WIDTH - 2;
    const int exitY = FLOOR_HEIGHT - 2;
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};
    bool visited[FLOOR_WIDTH][FLOOR_HEIGHT] = {false};
    int start = 0;
    int move = 0;
    int trialX[FLOOR_HEIGHT*FLOOR_WIDTH];
    int trialY[FLOOR_HEIGHT*FLOOR_WIDTH];
    trialX[move] = startX;
    trialY[move] = startY;
    move++;
    
    while (start<move) {
        int x = trialX[start];
        int y = trialY[start];
        start++;
        if (x==exitX && y==exitY) {
            return true;
        }
        
        for(int i=0; i<4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (nx < 0 || nx >= FLOOR_WIDTH || ny < 0 || ny >= FLOOR_HEIGHT) {
                continue;
            }
            if (nx == 0 || nx == FLOOR_WIDTH - 1 || ny == 0 || ny == FLOOR_HEIGHT - 1) {
                continue;
            }
            if (!visited[nx][ny] && floor->grid[nx][ny].type != HIDDEN) {
                visited[nx][ny] = true;
                trialX[move] = nx;
                trialY[move] = ny;
                move++;
            }
        }
    }
    return false;
} 



// Allocates and returns a new floor with randomised rooms
// Caller must free with freeFloor()
Floor* generateFloor(int floorNumber, Difficulty difficulty) {
    Floor* floor = new Floor;
    floor->width       = FLOOR_WIDTH;
    floor->height      = FLOOR_HEIGHT;
    floor->floorNumber = floorNumber;
    floor->enemyCount  = 0;
    floor->enemies     = nullptr;

    // default everything to walkable empty
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

    // randomly scatter walls across interior cells (~30% chance)
    bool pathExists = false;
    while (!pathExists) {
        for (int x = 1; x < FLOOR_WIDTH - 1; x++) {
            for (int y = 1; y < FLOOR_HEIGHT - 1; y++) {
                if (rand() % 10 < 3) {
                    floor->grid[x][y].type = HIDDEN;
                }
                else {
                    floor->grid[x][y].type = EMPTY;
                }
            }
        }
        pathExists = isReachable(floor);
    }

    // floor 10 — boss room
    if (floorNumber == TOTAL_FLOORS) {
        int cx = FLOOR_WIDTH / 2;
        int cy = FLOOR_HEIGHT / 2;
        floor->grid[cx][cy].type  = EXIT;
        floor->playerX = 1;
        floor->playerY = 1;
        floor->grid[1][1].type    = EMPTY;
        floor->grid[1][1].visited = true;
        return floor;
    }

    // collect all valid interior non-wall positions
    int candX[200], candY[200];
    int candCount = 0;
    for (int x = 2; x < FLOOR_WIDTH - 2; x++) {
        for (int y = 2; y < FLOOR_HEIGHT - 2; y++) {
            if (floor->grid[x][y].type == EMPTY)  {
                candX[candCount] = x;
                candY[candCount] = y;
                candCount++;
            }
        }
    }

    // Fisher-Yates shuffle
    for (int i = candCount - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tx = candX[i]; candX[i] = candX[j]; candX[j] = tx;
        int ty = candY[i]; candY[i] = candY[j]; candY[j] = ty;
    }

    // place rooms in shuffled positions
    int numEnemies  = (difficulty == HARD) ? 3 : 2;
    int numTreasure = 1;
    int numPuzzle   = (difficulty == HARD) ? 0 : 1;
    int numTrap     = 1;
    int numLore     = 1;

    int idx = 0;
    for (int k = 0; k < numLore     && idx < candCount; k++, idx++)
        floor->grid[candX[idx]][candY[idx]].type = LORE;
    for (int k = 0; k < numEnemies  && idx < candCount; k++, idx++)
        floor->grid[candX[idx]][candY[idx]].type = MONSTER;
    for (int k = 0; k < numTreasure && idx < candCount; k++, idx++)
        floor->grid[candX[idx]][candY[idx]].type = TREASURE;
    for (int k = 0; k < numPuzzle   && idx < candCount; k++, idx++)
        floor->grid[candX[idx]][candY[idx]].type = PUZZLE;
    for (int k = 0; k < numTrap     && idx < candCount; k++, idx++)
        floor->grid[candX[idx]][candY[idx]].type = TRAP;

    // exit at bottom-right area
    floor->grid[FLOOR_WIDTH - 2][FLOOR_HEIGHT - 2].type = EXIT;

    // player starts top-left
    floor->playerX = 1;
    floor->playerY = 1;
    floor->grid[1][1].type    = EMPTY;
    floor->grid[1][1].visited = true;

    // reveal start area
    for (int dx = 0; dx < 3; dx++)
        for (int dy = 0; dy < 3; dy++)
            if (floor->grid[1+dx][1+dy].type != HIDDEN)
                floor->grid[1+dx][1+dy].visited = true;

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
    if (r.type == HIDDEN) return false;
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
        if (neighbour && !neighbour->visited && neighbour->type != HIDDEN)
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
        case HIDDEN:   return "#";
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