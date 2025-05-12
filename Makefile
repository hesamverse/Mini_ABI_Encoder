# Makefile for Mini ABI Encoder
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
SRC_DIR = src
OBJ_DIR = build
BIN = abi_encoder

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(BIN)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)