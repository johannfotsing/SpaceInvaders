CC = gcc
DIAGFLAGS = -g -DFOR_CPUDIAG -DPRINT_OPS -DPRINT_STATE
BIN_DIR = ./bin
SRC_DIR = ./src

init:
	mkdir -p $(BIN_DIR)

cpu8080diag: init
	$(CC) $(DIAGFLAGS) $(SRC_DIR)/Cpu8080.c $(SRC_DIR)/Cpu8080Diag.c -o $(BIN_DIR)/diag