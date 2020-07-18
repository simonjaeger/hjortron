SRC_FOLDER := src
OBJ_FOLDER := obj
SRC := $(shell find $(SRC_FOLDER)/ -type f -name '*.s')
OBJ := $(patsubst %.s, %.o, $(patsubst $(SRC_FOLDER)/%, $(OBJ_FOLDER)/%, $(SRC)))

ASM = nasm
LD = ${HOME}/opt/htools/bin/i686-elf-ld

all: $(OBJ)
	mkdir -p bin
	$(LD) obj/boot.o -T boot.ld -o bin/boot.bin
	$(LD) obj/test.o -T test.ld -o bin/test.bin

$(OBJ_FOLDER)/%.o: $(SRC_FOLDER)/%.s
	mkdir -p $(@D)
	$(ASM) -f elf32 $< -o $@

.PHONY: clean
clean:
	rm obj -rf
	rm bin -rf