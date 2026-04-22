#Makefile for Cerberus' Cave
#Compile with: make
#Clean with: make clean

FLAGS = -pedantic-errors -std=c++11
TARGET = dungeon
OBJECTS = main.o player.o dungeon.o combat.o enemy.o puzzle.o ui.o fileio.o

$(TARGET): $(OBJECTS)
	g++ $(FLAGS) $^ -o $@

main.o: main.cpp player.h dungeon.h combat.h enemy.h puzzle.h ui.h fileio.h types.h
	g++ $(FLAGS) -c $<

player.o: player.cpp player.h types.h
	g++ $(FLAGS) -c $<

dungeon.o: dungeon.cpp dungeon.h types.h
	g++ $(FLAGS) -c $<

combat.o: combat.cpp combat.h types.h
	g++ $(FLAGS) -c $<

enemy.o: enemy.cpp enemy.h types.h
	g++ $(FLAGS) -c $<

puzzle.o: puzzle.cpp puzzle.h types.h
	g++ $(FLAGS) -c $<

ui.o: ui.cpp ui.h types.h
	g++ $(FLAGS) -c $<

fileio.o: fileio.cpp fileio.h types.h
	g++ $(FLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: clean