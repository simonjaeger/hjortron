all: boot kernel

fdd: boot kernel apps
	mkdir -p bin
	sudo ./scripts/build-fdd.sh

hdd: boot kernel apps
	mkdir -p bin
	sudo ./scripts/build-hdd.sh

.PHONY: boot
boot:
	+$(MAKE) -C boot

.PHONY: kernel
kernel:
	+$(MAKE) -C kernel

.PHONY: apps
apps: kernel
	+$(MAKE) -C apps

.PHONY: clean
clean:
	+$(MAKE) -C boot clean
	+$(MAKE) -C kernel clean
	+$(MAKE) -C apps clean
	rm bin -rf