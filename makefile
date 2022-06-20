CC = gcc -g
DIAGFLAGS = -DFOR_CPUDIAG -DPRINT_OPS -DPRINT_STATE
SDLFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf
BIN_DIR = ./bin
SRC_DIR = ./src

init:
	mkdir -p $(BIN_DIR)

cpu8080diag: init
	$(CC) $(DIAGFLAGS) $(SRC_DIR)/Cpu8080.c $(SRC_DIR)/Cpu8080Diag.c -o $(BIN_DIR)/diag

spaceinvaders: init
	$(CC) $(SRC_DIR)/Cpu8080.c $(SRC_DIR)/SpaceInvadersArcade.c $(SRC_DIR)/SpaceInvaders.c -o $(BIN_DIR)/spaceinvaders $(SDLFLAGS)