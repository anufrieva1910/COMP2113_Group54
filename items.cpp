#include "items.h"

const Item ITEMS_LIST[] = {
    { WEAPON, "Iron Sword",           8  },
    { WEAPON, "Steel Sword",         15  },
    { WEAPON, "Shadow Blade",        22  },
    { SHIELD, "Iron Shield",          5  },
    { SHIELD, "Steel Shield",        10  },
    { SHIELD, "Dragonhide Barrier",  15  },
    { POTION, "Health Potion",       40  },  // heals more now enemies hit harder
    { POTION, "Large Health Potion", 80  },
    { POTION, "Divine Blessing",    150  },
    { POTION, "Herb Wrap",           25  },  // added cheap heal option
    { LOOT,   "Gold Coin Pouch",     20  },
    { LOOT,   "Ancient Relic",       50  },
};
const int NUM_ITEMS = sizeof(ITEMS_LIST) / sizeof(ITEMS_LIST[0]);