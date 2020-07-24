#!/bin/sh

# Create loopback device.
# dd if=/dev/zero of=bin/hdd.img bs=1048576 count=32
dd if=/dev/zero of=bin/hdd.img bs=1048576 count=4
sudo losetup /dev/loop0 bin/hdd.img

# Format as FAT12.
sudo mkdosfs -F 12 -n HJORTRON /dev/loop0

# Write bootloader.
dd if=boot/bin/boot.bin skip=62 of=bin/hdd.img bs=512 count=450 iflag=skip_bytes,count_bytes seek=62 oflag=seek_bytes conv=notrunc

# Mount device.
sudo mount /dev/loop0 /mnt -t msdos -o "fat=12"

# Copy files.
sudo cp boot/bin/init.bin /mnt
sudo cp kernel/bin/kernel.bin /mnt
sudo mkdir /mnt/data1
sudo mkdir /mnt/data1/data2
sudo echo "test1" > /mnt/data1/test1.txt
sudo echo "test2" > /mnt/data1/data2/test2.txt
sudo cp scripts/lorem.txt /mnt/data1/data2

# Unmount and destroy device.
sudo umount /mnt
sudo losetup -d /dev/loop0