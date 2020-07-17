C_SRC = ${wildcard *.c} ${wildcard **/*.c} ${wildcard **/**/*.c}
ASM_SRC = ${wildcard *.s} ${wildcard **/*.s} ${wildcard **/**/*.s}
OBJ := $(patsubst %.c, %.o, $(C_SRC)) $(patsubst %.s, %.o, $(ASM_SRC)) 

C = ${HOME}/opt/htools/bin/i686-elf-gcc
CFLAGS = -nostdlib -ffreestanding -mno-red-zone -fno-exceptions -nostdlib -Wall -Wextra -Werror -I ../../include/kernel/
ASM = nasm
ASMFLAGS = -f elf32

all: $(OBJ)

%.o: %.c
	$(C) $(CFLAGS) -c -o $@ $^

%.o: %.s
	$(ASM) $(ASMFLAGS) $< -o $@

.PHONY: clean
clean:
	rm *.o -f
	rm **/*.o -f
	rm **/**/*.o -f
	rm **/**/**/*.o -f