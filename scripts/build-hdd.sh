#!/bin/sh

# Create loopback device.
dd if=/dev/zero of=bin/hdd.img bs=1048576 count=32
sudo losetup /dev/loop0 bin/hdd.img

# Format as FAT12.
sudo mkdosfs -F 12 -n HJORTON /dev/loop0

# Write bootloader.
dd if=boot/bin/boot.bin skip=62 of=bin/hdd.img bs=512 count=450 iflag=skip_bytes,count_bytes seek=62 oflag=seek_bytes conv=notrunc

# Mount device.
sudo mount /dev/loop0 /mnt -t msdos -o "fat=12"

# Copy files.
sudo cp boot/bin/init.bin /mnt
sudo cp kernel/bin/kernel.bin /mnt
sudo mkdir /mnt/data

# Unmount and destroy device.
sudo umount /mnt
sudo losetup -d /dev/loop0