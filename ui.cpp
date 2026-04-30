#include "ui.h"
#include "fileio.h"
#include "dungeon.h"
#include <iostream>
#include <unistd.h>
#include <fstream>
using namespace std;

// combat log buffer — keeps last 4 lines
static string combatLog[4] = {"", "", "", ""};

void clearScreen() {
    cout << "\033[2J\033[H";
}

void typewrite(const string &text, int delayMs) {
    for (int i = 0; i < (int)text.length(); i++) {
        cout << text[i] << flush;
        usleep(delayMs * 1000);
    }
    cout << endl;
}

// ASCII art for each enemy
string getEnemyArt(const string &enemyName) {
    if (enemyName == "Bat Swarm") return
        "    _   ,_,   _\n"
        "   / `'=) (='` \\\n"
        "  /.-.-.\\  /.-.-.\\\n"
        "  `      \"      `";

    if (enemyName == "Cave Rats") return
        "             (\\_/)\n"
        "    .-\"\"-.-.'`   \\\n"
        "   /   \\      _.--'\n"
        "  (\\   /_---\\\\__\\_\n"
        "   `'-.\n"
        "    ,__)";

    if (enemyName == "Stone Beetle") return
        "  \\         /\n"
        "   `-.`-'.-'\n"
        "   ,:--.--:.\n"
        "  / |  |  | \\\n"
        "   /\\  |  /\\\n"
        "   | `.:.' |";

    if (enemyName == "Stray Dogs") return
        "                __\n"
        "               /\\/''-,\n"
        "       ,--'''''   /\"\n"
        " ____,'.  )       \\___\n"
        "'\"\"\"\"\"------'\"\"\"'-----'";

    if (enemyName == "Venomous Snake") return
        "                        _,.--.  \n"
        "    --..,_           .'`__ o  `;__,\n"
        "       `'.'.       .'.'`  '---'`  '\n"
        "          '.`-...-'.'              \n"
        "            `-...-'";

    if (enemyName == "Rabid Jackal") return
        "               /^._\n"
        "  ,___,--~~~~--' /'\n"
        "  `~--~\\ )___,)/'\n"
        "      (/\\\\_  (/\\\\_ ";

    if (enemyName == "Corrupted Hound") return
        "               ;~~,__\n"
        ":-....,-------'`-'._.' \n"
        " `-,,,  ,       ;'~~'\n"
        "    ,'_,'~.__; '--.\n"
        "   //'       ````(;\n"
        "  `-'";

    if (enemyName == "Shadow Owl") return
        "   , _ ,\n"
        "  ( o o )\n"
        " /'` ' `'\\\n"
        " |'''''''|\n"
        " |\\\\'''//|\n"
        "   \"\"\"";

    if (enemyName == "Phantom Direwolf") return
        "   / \\__\n"
        "  (      \\___\n"
        "  /           O\n"
        " /    (______/\n"
        "/______/";

    if (enemyName == "Cerberus") return
        "                      /\\_/\\____,\n"
        "            ,___/\\_/\\ \\  ~     /\n"
        "            \\     ~  \\ )   XXX\n"
        "              XXX     /    /\\_/\\____,\n"
        "                 \\o-o/-o-o/   ~    /\n"
        "                  ) /     \\    XXX\n"
        "                 _|    / \\ \\_/\n"
        "              ,-/   _  \\_/   \\\n"
        "             / (   /____,__|  )\n"
        "            (  |_ (    )  \\) _|\n"
        "           _/ _)   \\   \\__/   (_\n"
        "          (,-(,(,(,/      \\,),),)";

    return "  [?]"; // unknown enemy fallback
}

