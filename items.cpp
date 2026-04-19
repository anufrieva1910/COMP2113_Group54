#include "items.h"

const Item ITEMS_LIST[]={
    { WEAPON, "Iron Sword", 8},
    { WEAPON, "Steel Sword", 15},
    { WEAPON, "Shadow Blade", 25},
    { SHIELD, "Iron Shield", 5},
    { SHIELD, "Steel Shield", 10},
    { SHIELD, "Dragonhide Barrier", 15},
    { POTION, "Health Potion", 30},
    { POTION, "Large Health Potion", 60},
    { POTION, "Divine Blessing", 100},
    { LOOT, "Gold Coin Pouch", 20},
    { LOOT, "Ancient Relic", 50},
    { LOOT, "Great Rune of Kings", 100}
};
const int NUM_ITEMS=sizeof(ITEMS_LIST)/sizeof(ITEMS_LIST[0]);