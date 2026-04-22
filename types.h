#ifndef TYPES_H
#define TYPES_H

#include <string>

const int MAX_INVENTORY = 10;
const int MAX_ENEMIES = 20;
const int TOTAL_FLOORS = 10;

enum Difficulty { EASY, NORMAL, HARD };
enum RoomType { EMPTY, MONSTER, TREASURE, TRAP, LORE, HIDDEN, PUZZLE, EXIT };
enum ItemType { WEAPON, SHIELD, POTION, LOOT };

//a single item; weapon or consumable
struct Item {
    ItemType type;
    std::string name;
    int value; //atk for weapons, hp heal for consumables
};

//a lore fragment; can be found in lore rooms
struct LoreFragment {
    int floorNumber;
    std::string title;
    std::string text;
};

//one room in the dungeon grid
struct Room {
    RoomType type;
    bool visited;
    bool revealed; //hidden rooms start false, revealed when player searches
    int enemyIndex; //index into floors enemy array, -1 if no enemy
    int loreIndex; //index into floors lore array, -1 if no lore
    int gold; //amount of gold in treasure rooms, 0 otherwise
};

//an enemy
struct Enemy {
    std::string name;
    int hp;
    int maxHp;
    int attack;
    int defense;
    bool isBoss;
    int phase; //for boss fights; changes at hp thresholds
};

//the player; persists across floors
struct Player {
    std::string name;
    int hp;
    int maxHp;
    int attack;
    int defense;
    int gold;
    int floor; //current floor number
    Difficulty difficulty;
    Item* inventory[MAX_INVENTORY];
    int inventorySize;
    bool loreFound[TOTAL_FLOORS]; //tracks if lore for each floor has been found
};

//a single floor of the dungeon
struct Floor {
    int width;
    int height;
    Room** grid; //2D array of rooms
    Enemy** enemies; //array of enemies on this floor
    int enemyCount;
    int playerX; //player's current x position on the grid
    int playerY; //player's current y position on the grid
    int floorNumber;
};

struct SaveData {
    std::string playerName;
    int hp;
    int maxHp;
    int attack;
    int defense;
    int gold;
    int floor;
    Difficulty difficulty;
};

#endif