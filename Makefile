all: boot kernel libc apps

fdd: boot kernel libc apps
	mkdir -p bin
	sudo ./scripts/build-fdd.sh

hdd: boot kernel libc apps
	mkdir -p bin
	sudo ./scripts/build-hdd.sh

.PHONY: boot
boot:
	+$(MAKE) -C boot

.PHONY: kernel
kernel:
	+$(MAKE) -C kernel

.PHONY: libc
libc: kernel
	+$(MAKE) -C libc

.PHONY: apps
apps: kernel libc
	+$(MAKE) -C apps

.PHONY: clean
clean:
	+$(MAKE) -C boot clean
	+$(MAKE) -C kernel clean
	+$(MAKE) -C libc clean
	+$(MAKE) -C apps clean
	rm bin -rf