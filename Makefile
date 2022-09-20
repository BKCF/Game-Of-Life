CC=g++
CCFLAGS=-O3
EXENAME=run



all: 
	g++ -I/usr/include/SDL2 -c -o main.o main.cpp
	g++ main.o -lSDL2 -o $(EXENAME) -g
debug: 
	$(CC) $(CCFLAGS) `pkg-config --cflags --libs sdl2` main.cpp -o $(EXENAME) -ggdb
	
clean:
	rm ./$(EXENAME) main.o
