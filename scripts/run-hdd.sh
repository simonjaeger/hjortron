#!/bin/sh

make -C ../ clean
# make -C ../ hdd
make -C ../ hdd
qemu-system-x86_64.exe -drive file=../bin/hdd.img,if=floppy,format=raw -serial stdio
# qemu-system-x86_64.exe -drive file=../bin/hdd.img,if=ide,format=raw -serial stdio