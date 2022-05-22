# Dimitios Koropoulis 3967
# csd3967@csd.uoc.gr
# CS352 - Fall 2021
# Makefile

CC = g++
CFLAGS = -std=c++17 -Wall -Wextra -Wno-unused-parameter

SRC = src
OBJ = obj

SRCS = $(wildcard $(SRC)/*.cc)
OBJS = $(patsubst $(SRC)/%.cc, $(OBJ)/%.o, $(SRCS))
TARGET = demo

.PHONY: all, clean, run

all:
	@$(MAKE) --no-print-directory clean
	@mkdir $(OBJ)
	@$(MAKE) --no-print-directory $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ) $(TARGET)

run: all
	@printf -------------------\\n
	@./$(TARGET)

