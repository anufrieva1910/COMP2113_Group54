#ifndef ENEMY_H
#define ENEMY_H

#include <string>


struct enemy{
  std::string name;
  int hp;
  int attack;
  int defense;
  bool isboss;
  int phase;
};

#endif 
