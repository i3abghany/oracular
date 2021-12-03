#!/bin/bash

set -eo pipefail

ORACULAR_TOP_LEVEL=$(git rev-parse --show-toplevel)
ORACULAR_TOOLCHAIN_DIR=$ORACULAR_TOP_LEVEL/toolchain

EXEC_DIR=$(pwd)

if [ "$EXEC_DIR" != "$ORACULAR_TOOLCHAIN_DIR" ]; then
    echo "script must be run from $ORACULAR_TOOLCHAIN_DIR dir."
    exit
fi

export PATH="$PATH:$HOME/opt/cross/bin"

cd butils
./build.sh

cd ../gcc
./build.sh

cd ..
