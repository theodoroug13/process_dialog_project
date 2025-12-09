CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude

BUILD_DIR = build

SRC_CLIENT = src/client.c src/dialog.c src/thread_funcs.c
SRC_INIT   = src/init_shared.c src/dialog.c

OBJ_CLIENT = $(SRC_CLIENT:src/%.c=$(BUILD_DIR)/%.o)
OBJ_INIT   = $(SRC_INIT:src/%.c=$(BUILD_DIR)/%.o)

EXEC_CLIENT = $(BUILD_DIR)/client
EXEC_INIT   = $(BUILD_DIR)/init_shared_memory

all: $(EXEC_CLIENT) $(EXEC_INIT)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


$(EXEC_CLIENT): $(BUILD_DIR) $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $@ $(OBJ_CLIENT)


$(EXEC_INIT): $(BUILD_DIR) $(OBJ_INIT)
	$(CC) $(CFLAGS) -o $@ $(OBJ_INIT)


$(BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
