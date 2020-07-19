#!/bin/sh

# Create loopback device.
dd if=/dev/zero of=bin/fdd.img bs=512 count=2880
sudo losetup /dev/loop0 bin/fdd.img

# Format as FAT12.
sudo mkdosfs -F 12 /dev/loop0

# Write bootloader.
dd if=boot/bin/boot.bin skip=62 of=bin/fdd.img bs=512 count=450 iflag=skip_bytes,count_bytes seek=62 oflag=seek_bytes conv=notrunc

# Mount device.
sudo mount /dev/loop0 /mnt -t msdos -o "fat=12"

# Copy files.
sudo cp boot/bin/test.bin /mnt
sudo cp boot/bin/test.bin /mnt/test2.bin
# sudo cp kernel/bin/kernel.bin /mnt

# Unmount and destroy device.
sudo umount /mnt
sudo losetup -d /dev/loop0