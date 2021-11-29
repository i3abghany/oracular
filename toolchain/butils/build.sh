#!/bin/bash

SATURN_TOP_LEVEL=$(git rev-parse --show-toplevel)
SATURN_BINUTILS_DIR=$SATURN_TOP_LEVEL/toolchain/butils

EXEC_DIR=$(pwd)

if [ "$EXEC_DIR" != "$SATURN_BINUTILS_DIR" ]; then
    echo 'script must be run from toolchain/butils dir.'
    exit
fi

if [ "$1" = "clean" ]; then
    # rm -rf bintuils-src
    rm -rf binutils-build
    exit
fi

which riscv64-elf-ld > /dev/null && exit

if [ "$PREFIX" = "" ]; then
    echo "export PREFIX=\"\$HOME/opt/cross\"" >> ~/.bashrc
    echo "export TARGET=\"riscv64-elf\"" >> ~/.bashrc
    echo "export PATH=\"\$PREFIX/bin:\$PATH\"" >> ~/.bashrc
    source ~/.bashrc
fi

if ! [ -d binutils-src ]; then
    mkdir -v binutils-src
    cd binutils-src

    echo 'Fetching binutils version 2.36.1'
    curl -O https://ftp.cc.uoc.gr/mirrors/gnu/binutils/binutils-2.36.1.tar.gz

    echo 'Extracting binutils'
    tar xf binutils-2.36.1.tar.gz

    cd ..
fi

mkdir -p -v binutils-build
cd binutils-build

../binutils-src/binutils-2.36.1/configure \
    --target=$TARGET                      \
    --prefix=$PREFIX                      \
    --with-sysroot                        \
    --disable-nls                         \
    --disable-werror

make -j$(nproc)
make install -j$(nproc)
