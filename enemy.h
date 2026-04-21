#ifndef ENEMY_H
#define ENEMY_H

#include <string>
using namespace std;

struct enemy{
  string name;
  int hp;
  int attack;
  int defense;
  bool isboss;
  int phase;
};

#endif 
