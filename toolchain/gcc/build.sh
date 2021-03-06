#!/bin/bash

set -eo pipefail

ORACULAR_TOP_LEVEL=$(git rev-parse --show-toplevel)
ORACULAR_GCC_DIR=$ORACULAR_TOP_LEVEL/toolchain/gcc

EXEC_DIR=$(pwd)

if [ "$EXEC_DIR" != "$ORACULAR_GCC_DIR" ]; then
    echo "script must be run from the $ORACULAR_GCC_DIR dir."
    exit
fi

if [ "$1" = "clean" ]; then
    rm -rf gcc-build
    rm -rf gcc-src
    exit
fi

export PATH="$PATH:$HOME/opt/cross/bin/"
which riscv64-linux-gcc > /dev/null && exit

PREFIX=$HOME/opt/cross
TARGET=riscv64-elf

if ! [ -d gcc-src ]; then
    mkdir -v gcc-src
    cd gcc-src

    echo 'Fetching gcc version 11.1.0'
    curl -O https://ftp.gnu.org/gnu/gcc/gcc-11.1.0/gcc-11.1.0.tar.gz

    echo 'Extracting gcc'
    tar xf gcc-11.1.0.tar.gz

    cd ..
fi

which $TARGET-as || { echo "Could not find $TARGET-as; exitting"; exit 1; }

pushd gcc-src/gcc-11.1.0/
./contrib/download_prerequisites
popd

mkdir -p -v gcc-build
cd gcc-build

../gcc-src/gcc-11.1.0/configure \
    --target="$TARGET"          \
    --prefix="$PREFIX"          \
    --disable-nls               \
    --without-headers           \
    --enable-languages=c,c++ || echo "GCC source corrupted. please remove the
    gcc/gcc-src directory and rerun the script.";

make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make install-gcc -j$(nproc)
make install-target-libgcc -j$(nproc)
