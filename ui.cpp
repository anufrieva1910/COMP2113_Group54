#include "ui.h"
#include <iostream>
#include <unistd.h>
using namespace std;

// combat log buffer. keeps last 2 lines
static string logLine1 = "";
static string logLine2 = "";

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
        "   / `'=) (='` \\n"
        "  /.-.-.\\  /.-.-.\\\n"
        "  `      \"      `";

    if (enemyName == "Cave Rat") return
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

    if (enemyName == "Stray Dog") return
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
        "    \"\"\"";

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
    cout << "                     /)-._" << endl;
    cout << "                     . ' _]" << endl;
    cout << "            ,.._    |`--\"=" << endl;
    cout << "           /    \"-/     \\" << endl;
    cout << " /)       |    |_      `\\|___" << endl;
    cout << " \\::::::::\\____/ \\__\\________\\" << endl;
    cout << endl;
    cout << "                   ,                    " << endl;
    cout << "                   |`-.__               " << endl;
    cout << "                   /   _/               " << endl;
    cout << "                  ****`                 " << endl;
    cout << "                 /    }                 " << endl;
    cout << "                /  \\ /                  " << endl;
    cout << "            \\ /`   \\\\\\                 " << endl;
    cout << "             `\\    /_\\\\                " << endl;
    cout << "              `~~~~~``~`               " << endl;
    cout << endl;

    // game title typewriter
    typewrite("              C E R B E R U S ' S   C A V E", 30);
    cout << endl;
    cout << "    a story-driven terminal dungeon crawler" << endl;
    cout << "    uncover the truth behind Cerberus, one wall at a time." << endl;
    cout << endl;
    cout << "    [ N ]  New Game" << endl;
    cout << "    [ L ]  Load Game" << endl;
    cout << "    [ S ]  Leaderboard" << endl;
    cout << "    [ Q ]  Quit" << endl;
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
         << "  |  DEF: " << player.defense
         << "  |  Gold: " << player.gold << endl;

    // lore scroll tracker
    cout << "  Lore: ";
    for (int i = 1; i <= 9; i++) {
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

    // floor title typewriter
    string title = "  Floor " + to_string(floorNum) + " -- " + fragment.title;
    typewrite(title, 25);
    cout << endl;

    // atmospheric paragraph typewriter
    // the first paragraph in fragment.text
    string text = fragment.text;
    string entry = "";
    int wallPos = text.find("[WALL ART]");
    int diaryPos = text.find("[LOST SCHOLAR");
    int cutPos = (int)text.length();
    if (wallPos != (int)string::npos) cutPos = min(cutPos, wallPos);
    if (diaryPos != (int)string::npos) cutPos = min(cutPos, diaryPos);
    entry = text.substr(0, cutPos);

    typewrite(entry, 18);
    cout << endl;
    cout << "  Press Enter to continue...";
    cin.ignore(10000, '\n');
}

void showMap(const Floor &floor) {
    // placeholder until Tarishi's renderer is integrated
    cout << endl;
    cout << "  [ map goes here ]" << endl;
    cout << endl;
    (void)floor; // suppress unused warning
}

void showCombatScreen(const Player &player, const Enemy &enemy, const string &enemyArt) {
    clearScreen();
    cout << "=================================================================" << endl;
    cout << "  COMBAT" << endl;
    cout << "=================================================================" << endl;
    cout << endl;

    // cat warrior on left, enemy on right
    cout << "   /\\_/\\                    " << enemy.name << endl;
    cout << "  ( o o )       VS" << endl;
    cout << "   /   \\" << endl;
    cout << endl;
    cout << enemyArt << endl;
    cout << endl;
    cout << "-----------------------------------------------------------------" << endl;
    cout << "  [YOU]   HP: " << player.hp << "/" << player.maxHp
         << "   DEF: " << player.defense << endl;
    cout << "  [" << enemy.name << "]   HP: " << enemy.hp << "/" << enemy.maxHp << endl;
    cout << "-----------------------------------------------------------------" << endl;
    cout << endl;
    cout << "  [ A ] Attack    [ I ] Use Item    [ R ] Run" << endl;
    cout << endl;

    showCombatLog();

    cout << "=================================================================" << endl;
    cout << "  > ";
}

