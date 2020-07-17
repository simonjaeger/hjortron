SRC = boot16.s boot32.s
OBJ := $(patsubst %.s, %.o, $(SRC))

ASM = nasm
ASMFLAGS = -f elf32

all: $(OBJ)

%.o: %.s
	$(ASM) $(ASMFLAGS) $< -o $@

.PHONY: clean
clean:
	rm *.o -f
	rm **/*.o -f