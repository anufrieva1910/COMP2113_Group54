#ifndef DUNGEON_H
#define DUNGEON_H

#include "types.h"

// Dungeon grid dimensions
const int FLOOR_WIDTH  = 20;
const int FLOOR_HEIGHT = 12;

// Allocates and returns a new floor with randomised rooms
// Caller must free with freeFloor()
Floor* generateFloor(int floorNumber, Difficulty difficulty);

// Frees all memory allocated by generateFloor()
void freeFloor(Floor* floor);

// Checks adjacent rooms for hidden passages and reveals them
// Returns true if at least one was found
bool revealHiddenPassage(Floor* floor, int x, int y);

// Prints the dungeon grid to the terminal
void renderMap(const Floor* floor, int playerX, int playerY);

// Returns pointer to room at (x,y), or nullptr if out of bounds
Room* getRoom(Floor* floor, int x, int y);

// Returns true if (x,y) is a valid position the player can move to
bool isValidMove(const Floor* floor, int x, int y);

// Sets room at (x,y) as visited
void markVisited(Floor* floor, int x, int y);

#endif