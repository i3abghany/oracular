#!/bin/bash

set -eo pipefail

ORACULAR_TOP_LEVEL=$(git rev-parse --show-toplevel)
ORACULAR_BINUTILS_DIR=$ORACULAR_TOP_LEVEL/toolchain/binutils

EXEC_DIR=$(pwd)

if [ "$EXEC_DIR" != "$ORACULAR_BINUTILS_DIR" ]; then
    echo "script must be run from the $ORACULAR_BINUTILS_DIR dir."
    exit
fi

if [ "$1" = "clean" ]; then
    rm -rf binutils-build
    rm -rf binutils-src
    exit
fi

PREFIX=$HOME/opt/cross
TARGET=riscv64-elf

export PATH="$PATH:$HOME/opt/cross/bin"
which $TARGET-ld > /dev/null && { echo "$TARGET binutlis already installed."; exit; }

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
    --target="$TARGET"                    \
    --prefix="$PREFIX"                    \
    --with-sysroot                        \
    --disable-nls                         \
    --disable-werror || echo "Binutils source corrupted. please remove the
    binutils/binutils-src directory and rerun the script.";

make -j$(nproc)
make install -j$(nproc)
