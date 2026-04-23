#ifndef PUZZLE_H
#define PUZZLE_H

#include "types.h"

enum PuzzleResult {
    SOLVED,
    FAILED,
    SKIPPED
};

PuzzleResult runPuzzle(int floorNum, Difficulty difficulty); // Randomized puzzle generation

PuzzleResult runLoreCipher(int floorNum, Difficulty difficulty, bool& cipherOpensExit); // Lore cipher puzzle


//Puzzle types
PuzzleResult runPressurePlate(Difficulty difficulty, bool hard);
PuzzleResult runNumberRiddle(Difficulty difficulty, bool hard);
PuzzleResult runSymbolMatch(Difficulty difficulty, bool hard);
PuzzleResult runLeverSequence(Difficulty difficulty, bool hard);

PuzzleResult runLockedDoor(); //checks if player has the key. incomplete

int getRetries(Difficulty difficulty); //allowed retries based on difficulty
int getSkipPenalty(Difficulty difficulty); //gold penalty for skipping

#endif