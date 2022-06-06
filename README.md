# OracularOS

Oracular is a minimal UNIX-like operating system for RISC-V platforms. The goal
is to learn about operating systems development and employ prominent systems
literature in a real-world project.

## Build instructions

Currently, we're only experimenting on Ubuntu Linux, so the instructions are
based on it.

### Install prerequisites

```console
sudo apt install make git curl qemu-system-misc libmpfr-dev libmpc-dev libgmp-dev gdb-multiarch clang-format python
```

### Clone the project

```console
git clone https://github.com/i3abghany/oracular.git
cd oracular
```

### Build Binutils and GCC

Download, build, and install GCC cross compiler for RISCV64. This is automated
using the script `toolchain/build.sh` (which must be executed from the
`toolchain` directory). After the process is done, GCC and Binutils will be
installed in the directory `$HOME/opt/cross/bin`.

```console
cd toolchain
./build.sh
```

### Compile the kernel

By using the top-level Makefile, you can compile the kernel and load it into
QEMU.

```console
make qemu
```

### Debugging

The Makefile has a target that builds the kernel, loads it into QEMU with
debugging options and then hangs waiting for GDB to connect. First, invoke the
`gdb` Makefile target (It's also advised to turn on the `DEBUG` switch to turn off
-O3, which makes debugging using GDB dreadful.)

```console
make gdb DEBUG=ON
```

And then invoke `gdb-multiarch` in the repository top-level directory in another
terminal session.

Note that loading `.gdbinit` automatically will initially fail. You'll have to
add the following line to `${HOME}/.gdbinit`.

```console
add-auto-load-safe-path /PATH/TO/ORACULAR/.gdbinit
```

And substitute the path with the appropriate oracular repository location.
