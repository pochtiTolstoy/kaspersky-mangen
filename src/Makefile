# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic -I. -g
LDFLAGS = -lcrypto -lssl

# Source files
SRC_DIR = .
TEST_DIR = $(SRC_DIR)/tests
BUILD_DIR = ../build
GIT_HASH = $(shell git rev-parse --short HEAD 2>/dev/null || echo "unknown")
CFLAGS += -DGIT_COMMIT_HASH=\"$(GIT_HASH)\"

SRCS = $(shell find $(SRC_DIR) -not -path '$(TEST_DIR)/*' -type f -name '*.c' 2>/dev/null)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = mangen

# Targets
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "[INFO] program was built: $@"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	@rm -rf $(BUILD_DIR) $(TARGET)
	@echo "[INFO] Build files cleaned"

rebuild: clean all

# Docs
docs: all
	cd ../ && \
	rm -rf doxygen_docs && \
	doxygen doxygen.txt && \
	open doxygen_docs/html/index.html

# Style targets
style_check: 
	find $(SRC_DIR) -type f -name "*.[c,h]" -exec clang-format -n -style=Google {} +
	! find $(SRC_DIR) -type f -name "*.[c,h]" -exec grep -n '[А-Яа-яЁё]' {} +

style_fix: 
	find $(SRC_DIR) -type f -name "*.[c,h]" -exec clang-format -i -style=Google {} +

comments:
	find $(SRC_DIR) -type f -name "*.[c,h]" -exec grep -n '//\|\/\*' {} +

.PHONY: all clean rebuild style_check style_fix comments