char showTitleScreen() {
    clearScreen();

    cout << "=================================================================" << endl;
    cout << endl;
    cout << "                        /)-._                                        ," << endl;
    cout << "                        . ' _]                                       |`-.__" << endl;
    cout << "               ,.._    |`--\"=          C E R B E R U S ' S          /   _/" << endl;
    cout << "              /    \"-/     \\                C A V E                ****`" << endl;
    cout << "  /)          |    |_      `\\|___                                 /    }" << endl;
    cout << "  \\:::::::::\\____/ \\__\\________\\   ~ dungeon crawler ~           /  \\ /" << endl;
    cout << "                                                              \\ /`   \\\\\\     " << endl;
    cout << "                                                               `\\    /_\\\\" << endl;
    cout << "                                                                `~~~~~``~`" << endl;
    cout << endl;
    cout << "       uncover the truth behind Cerberus, one wall at a time." << endl;
    cout << endl;
    cout << "    [ N ]  New Game     [ L ]  Load Game" << endl;
    cout << "    [ S ]  Leaderboard  [ Q ]  Quit" << endl;
    cout << endl;
    cout << "=================================================================" << endl;
    cout << "  > ";

    char choice;
    cin >> choice;
    cin.ignore(10000, '\n');
    return toupper(choice);
}

Difficulty showDifficultySelect() {
    clearScreen();
    cout << "=================================================================" << endl;
    cout << endl;
    typewrite("  Choose your difficulty.", 20);
    cout << endl;
    cout << "  [ 1 ]  EASY    -- more retries, lower gold penalties" << endl;
    cout << "  [ 2 ]  NORMAL  -- balanced experience" << endl;
    cout << "  [ 3 ]  HARD    -- one mistake costs everything. permadeath." << endl;
    cout << endl;
    cout << "=================================================================" << endl;
    cout << "  > ";

    int choice;
    cin >> choice;
    cin.ignore(10000, '\n');

    if (choice == 1) return EASY;
    if (choice == 3) return HARD;
    return NORMAL;
}

string showNameEntry() {
    clearScreen();
    cout << "=================================================================" << endl;
    cout << endl;
    typewrite("  The warrior has no name until they choose one.", 20);
    cout << endl;
    cout << "  Enter your name: > ";

    string name;
    getline(cin, name);
    if (name.empty()) name = "Warrior";
    return name;
}

void showHUD(const Player &player) {
    cout << "=================================================================" << endl;
    cout << "  Floor: " << player.floor
         << "  |  HP: " << player.hp << "/" << player.maxHp
         << "  |  ATK: " << player.attack
         << "  |  DEF: " << player.defense
         << "  |  Gold: " << player.gold << endl;

    // lore scroll tracker
    cout << "  Lore: ";
    for (int i = 0; i < TOTAL_FLOORS; i++) {
        if (player.loreFound[i])
            cout << "[" << i << ":FOUND] ";
        else
            cout << "[" << i << ":NULL]  ";
    }
    cout << endl;
    cout << "=================================================================" << endl;
}

void showFloorEntry(int floorNum, const LoreFragment &fragment) {
    clearScreen();
    cout << "=================================================================" << endl;
    cout << endl;

    string title = "  Floor " + to_string(floorNum) + " -- " + fragment.title;
    typewrite(title, 25);
    cout << endl;

    // fragment.intro contains: atmospheric paragraph + diary, separated by ---DIARY---
    // We only show the atmospheric paragraph here (everything before ---DIARY---)
    string intro = fragment.intro;
    size_t sep = intro.find("---DIARY---");
    string atmospheric = (sep != string::npos) ? intro.substr(0, sep) : intro;

    // trim trailing newlines
    while (!atmospheric.empty() && atmospheric[atmospheric.size()-1] == '\n')
        atmospheric.erase(atmospheric.size()-1);

    if (!atmospheric.empty()) {
        typewrite(atmospheric, 15);
    } else {
        // diary-only floors (6, 7): show a brief atmospheric line
        typewrite("  The walls are bare here. No carvings. No paintings.", 15);
        typewrite("  But wedged into a crack near the ground, you find a journal.", 15);
    }

    cout << endl;
    cout << "  Press Enter to continue...";
    cin.ignore(10000, '\n');
}

void showMap(const Floor &floor) {
    renderMap(&floor, floor.playerX, floor.playerY);
    (void)floor; // suppress unused warning
}

