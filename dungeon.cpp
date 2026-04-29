#include "dungeon.h"
#include <iostream>
#include <cstdlib>
#include "ui.h"

// Returns a random room type based on floor depth and difficulty
static RoomType randomRoomType(int floorNumber, Difficulty difficulty) {
    int roll = rand() % 100;
    int monsterCap = 54 + floorNumber;
    if (difficulty == HARD) monsterCap += 5;
    if (difficulty == EASY) monsterCap -= 5;

    if (roll < 40)               return EMPTY;
    if (roll <= monsterCap)      return MONSTER;
    if (roll <= monsterCap + 10) return TREASURE;
    if (roll <= monsterCap + 20) return TRAP;
    if (roll <= monsterCap + 25) return LORE;
    if (roll <= monsterCap + 31) return HIDDEN;
    if (roll <= monsterCap + 37) return PUZZLE;
    return EMPTY;
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

    // Assign random types to interior cells
    for (int x = 1; x < FLOOR_WIDTH - 1; x++)
        for (int y = 1; y < FLOOR_HEIGHT - 1; y++)
            floor->grid[x][y].type = randomRoomType(floorNumber, difficulty);

    // Place exit at bottom-right
    floor->grid[FLOOR_WIDTH - 2][FLOOR_HEIGHT - 2].type = EXIT;

    // Guarantee at least one lore room per floor
    bool lorePlaced = false;
    for (int x = 1; x < FLOOR_WIDTH - 1 && !lorePlaced; x++)
        for (int y = 1; y < FLOOR_HEIGHT - 1 && !lorePlaced; y++)
            if (floor->grid[x][y].type == LORE) lorePlaced = true;

    if (!lorePlaced) {
        int lx = 1 + rand() % (FLOOR_WIDTH - 2);
        int ly = 1 + rand() % (FLOOR_HEIGHT - 2);
        floor->grid[lx][ly].type = LORE;
    }

    // Set player start and reveal surrounding rooms
    floor->playerX = 1;
    floor->playerY = 1;
    floor->grid[1][1].type    = EMPTY;
    floor->grid[1][1].visited = true;

    int dx[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
    int dy[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
    for (int d = 0; d < 8; d++) {
        int nx = 1 + dx[d];
        int ny = 1 + dy[d];
        if (nx > 0 && nx < FLOOR_WIDTH - 1 && ny > 0 && ny < FLOOR_HEIGHT - 1)
            floor->grid[nx][ny].visited = true;
    }

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
    int dx[] = { 0, 0, 1, -1, 1, 1, -1, -1 };
    int dy[] = { 1, -1, 0, 0, 1, -1, 1, -1 };
    for (int d = 0; d < 8; d++) {
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