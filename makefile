CC = gcc

# Main directories
BIN_DIR = bin
SRC_DIR = src
INC_DIR = include

# Compilation flags
C_FLAGS = -I$(INC_DIR)
SDL2_C_FLAGS = `sdl2-config --cflags`
SDL2_LD_FLAGS = `sdl2-config --libs`

# Target sources
CPU8080_SRCS = $(SRC_DIR)/main/cpu_8080/*.c
ARCADE_SRCS = $(SRC_DIR)/main/arcade/*.c
DIAGNOSTIC_SRCS = $(SRC_DIR)/main/diagnostic.c $(CPU8080_SRCS)
SDL_TEST_SRCS = $(SRC_DIR)/test/sdl_window.c
SPACE_INVADERS_SRCS = $(SRC_DIR)/main/space_invaders.c $(CPU8080_SRCS) $(ARCADE_SRCS)

# Target flags
DIAGNOSTIC_C_FLAGS = $(CFLAGS) -DFOR_CPUDIAG -DPRINT_OPS -DPRINT_STATE
SPACE_INVADERS_C_FLAGS = $(CFLAGS) -DFOR_SPACE_INVADERS
SPACE_INVADERS_LD_FLAGS = -lrt -lpthread $(SDL2_LD_FLAGS)

init:
	mkdir -p $(BIN_DIR)

diagnostic: init
	$(CC) $(DIAGNOSTIC_C_FLAGS) $(DIAGNOSTIC_SRCS) -o $(BIN_DIR)/diagnostic

sdl_test: init
	$(CC) $(SDL2_C_FLAGS) $(SDL_TEST_SRCS) -o $(BIN_DIR)/sdl_test $(SDL2_LD_FLAGS)

spaceinvaders: init
	$(CC) $(SPACE_INVADERS_C_FLAGS) $(SPACE_INVADERS_SRCS) -o $(BIN_DIR)/spaceinvaders $(SPACE_INVADERS_LD_FLAGS)

spaceinvadersd: init
	$(CC) -ggdb3 -fsanitize=address -Wall -DPRINT_OPS $(SPACE_INVADERS_C_FLAGS) $(SPACE_INVADERS_SRCS) -o $(BIN_DIR)/spaceinvadersd $(SPACE_INVADERS_LD_FLAGS)

clean:
	rm -r bin