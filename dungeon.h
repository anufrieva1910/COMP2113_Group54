#ifndef DUNGEON_H
#define DUNGEON_H

#include "types.h"

/*
 * dungeon.h
 * Declares all dungeon generation and rendering functions.
 * Author: Tarishi
 *
 * Key responsibilities:
 *   - Generate a Floor (2D grid of Rooms) procedurally
 *   - Assign room types randomly (EMPTY, MONSTER, TREASURE, TRAP, LORE, HIDDEN, PUZZLE, EXIT)
 *   - Place hidden passageways between rooms
 *   - Render the dungeon as ASCII art to the terminal
 *   - Free dynamically allocated floor memory
 */

/* ---------------------------------------------------------------
 * generateFloor
 * Inputs:  floorNumber (1-10), difficulty level
 * Output:  pointer to a newly heap-allocated Floor struct
 *          (caller must free with freeFloor)
 * What it does:
 *   Allocates a FLOOR_WIDTH x FLOOR_HEIGHT grid of Rooms.
 *   Assigns room types using rand(), places the player start
 *   and exit, and seeds monsters / lore / treasure rooms
 *   based on floorNumber and difficulty.
 * --------------------------------------------------------------- */
Floor* generateFloor(int floorNumber, Difficulty difficulty);

/* ---------------------------------------------------------------
 * freeFloor
 * Input:   pointer to a Floor allocated by generateFloor
 * Output:  none (all heap memory is freed)
 * What it does:
 *   Deletes every enemy array in each room, then deletes the
 *   2D grid, then deletes the Floor itself.
 * --------------------------------------------------------------- */
void freeFloor(Floor* floor);

/* ---------------------------------------------------------------
 * revealHiddenPassage
 * Inputs:  floor, x and y coordinates of the room being searched
 * Output:  true if a hidden passage was found and revealed
 * What it does:
 *   Checks adjacent rooms for HIDDEN type; if found, sets
 *   revealed = true so the renderer shows it.
 * --------------------------------------------------------------- */
bool revealHiddenPassage(Floor* floor, int x, int y);

/* ---------------------------------------------------------------
 * renderMap
 * Inputs:  floor, player's current x and y position
 * Output:  none (prints ASCII map to stdout)
 * What it does:
 *   Prints a grid where each cell is one of:
 *     @  = player   E = enemy   L = lore   T = treasure
 *     ^  = trap     > = exit    . = empty  # = wall/hidden
 *   Only visited or revealed rooms are shown; others print as #.
 * --------------------------------------------------------------- */
void renderMap(const Floor* floor, int playerX, int playerY);

/* ---------------------------------------------------------------
 * getRoom
 * Inputs:  floor, x, y
 * Output:  pointer to Room at (x, y), or nullptr if out of bounds
 * --------------------------------------------------------------- */
Room* getRoom(Floor* floor, int x, int y);

/* ---------------------------------------------------------------
 * isValidMove
 * Inputs:  floor, target x and y
 * Output:  true if (x, y) is inside the grid and not a wall
 * --------------------------------------------------------------- */
bool isValidMove(const Floor* floor, int x, int y);

/* ---------------------------------------------------------------
 * markVisited
 * Inputs:  floor, x, y
 * Output:  none
 * What it does: sets room.visited = true for (x, y)
 * --------------------------------------------------------------- */
void markVisited(Floor* floor, int x, int y);

/* Constants used by dungeon generation */
const int FLOOR_WIDTH  = 20;
const int FLOOR_HEIGHT = 12;

#endif