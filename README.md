# OracularOS

Oracular is a minimal unix-like operating system for the RISC-V ISA. The goal is
to learn about operating systems development and employ prominent systems
literature in a real-world project.

## Build instructions

Currently, we're only experimenting on Ubuntu Linux, so the instructions are
based on it.

### Install prerequisites

```console
sudo apt install make git curl qemu-system-misc libmpfr-dev libmpc-dev libgmp-dev
```

### Clone the project

```console
git clone https://github.com/i3abghany/oracular.git
cd oracular
```

### Build Binutils and GCC

Download, build, and install GCC cross compiler for RISCV64. This is automated
using the script `toolchain/build.sh` (which must be executed from the
`toolchain` directory). After the process is done, GCC and binutils will be
installed in the directory `$HOME/opt/cross/bin`.

```console
cd toolchain
./build.sh
```

### Compile the kernel

By using the top-level Makefile, we can compile the kernel and load it into
QEMU.

```console
make qemu
```

## Debugging

The Makefile has a target that builds the kernel, loads it into QEMU, and
attaches GDB for debugging.

```console
make gdb
```