void showCombatScreen(const Player &player, const Enemy &enemy, const std::string &enemyArt) {
    clearScreen();
    cout << "=================================================================" << endl;
    cout << "  COMBAT  --  Floor " << player.floor << endl;
    cout << "=================================================================" << endl;
    cout << endl;

    // split enemyArt into lines so we can print cat and enemy side by side
    string artLines[12];
    int artCount = 0;
    string buf = "";
    for (int i = 0; i <= (int)enemyArt.length(); i++) {
        if (i == (int)enemyArt.length() || enemyArt[i] == '\n') {
            if (artCount < 12) artLines[artCount++] = buf;
            buf = "";
        } else {
            buf += enemyArt[i];
        }
    }

    // cat art lines (left column, fixed width 22)
    const string catLines[] = {
        "   /\\_/\\",
        "  ( o o )      VS",
        "   > ^ <",
        "",
        "",
        ""
    };
    const int CAT_LINES = 6;
    const int COL_WIDTH = 22;

    // first line: enemy name appears top-right
    cout << "   /\\_/\\";
    for (int p = 7; p < COL_WIDTH; p++) cout << " ";
    cout << "  " << RED << enemy.name << RESET << endl;

    // remaining side-by-side rows
    int totalLines = (CAT_LINES > artCount) ? CAT_LINES : artCount;
    for (int i = 1; i < totalLines; i++) {
        string left  = (i < CAT_LINES) ? catLines[i] : "";
        string right = (i < artCount)  ? artLines[i]  : "";
        cout << left;
        int pad = COL_WIDTH - (int)left.length();
        for (int p = 0; p < pad; p++) cout << " ";
        cout << "  " << right << endl;
    }

    cout << endl;
    cout << "-----------------------------------------------------------------" << endl;
    cout << "  " << YELLOW << "[YOU]" << RESET
         << "  HP: " << player.hp << "/" << player.maxHp
         << "  ATK: " << player.attack
         << "  DEF: " << player.defense << endl;
    cout << "  " << RED << "[" << enemy.name << "]" << RESET
         << "  HP: " << enemy.hp << "/" << enemy.maxHp
         << "  ATK: " << enemy.attack
         << "  DEF: " << enemy.defense << endl;
    cout << "-----------------------------------------------------------------" << endl;
    cout << endl;
    cout << "  [ A ] Attack    [ I ] Use Item    [ R ] Run" << endl;
    cout << endl;
    showCombatLog();
    cout << "=================================================================" << endl;
    cout << "  > ";
}

void addCombatLog(const string &line) {
    combatLog[0] = combatLog[1];
    combatLog[1] = combatLog[2];
    combatLog[2] = combatLog[3];
    combatLog[3] = line;
}

void showCombatLog() {
    for (int i = 0; i < 4; i++)
        if (!combatLog[i].empty()) cout << "  " << combatLog[i] << endl;
}

void clearCombatLog() {
    for (int i = 0; i < 4; i++) combatLog[i] = "";
}

void showLoreScreen(const LoreFragment &fragment, bool cipherAvailable) {
    clearScreen();
    cout << "=================================================================" << endl;
    cout << "  ~ Ancient Carving -- Floor " << fragment.floorNumber
         << " : " << fragment.title << " ~" << endl;
    cout << "-----------------------------------------------------------------" << endl;
    cout << endl;

    // Wall art (fragment.text)
    if (!fragment.text.empty()) {
        cout << "  [ WALL ART ]" << endl << endl;
        typewrite(fragment.text, 12);
        cout << endl;
    }

    // Scholar diary — extract from fragment.intro after ---DIARY--- separator
    string intro = fragment.intro;
    size_t sep = intro.find("---DIARY---");
    string diary = (sep != string::npos) ? intro.substr(sep + 11) : "";

    // trim leading newline from diary
    while (!diary.empty() && diary[0] == '\n') diary.erase(0, 1);

    if (!diary.empty()) {
        cout << "  [ LOST SCHOLAR'S DIARY ]" << endl << endl;
        typewrite(diary, 12);
        cout << endl;
    }

    cout << "-----------------------------------------------------------------" << endl;
    cout << "  [ X ] Leave" << endl;
    cout << "=================================================================" << endl;
    cout << "  > ";

    char loreInput;
    cin >> loreInput;
    cin.ignore(10000, '\n');
}

