CC = gcc
DIAG_FLAGS = -DFOR_CPUDIAG -DPRINT_OPS -DPRINT_STATE
LD_FLAGS = -lSDL2 -lrt -lpthread
BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include
CPU8080_SRCS = $(SRC_DIR)/main/cpu_8080/*.c
ARCADE_SRCS = $(SRC_DIR)/main/arcade/*.c

init:
	mkdir -p $(BIN_DIR)

diagnostic: init
	$(CC) $(DIAG_FLAGS) $(SRC_DIR)/main/diagnostic.c $(CPU8080_SRCS) -o $(BIN_DIR)/diag

spaceinvaders: init
	$(CC) -DFOR_SPACE_INVADERS $(SRC_DIR)/main/space_invaders.c $(CPU8080_SRCS) $(ARCADE_SRCS) -o $(BIN_DIR)/spaceinvaders $(LD_FLAGS)

spaceinvadersd: init
	$(CC) -g -DPRINT_OPS -DFOR_SPACE_INVADERS $(SRC_DIR)/main/space_invaders.c $(CPU8080_SRCS) $(ARCADE_SRCS) -o $(BIN_DIR)/spaceinvadersd $(LD_FLAGS)

clean:
	rm -r bin