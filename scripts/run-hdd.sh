#!/bin/sh

set -e

make -C ../ clean
make -C ../ hdd
qemu-system-x86_64.exe -drive file=../bin/hdd.img,if=ide,format=raw -serial stdio