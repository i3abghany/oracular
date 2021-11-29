CC = riscv64-elf-gcc
LD = riscv64-elf-ld
AS = riscv64-elf-as
QEMU = qemu-system-riscv64

CFLAGS =
CFLAGS += -Wall
CFLAGS += -march=rv64g
CFLAGS += -Wextra
CFLAGS += -Werror
CFLAGS += -pedantic
CFLAGS += -mcmodel=medany
CFLAGS += -ffreestanding
CFLAGS += -c
CFLAGS += -ggdb
CFLAGS += -fno-builtin
CFLAGS += -mno-relax
CFLAGS += -fno-common

ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif

LDFLAGS =
LDFLAGS += -T kernel.ld
LDFLAGS += -nostdlib

QEMU_FLAGS =
QEMU_FLAGS += -machine virt
QEMU_FLAGS += -smp 1
QEMU_FLAGS += -bios none
QEMU_FLAGS += -m 128
QEMU_FLAGS += -serial mon:stdio
QEMU_FLAGS += -nographic

SRC_FILES = kernel.c

OBJ_FILES = $(patsubst %.c, %.o, $(SRC_FILES))
OBJ_FILES += entry.o

qemu: kernel.elf
	$(QEMU) $(QEMU_FLAGS) -kernel $<

# Not working yet...
debug: QEMU_FLAGS += -s -S
debug: QEMU_FLAGS += -d int
debug: QEMU_FLAGS += -no-reboot
debug: QEMU_FLAGS += -no-shutdown
debug: kernel.elf
	setsid $(QEMU) $(QEMU_FLAGS) -kernel $< & true
	gdb-multiarch -x .gdbinit

kernel.elf: $(SRC_FILES) entry.S
	$(CC) $(CFLAGS) $(SRC_FILES) -o kernel.o
	$(AS) entry.S -o entry.o
	$(LD) $(LDFLAGS) $(OBJ_FILES) -o $@

.PHONY: clean
clean:
	rm -f *.o *.elf *.d