void addCombatLog(const string &line) {
    logLine1 = logLine2;
    logLine2 = line;
}

void showCombatLog() {
    if (!logLine1.empty()) cout << "  " << logLine1 << endl;
    if (!logLine2.empty()) cout << "  " << logLine2 << endl;
}

void showLoreScreen(const LoreFragment &fragment, bool cipherAvailable) {
    clearScreen();
    cout << "=================================================================" << endl;
    cout << "  ~ Ancient Carving -- Floor " << fragment.floorNumber
         << " : " << fragment.title << " ~" << endl;
    cout << "-----------------------------------------------------------------" << endl;
    cout << endl;

    typewrite(fragment.text, 18);

    cout << endl;
    cout << "-----------------------------------------------------------------" << endl;
    if (cipherAvailable)
        cout << "  [ C ] Attempt cipher        [ X ] Leave" << endl;
    else
        cout << "  [ X ] Leave" << endl;
    cout << "=================================================================" << endl;
    cout << "  > ";
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
    for (int i = 1; i <= 9; i++) {
        if (player.loreFound[i]) loreCount++;
    }
    cout << "  Lore discovered:  " << loreCount << " / 9" << endl;
    cout << endl;
    cout << "  [ R ] Retry        [ Q ] Quit" << endl;
    cout << endl;
    cout << "=================================================================" << endl;
    cout << "  > ";
}

void showVictory(const Player &player) {
    clearScreen();
    (void)player;
    cout << "=================================================================" << endl;
    cout << endl;

    // ending dialogue from lore.txt ENDING block. typewritten line by line
    const char* lines[] = {
        "  The cave falls quiet after the final blow. Not the quiet of",
        "  emptiness -- the quiet of something finally finished.",
        "",
        "  The dark force lifts slowly. What remains on the ground is",
        "  small. Trembling. A puppy, barely older than a few months.",
        "",
        "  You sheathe your weapon.",
        "",
        "  Kyon: \"...where... where am I...\"",
        "",
        "  Warrior: \"You're safe. It's over.\"",
        "",
        "  Kyon: \"I couldn't stop it... she kept talking and I couldn't",
        "         make it stop... everything was so loud...\"",
        "",
        "  Warrior: \"I know. I read the walls. All of them.\"",
        "",
        "  Kyon: \"Did I... did I hurt people...?\"",
        "",
        "  Warrior: \"Yes. But that wasn't you. That was what she turned you into.\"",
        "",
        "  Kyon: \"...is there a difference?\"",
        "",
        "  Warrior: \"There will be. When they know the truth, there will be.\"",
        "",
        "  Kyon: \"...I had a name. Before all this. I think I had a name.\"",
        "",
        "  Warrior: \"Kyon Khryseos. The Golden Hound. I know.\"",
        "",
        "  Kyon: \"...will you stay? Just for a moment.\"",
        "",
        "  Warrior: \"As long as you need.\"",
        nullptr
    };

    for (int i = 0; lines[i] != nullptr; i++) {
        typewrite(string(lines[i]), 20);
        if (lines[i][0] == '\0') usleep(300000); // small pause on blank lines
    }

    cout << endl;
    cout << "=================================================================" << endl;
    cout << "  Press Enter to see the leaderboard...";
    cin.ignore(10000, '\n');
}

void showLeaderboard() {
    clearScreen();
    cout << "=================================================================" << endl;
    cout << "  LEADERBOARD" << endl;
    cout << "-----------------------------------------------------------------" << endl;
    cout << endl;
    cout << "  [ leaderboard loaded by fileio -- placeholder ]" << endl;
    cout << endl;
    cout << "=================================================================" << endl;
    cout << "  Press Enter to return...";
    cin.ignore(10000, '\n');
}