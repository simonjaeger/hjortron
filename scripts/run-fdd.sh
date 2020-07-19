#!/bin/sh

make -C ../ fdd
qemu-system-x86_64.exe -drive file=../bin/fdd.img,if=floppy,format=raw -serial stdio