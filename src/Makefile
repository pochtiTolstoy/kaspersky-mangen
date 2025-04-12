# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic -I.

# Source files
SRC_DIR = .
BUILD_DIR = ../build

SRCS = $(shell find $(SRC_DIR) -type f -name '*.c')
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = mangen

# Targets
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@ 
	@echo "[INFO] program was built: $@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR) $(TARGET)
	@echo "[INFO] Build files cleaned"

rebuild: clean all

# Style targets
style_check: 
	find $(SRC_DIR) -type f -name "*.[c,h]" -exec clang-format -n -style=Google {} +
	! find $(SRC_DIR) -type f -name "*.[c,h]" -exec grep -n '[А-Яа-яЁё]' {} +

style_fix: 
	find $(SRC_DIR) -type f -name "*.[c,h]" -exec clang-format -i -style=Google {} +

comments:
	find $(SRC_DIR) -type f -name "*.[c,h]" -exec grep -n '//\|\/\*' {} +

.PHONY: all clean rebuild style_check style_fix comments