int showInventory(const Player &player) {
    clearScreen();
    cout << "=================================================================" << endl;
    cout << "  INVENTORY" << endl;
    cout << "-----------------------------------------------------------------" << endl;
    cout << endl;

    if (player.inventorySize == 0) {
        cout << "  Nothing here." << endl;
    } else {
        for (int i = 0; i < player.inventorySize; i++) {
            cout << "  [" << (i + 1) << "] " << player.inventory[i]->name
                 << "  (value: " << player.inventory[i]->value << ")" << endl;
        }
    }

    cout << endl;
    cout << "  Select number to use  |  D + number to drop  |  X to close" << endl;
    cout << "=================================================================" << endl;
    cout << "  > ";

    string input;
    cin >> input;
    cin.ignore(10000, '\n');

    if (input == "X" || input == "x") return -1;
    if ((input[0] == 'D' || input[0] == 'd') && input.length() > 1) {
        int idx = atoi(input.substr(1).c_str()) - 1;
        if (idx >= 0 && idx < player.inventorySize) return -(idx + 2); // negative = drop signal
    }
    int idx = atoi(input.c_str()) - 1;
    if (idx >= 0 && idx < player.inventorySize) return idx;
    return -1;
}

void showGameOver(const Player &player) {
    clearScreen();
    cout << "=================================================================" << endl;
    cout << endl;
    cout << "   /\\_/\\" << endl;
    cout << "  ( x x )" << endl;
    cout << "   /   \\" << endl;
    cout << endl;
    typewrite("  You have fallen in Cerberus's Cave.", 20);
    cout << endl;
    cout << "  Floor reached:    " << player.floor << endl;
    cout << "  Gold collected:   " << player.gold << endl;

    int loreCount = 0;
    for (int i = 0; i < TOTAL_FLOORS; i++) {
        if (player.loreFound[i]) loreCount++;
    }
    cout << "  Lore discovered:  " << loreCount << " / " << TOTAL_FLOORS << endl;
    cout << endl;
    cout << "  [ R ] Retry        [ Q ] Quit" << endl;
    cout << endl;
    cout << "=================================================================" << endl;
    cout << "  > ";
}

void showVictory(const Player &player) {
    clearScreen();
    (void)player;

    // read ENDING_START...ENDING_END block from lore.txt
    string endingLines[300];
    int endingCount = 0;

    ifstream loreFile("data/lore.txt");
    if (loreFile.is_open()) {
        string line;
        bool inEnding = false;
        while (getline(loreFile, line) && endingCount < 298) {
            if (line == "ENDING_START") { inEnding = true; continue; }
            if (line == "ENDING_END")   { break; }
            if (inEnding) endingLines[endingCount++] = line;
        }
        loreFile.close();
    }

    cout << "=================================================================" << endl;
    cout << endl;

    for (int i = 0; i < endingCount; i++) {
        const string &ln = endingLines[i];

        if (ln.empty() || ln == "---") {
            cout << endl;
            usleep(700000);   // pause on blank lines / section breaks
            continue;
        }

        bool isDialogue = (ln.find("Kyon:")    != string::npos ||
                           ln.find("Warrior:") != string::npos ||
                           ln.find("Bastet:")  != string::npos);

        cout << "  ";
        typewrite(ln, isDialogue ? 28 : 18);

        if (isDialogue) usleep(500000);   // extra pause after each spoken line
    }

    cout << endl;
    cout << "=================================================================" << endl;
    cout << "  Press Enter to see the leaderboard...";
    cin.ignore(10000, '\n');
}

void showLeaderboard() {
    clearScreen();
    printLeaderboard();
    cout << " Press Enter to return to title screen...";
    cin.ignore(10000, '\n');
}