#include "puzzle.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

void toUpper(string& str) {
    for (int i = 0; i < (int)str.length(); i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

void printSeperator() {
    cout << "-----------------------------" << endl;
}


int getRetries(Difficulty difficulty) {
    if (difficulty == EASY) {
        return 3;
    } else if (difficulty == NORMAL) {
        return 2;
    } else { // HARD
        return 1;
    }
}

int getSkipPenalty(Difficulty difficulty) {
    if (difficulty == EASY) {
        return 5;
    } else if (difficulty == NORMAL) {
        return 15;
    } else { // HARD
        return 30;
    }
}

PuzzleResult runPuzzle(int floorNum, Difficulty difficulty) {
    if (floorNum >= 10) {
        return SKIPPED; // No puzzles on floor 10 and above
    }

    bool hard = (floorNum >= 6);
    int roll = rand() % 4; // Randomly select a puzzle type


    cout << endl;
    if (roll == 0) {
        return runPressurePlate(difficulty, hard);
    } else if (roll == 1) {
        return runNumberRiddle(difficulty, hard);
    } else if (roll == 2) {
        return runSymbolMatch(difficulty, hard);
    } else { // roll == 3
        return runLeverSequence(difficulty, hard);
    }
}

PuzzleResult runPressurePlate(Difficulty difficulty, bool hard) {
    int retries = getRetries(difficulty);

    int normalSeq[] = {2, 3, 1};
    int hardSeq[] = {4, 1, 6, 2, 5, 3};
    int* correctSeq = hard ? hardSeq : normalSeq;
    int seqLen = hard ? 6 : 3;

    printSeperator();
    cout << "  | PRESSURE PLATE PUZZLE                            |" << endl;
    printSeperator();
    cout << endl;
    cout << "  Stone tiles line the floor. A carving above reads:" << endl;
    cout << "  \"Step only where the god once walked.\"" << endl;
    cout << endl;

    cout << "  ";
    for (int i = 0; i < seqLen; i++) {
        cout << "[ " << (i + 1) << " ]";
        if (i < seqLen - 1) {
            cout << "  ";
        }
    }

    cout << endl << endl;

    if (hard)
        cout << "  Enter the order to step on the tiles (e.g. 4 1 6 2 5 3)." << endl;
    else
        cout << "  Enter the order to step on the tiles (e.g. 2 3 1)." << endl;

    cout << "  Type S to skip (costs " << getSkipPenalty(difficulty) << " gold)." << endl;
    cout << endl;

    while (retries > 0) {
        cout << "  (" << retries << (retries == 1 ? " attempt" : " attempts") << " left)  > ";

        string first;
        cin >> first;
        toUpper(first);

        if (first == "S") {
            cin.ignore(10000, '\n');
            cout << endl << "  You step back. The tiles reset." << endl << endl;
            return SKIPPED;
        }

        int playerSeq[6];
        playerSeq[0] = atoi(first.c_str());
        bool valid = true;

        for (int i = 1; i < seqLen && valid; i++) {
            if (!(cin >> playerSeq[i])) {
                valid = false;
                cin.clear();
            }
        }
        cin.ignore(10000, '\n');

        if (!valid) {
            cout << endl << "  Enter numbers only." << endl << endl;
            retries--;
            continue;
        }

        bool correct = true;
        for (int i = 0; i < seqLen; i++) {
            if (playerSeq[i] != correctSeq[i]) {
                correct = false;
                break;
            }
        }

        if (correct) {
            cout << endl << "  The tiles click into place. A passage opens." << endl << endl;
            return SOLVED;
        } else {
            retries--;
            if (retries > 0)
                cout << endl << "  A low rumble. Wrong order. The tiles reset." << endl << endl;
        }
    }

    cout << endl << "  The tiles lock in place. The puzzle seals itself." << endl << endl;
    return FAILED;
}

// number riddle. player reads a carved riddle and types a number answer
// 3 normal riddles and 3 hard riddles, one picked randomly each time
PuzzleResult runNumberRiddle(Difficulty difficulty, bool hard) {
    int retries = getRetries(difficulty);

    const char *normalRiddles[] = {
        "  \"I have cities, but no houses live in me.\n"
        "   I have mountains, but no trees grow.\n"
        "   I have roads, but no travellers walk.\n"
        "   How many legs do I stand on?\"",

        "  \"Three gods stood at the peak.\n"
        "   The first fell. Then there were two.\n"
        "   The second turned away. Then there was one.\n"
        "   The last looked down and saw his shadow split in three.\n"
        "   How many shadows were there in the end?\"",

        "  \"A canine has 4 legs and 1 tail.\n"
        "   Cerberus has 3 heads but is still 1 beast.\n"
        "   How many legs does Cerberus have?\""
    };
    int normalAnswers[] = {0, 3, 4};

    const char *hardRiddles[] = {
        "  \"I am always in front of you but can never be seen.\n"
        "   Subtract the legs of a loyal hound\n"
        "   from the heads of the beast he became.\n"
        "   What remains?\"",

        "  \"A scholar descends 1 floor per day.\n"
        "   Each floor he descends, he loses half his torches.\n"
        "   He starts with 16. By floor 5, how many does he have?\"",

        "  \"The god had 1 name. Bastet gave him another.\n"
        "   The warriors gave him a third.\n"
        "   His people forgot all of them.\n"
        "   How many names does he have now?\""
    };
    int hardAnswers[] = {-1, 1, 0};

    int roll = rand() % 3;
    const char *riddle = hard ? hardRiddles[roll] : normalRiddles[roll];
    int answer = hard ? hardAnswers[roll]  : normalAnswers[roll];

    printSeperator();
    cout << "  | NUMBER RIDDLE                                    |" << endl;
    printSeperator();
    cout << endl;
    cout << "  Words carved deep into the stone:" << endl << endl;
    cout << riddle << endl << endl;
    cout << "  Type S to skip (costs " << getSkipPenalty(difficulty) << " gold)." << endl;
    cout << endl;

    while (retries > 0) {
        cout << "  (" << retries << (retries == 1 ? " attempt" : " attempts") << " left)  > ";

        string input;
        cin >> input;
        cin.ignore(10000, '\n');
        toUpper(input);

        if (input == "S") {
            cout << endl << "  The words fade. You walk away." << endl << endl;
            return SKIPPED;
        }

        bool isNumeric = !input.empty() && (input[0] == '-' || isdigit((unsigned char)input[0]));
                         
        if (!isNumeric) {
            cout << endl << "  Enter a number." << endl << endl;
            retries--;
            continue;
        }

        if (atoi(input.c_str()) == answer) {
            cout << endl << "  The wall trembles. The passage shifts open." << endl << endl;
            return SOLVED;
        } else {
            retries--;
            if (retries > 0)
                cout << endl << "  The stone is silent. That is not the answer." << endl << endl;
        }
    }

    cout << endl << "  The riddle fades from the wall as if it was never there." << endl << endl;
    return FAILED;
}

// symbol match. player sees a sequence of symbols then reproduces it from memory
// normal: 3 symbols  hard: 5 symbols
PuzzleResult runSymbolMatch(Difficulty difficulty, bool hard) {
    int retries = getRetries(difficulty);
    int seqLen  = hard ? 5 : 3;

    // A=SUN  B=MOON  C=PAW  D=EYE  E=FLAME
    const string symbolNames[] = {"SUN", "MOON", "PAW", "EYE", "FLAME"};

    char normalSeq[] = {'C', 'A', 'D'};
    char hardSeq[]   = {'E', 'B', 'D', 'A', 'C'};
    char *correctSeq = hard ? hardSeq : normalSeq;

    printSeperator();
    cout << "  | SYMBOL MATCHING PUZZLE                           |" << endl;
    printSeperator();
    cout << endl;
    cout << "  Ancient carvings glow faintly on the wall." << endl;
    cout << "  Study the sequence carefully. It will not stay long." << endl;
    cout << endl;

    cout << "  [ ";
    for (int i = 0; i < seqLen; i++) {
        cout << symbolNames[correctSeq[i] - 'A'];
        if (i < seqLen - 1) cout << " ]  [ ";
    }
    cout << " ]" << endl << endl;

    cout << "  Press Enter when you have memorised it...";
    cin.ignore(10000, '\n');

    for (int i = 0; i < 15; i++) cout << endl;

    cout << "  The carvings go dark." << endl << endl;
    cout << "  Symbols:  A=SUN   B=MOON   C=PAW   D=EYE   E=FLAME" << endl << endl;
    cout << "  Reproduce the sequence (e.g. C A D)." << endl;
    cout << "  Type S to skip (costs " << getSkipPenalty(difficulty) << " gold)." << endl;
    cout << endl;

    while (retries > 0) {
        cout << "  (" << retries << (retries == 1 ? " attempt" : " attempts") << " left)  > ";

        string first;
        cin >> first;
        toUpper(first);

        if (first == "S") {
            cin.ignore(10000, '\n');
            cout << endl << "  The carvings go dark permanently." << endl << endl;
            return SKIPPED;
        }

        bool correct = (first.length() == 1 && first[0] == correctSeq[0]);

        for (int i = 1; i < seqLen; i++) {
            string token;
            if (!(cin >> token)) { correct = false; break; }
            toUpper(token);
            if (token.length() != 1 || token[0] != correctSeq[i])
                correct = false;
        }
        cin.ignore(10000, '\n');

        if (correct) {
            cout << endl << "  The carvings glow. A hidden door clicks open." << endl << endl;
            return SOLVED;
        } else {
            retries--;
            if (retries > 0)
                cout << endl << "  Wrong sequence. The carvings reset." << endl << endl;
        }
    }

    cout << endl << "  The carvings go dark permanently. The puzzle is sealed." << endl << endl;
    return FAILED;
}

// lever sequence. player pulls levers in the correct order
// normal: 4 levers  hard: 6 levers
PuzzleResult runLeverSequence(Difficulty difficulty, bool hard) {
    int retries   = getRetries(difficulty);
    int numLevers = hard ? 6 : 4;

    int normalOrder[] = {2, 4, 1, 3};
    int hardOrder[]   = {4, 1, 6, 2, 5, 3};
    int *correctOrder = hard ? hardOrder : normalOrder;

    const char *normalState = " DOWN   UP   DOWN   UP ";
    const char *hardState   = "  UP   DOWN   UP   DOWN   UP   DOWN";

    printSeperator();
    cout << "  | LEVER SEQUENCE PUZZLE                            |" << endl;
    printSeperator();
    cout << endl;
    cout << "  A row of iron levers lines the wall." << endl;
    cout << "  An inscription reads: \"Pull in the order the god descended.\"" << endl;
    cout << endl;

    cout << "  ";
    for (int i = 0; i < numLevers; i++)
        cout << "  [" << (i + 1) << "] ";
    cout << endl;
    cout << "  " << (hard ? hardState : normalState) << endl;
    cout << endl;

    if (hard)
        cout << "  Enter the pull order (e.g. 4 1 6 2 5 3)." << endl;
    else
        cout << "  Enter the pull order (e.g. 2 4 1 3)." << endl;

    cout << "  Type S to skip (costs " << getSkipPenalty(difficulty) << " gold)." << endl;
    cout << endl;

    while (retries > 0) {
        cout << "  (" << retries << (retries == 1 ? " attempt" : " attempts") << " left)  > ";

        string first;
        cin >> first;
        toUpper(first);

        if (first == "S") {
            cin.ignore(10000, '\n');
            cout << endl << "  The levers lock. The passage stays sealed." << endl << endl;
            return SKIPPED;
        }

        int playerOrder[6];
        playerOrder[0] = atoi(first.c_str());
        bool valid = true;

        for (int i = 1; i < numLevers && valid; i++) {
            if (!(cin >> playerOrder[i])) {
                valid = false;
                cin.clear();
            }
        }
        cin.ignore(10000, '\n');

        if (!valid) {
            cout << endl << "  Enter numbers only." << endl << endl;
            retries--;
            continue;
        }

        bool correct = true;
        for (int i = 0; i < numLevers; i++) {
            if (playerOrder[i] != correctOrder[i]) {
                correct = false;
                break;
            }
        }

        if (correct) {
            cout << endl << "  The levers click. A gate swings open ahead." << endl << endl;
            return SOLVED;
        } else {
            retries--;
            if (retries > 0)
                cout << endl << "  A grinding noise. The levers reset." << endl << endl;
        }
    }

    cout << endl << "  The levers lock in place. The passage stays sealed." << endl << endl;
    return FAILED;
}

// locked door stub. check player inventory for key during integration
PuzzleResult runLockedDoor() {
    cout << endl;
    printSeperator();
    cout << "  | LOCKED DOOR                                      |" << endl;
    printSeperator();
    cout << endl;
    cout << "  A heavy iron door blocks the passage." << endl;
    cout << "  There is a keyhole. The key must be somewhere on this floor." << endl;
    cout << endl;
    return FAILED;
}

// lore cipher. player types the hidden word from the lore text
// correct answer randomly unlocks the exit or opens a hidden passage
PuzzleResult runLoreCipher(int floorNum, Difficulty difficulty, bool &cipherOpensExit) {
    if (floorNum < 1 || floorNum > 9) return FAILED;

    const string cipherAnswers[] = {
        "",          // index 0 unused
        "CANINE",    // floor 1
        "LOST",      // floor 2
        "FEAR",      // floor 3
        "DARKNESS",  // floor 4
        "RUN",       // floor 5
        "MOURN",     // floor 6
        "END",       // floor 7
        "SEEN",      // floor 8
        "DECEIT"     // floor 9
    };

    // nth letter hint shown to player for floors 6-9
    int nValues[] = {0, 0, 0, 0, 0, 0, 3, 3, 2, 1};

    int retries = getRetries(difficulty);
    cipherOpensExit = (rand() % 2 == 0); // 50/50 each run

    printSeperator();
    cout << "  | HIDDEN MESSAGE                                   |" << endl;
    printSeperator();
    cout << endl;
    cout << "  You study the carving more closely." << endl;
    cout << "  Some letters look different. Larger. Deliberate." << endl;
    cout << endl;

    if (floorNum >= 6) {
        int n = nValues[floorNum];
        string suffix = "th";
        if (n == 1) suffix = "st";
        if (n == 2) suffix = "nd";
        if (n == 3) suffix = "rd";
        cout << "  The capitalised words hold a secret." << endl;
        cout << "  Take the " << n << suffix << " letter of each." << endl;
    } else {
        cout << "  The marked letters spell something." << endl;
    }

    cout << endl;
    cout << "  What word do they spell?" << endl;
    cout << "  Type S to skip." << endl;
    cout << endl;

    while (retries > 0) {
        cout << "  (" << retries << (retries == 1 ? " attempt" : " attempts") << " left)  > ";

        string input;
        cin >> input;
        cin.ignore(10000, '\n');
        toUpper(input);

        if (input == "S") {
            cout << endl << "  The letters blur back into the stone." << endl << endl;
            return SKIPPED;
        }

        if (input == cipherAnswers[floorNum]) {
            if (cipherOpensExit)
                cout << endl << "  The wall shifts. The path to the next floor is revealed." << endl << endl;
            else
                cout << endl << "  A section of wall grinds aside. A hidden passage opens." << endl << endl;
            return SOLVED;
        } else {
            retries--;
            if (retries > 0)
                cout << endl << "  Nothing happens. Look closer." << endl << endl;
        }
    }

    cout << endl << "  The message is lost. The letters mean nothing now." << endl << endl;
    return FAILED;
}