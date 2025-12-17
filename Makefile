CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude

BUILD_DIR = build

SRC_MAIN = src/main.c src/dialog.c src/thread_funcs.c
SRC_INIT   = src/init_shared.c src/dialog.c

OBJ_MAIN = $(SRC_MAIN:src/%.c=$(BUILD_DIR)/%.o)
OBJ_INIT   = $(SRC_INIT:src/%.c=$(BUILD_DIR)/%.o)

EXEC_MAIN = $(BUILD_DIR)/exec
EXEC_INIT   = $(BUILD_DIR)/init_shared_memory

all: $(EXEC_MAIN) $(EXEC_INIT)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


$(EXEC_MAIN): $(BUILD_DIR) $(OBJ_MAIN)
	$(CC) $(CFLAGS) -o $@ $(OBJ_MAIN)


$(EXEC_INIT): $(BUILD_DIR) $(OBJ_INIT)
	$(CC) $(CFLAGS) -o $@ $(OBJ_INIT)


$(BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
