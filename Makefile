all: boot kernel

fdd: boot kernel
	mkdir -p bin
	sudo ./scripts/build-fdd.sh

.PHONY: boot
boot:
	+$(MAKE) -C boot

.PHONY: kernel
kernel:
	+$(MAKE) -C kernel

.PHONY: clean
clean:
	+$(MAKE) -C boot clean
	+$(MAKE) -C kernel clean
	rm bin -rf