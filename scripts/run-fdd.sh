#!/bin/sh

make -C ../ clean
make -C ../ fdd
qemu-system-x86_64.exe -fda ../bin/fdd.img -serial stdio