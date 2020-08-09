SRC_FOLDER := src
OBJ_FOLDER := obj
BIN_FOLDER := bin

SRC = $(shell find $(SRC_FOLDER)/ -type f -name '*.[cs]')
OBJ := $(patsubst %.s, %.o, $(patsubst %.c, %.o, $(patsubst $(SRC_FOLDER)/%, $(OBJ_FOLDER)/%, $(SRC)))) \
	../kernel/obj/string.o ../kernel/obj/display/display.o

C = ${HOME}/opt/htools/bin/i686-elf-gcc
CFLAGS = -nostdlib -ffreestanding -mno-red-zone -fno-exceptions -nostdlib -Wall -Wextra -Werror -I ../kernel/include/
LD = ${HOME}/opt/htools/bin/i686-elf-ld

all: bin/test.elf

bin/test.elf: $(OBJ)
	mkdir -p $(BIN_FOLDER)
	$(LD) $(OBJ) -T linker.ld -r -o $@

$(OBJ_FOLDER)/%.o: $(SRC_FOLDER)/%.c
	mkdir -p $(@D)
	$(C) $(CFLAGS) -c -o $@ $^

$(OBJ_FOLDER)/%.o: $(SRC_FOLDER)/%.s
	mkdir -p $(@D)
	$(ASM) $(ASMFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(OBJ_FOLDER) -rf
	rm $(BIN_FOLDER) -rf