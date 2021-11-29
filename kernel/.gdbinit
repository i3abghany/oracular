set confirm off
set architecture riscv:rv64
target remote localhost:1234
symbol-file kernel.elf
