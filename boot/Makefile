SRC_FOLDER := src
OBJ_FOLDER := obj
BIN_FOLDER := bin

SRC := $(shell find $(SRC_FOLDER)/ -type f -name '*.s')
OBJ := $(patsubst %.s, %.o, $(patsubst $(SRC_FOLDER)/%, $(OBJ_FOLDER)/%, $(SRC)))

ASM = nasm
ASMFLAGS = -f elf32
LD = ${HOME}/opt/htools/bin/i686-elf-ld

all: $(OBJ)
	mkdir -p $(BIN_FOLDER)
	$(LD) obj/boot.o obj/print16.o -T boot.ld -o bin/boot.bin
	$(LD) obj/init16.o obj/init32.o obj/print16.o  obj/print32.o -T init.ld -o bin/init.bin

$(OBJ_FOLDER)/%.o: $(SRC_FOLDER)/%.s
	mkdir -p $(@D)
	$(ASM) $(ASMFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(OBJ_FOLDER) -rf
	rm $(BIN_FOLDER) -rf