#Don't forget 
#export LIBRARY_PATH="$LIBRARY_PATH:$(brew --prefix)/lib"
#export LIBRARY_PATH="$LIBRARY_PATH:$(brew --prefix)/include"


CC = g++
LANG_STD = --std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors
INCLUDE_PATH = -I"./libs"
SRC_FILES = src/*.cpp \
			src/Logger/*.cpp \
			src/Game/*.cpp \
			src/ECS/*.cpp \
			src/AssetStore/*.cpp \
			libs/imgui/*.cpp

#also `sdl2-config --libs --cflags` not -lSDL2
LINKER_FILES = `sdl2-config --libs --cflags` -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua -lspdlog
OBJ_NAME = gameengine

build:
	$(CC) $(COMPILER_FLAGS) $(LANG_STD) $(INCLUDE_PATH) $(SRC_FILES) $(LINKER_FILES) -o $(OBJ_NAME);
run:
	./$(OBJ_NAME);
clean:
	rm $(OBJ_NAME);