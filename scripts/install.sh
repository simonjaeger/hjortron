#!/bin/sh

# https://wiki.osdev.org/GCC_Cross-Compiler
# Run with sudo ./install.sh.

# TODO: Install for user instead...

# Configuration.
export SOURCE="$HOME/src"
export DESTINATION="$HOME/opt/htools"
export TARGET=i686-elf
export PATH="$DESTINATION/bin:$PATH"
export BINUTILS_VERSION=2.34
export GCC_VERSION=10.1.0

# Prerequisites.
apt-get update
apt-get install nasm -y
apt-get install genisoimage -y
apt-get install build-essential -y
apt-get install bison -y
apt-get install flex -y
apt-get install libgmp3-dev -y
apt-get install libmpc-dev -y
apt-get install libmpfr-dev -y
apt-get install texinfo -y

# Get Binutils.
wget "https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.xz" -P $SOURCE
tar -xvf "$SOURCE/binutils-$BINUTILS_VERSION.tar.xz" -C $SOURCE

# Build Binutils.
mkdir "$SOURCE/build-binutils"
cd "$SOURCE/build-binutils"
"../binutils-$BINUTILS_VERSION/configure" --target=$TARGET --prefix=$DESTINATION --with-sysroot --disable-nls --disable-werror
make
make install

# Clean Binutils.
rm "$SOURCE/binutils-$BINUTILS_VERSION.tar.xz" -f
rm "$SOURCE/binutils-$BINUTILS_VERSION" -rf
rm "$SOURCE/build-binutils" -rf

# Get GCC.
wget "https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz" -P $SOURCE
tar -xvf "$SOURCE/gcc-$GCC_VERSION.tar.gz" -C $SOURCE

# Build GCC.
mkdir "$SOURCE/build-gcc"
cd "$SOURCE/build-gcc"
"../gcc-$GCC_VERSION/configure" --target=$TARGET --prefix=$DESTINATION --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc

# Clean GCC.
rm "$SOURCE/gcc-$GCC_VERSION.tar.gz" -f
rm "$SOURCE/gcc-$GCC_VERSION" -rf
rm "$SOURCE/build-gcc" -rf
