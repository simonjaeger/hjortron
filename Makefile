OBJ = ${wildcard src/boot/*.o} \
	${wildcard src/kernel/*.o} \
	${wildcard src/kernel/**/*.o} \
	${wildcard src/kernel/**/**/*.o}

CC = ${HOME}/opt/htools/bin/i686-elf-g++
CFLAGS = -nostdlib -ffreestanding -std=c++11 -mno-red-zone -fno-exceptions -nostdlib -fno-rtti -Wall -Wextra -Werror

bin: boot kernel
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/os512.bin -Xlinker --defsym=SECTOR_SIZE=512 -T linker.ld $(OBJ)
	$(CC) $(CFLAGS) -o bin/os2048.bin -Xlinker --defsym=SECTOR_SIZE=2048 -T linker.ld $(OBJ)

.PHONY: boot
boot:
	+$(MAKE) -C src/boot

.PHONY: kernel
kernel:
	+$(MAKE) -C src/kernel

.PHONY: iso
iso: bin
	rm iso -rf
	mkdir iso
	cp bin/os2048.bin iso/os2048.bin
	mkisofs -V "OS" -J -r -b os2048.bin -no-emul-boot -boot-load-size 4 -boot-info-table -o bin/os.iso ./iso
	rm iso -rf

.PHONY: clean
clean:
	+$(MAKE) -C src/boot clean
	+$(MAKE) -C src/kernel clean
	rm bin -rf
	rm iso -rf