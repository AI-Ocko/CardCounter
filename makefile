CC = gcc
CFLAGS = -Wall -Wextra -I$(INC_DIR) -lncursesw

BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
INC_DIR = include
TARGET = $(BIN_DIR)/basicStrategyTrainer
SRCS = src/basicStrategy.c src/layout.c src/settings.c src/trainer_cardDrawFunctions.c src/trainer_hardTotalTrainer.c src/trainer_pairSplittingTrainer.c src/trainer_softTotalTrainer.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(TARGET)

# Link all object files into the final binary
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Compile each src/*.c into obj/*.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

.PHONY: all clean

