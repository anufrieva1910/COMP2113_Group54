#include "items.h"

//array of Item type objects that includes every pickable items in the game
const Item ITEMS_LIST[] = {
    { WEAPON, "Iron Sword",           8  },
    { WEAPON, "Steel Sword",         15  },
    { WEAPON, "Shadow Blade",        22  },
    { SHIELD, "Iron Shield",          5  },
    { SHIELD, "Steel Shield",        10  },
    { SHIELD, "Dragonhide Barrier",  15  },
    { POTION, "Herb Wrap",           25  },  
    { POTION, "Health Potion",       40  },  
    { POTION, "Large Health Potion", 80  },
    { POTION, "Divine Blessing",    150  },
    { LOOT,   "Gold Coin Pouch",     20  },
    { LOOT,   "Ancient Relic",       50  },
    { LOOT,   "Great Rune",          100 }
};

//constant integer number that incidates the number of pickable items
const int NUM_ITEMS = sizeof(ITEMS_LIST) / sizeof(ITEMS_LIST[0]);