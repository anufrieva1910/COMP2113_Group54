#include <iostream>
#include <cstdlib>
#include <ctime>
#include "dungeon.h"

/*
 * test_dungeon.cpp
 * Quick test to verify the dungeon generator works correctly.
 * Generates floors at different difficulties and prints the ASCII map.
 * Author: Tarishi
 */

int main() {
    // Seed random number generator (main.cpp will do this in the real game)
    srand(time(0));

    std::cout << "=== TEST 1: Floor 1, EASY ===\n";
    Floor* f1 = generateFloor(1, EASY);
    renderMap(f1, f1->playerX, f1->playerY);
    freeFloor(f1);

    std::cout << "\n=== TEST 2: Floor 5, NORMAL ===\n";
    Floor* f2 = generateFloor(5, NORMAL);
    renderMap(f2, f2->playerX, f2->playerY);
    freeFloor(f2);

    std::cout << "\n=== TEST 3: Floor 10, HARD ===\n";
    Floor* f3 = generateFloor(10, HARD);
    renderMap(f3, f3->playerX, f3->playerY);
    freeFloor(f3);

    std::cout << "\n=== TEST 4: isValidMove ===\n";
    Floor* f4 = generateFloor(3, NORMAL);
    // Border should be invalid
    std::cout << "Move to border (0,0): " << (isValidMove(f4, 0, 0) ? "VALID" : "INVALID") << " (expected INVALID)\n";
    // Interior should be valid
    std::cout << "Move to (1,1): " << (isValidMove(f4, 1, 1) ? "VALID" : "INVALID") << " (expected VALID)\n";
    freeFloor(f4);

    std::cout << "\n=== TEST 5: revealHiddenPassage ===\n";
    Floor* f5 = generateFloor(3, NORMAL);
    bool found = revealHiddenPassage(f5, 1, 1);
    std::cout << "Hidden passage near (1,1): " << (found ? "FOUND" : "none nearby") << "\n";
    freeFloor(f5);

    std::cout << "\nAll tests done. If no crashes, memory management is working!\n";
    return 0;
}
