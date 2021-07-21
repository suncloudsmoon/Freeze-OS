PREFIX	?= /usr/

CC       := gcc
CFLAGS   += -Wall -Wextra -O0 -g
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects/
APP_DIR  := $(BUILD)/bin/
TARGET   := freezeos
SRC      := $(wildcard src/*.c)
OBJECTS  := $(SRC:%.c=$(OBJ_DIR)%.o)

all: build freezeos

$(OBJ_DIR)%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

$(APP_DIR)$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $(APP_DIR)$(TARGET) $(OBJECTS) $(LDFLAGS)

.PHONY: all build clean freezeos

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

freezeos: $(APP_DIR)$(TARGET)

install: install_bin

install_bin: $(APP_DIR)$(TARGET)
	install -d $(PREFIX)/bin/
	install -m 775 $(APP_DIR)$(TARGET) $(PREFIX)/bin/

clean:
	-@rm -rvf $(OBJ_DIR)*
	-@rm -rvf $(APP_DIR)*

