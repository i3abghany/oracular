ARCH = riscv64
TOOLCHAIN_PREFIX = ${HOME}/opt/cross/bin
CC = $(TOOLCHAIN_PREFIX)/riscv64-elf-gcc
LD = $(TOOLCHAIN_PREFIX)/riscv64-elf-ld
AS = $(TOOLCHAIN_PREFIX)/riscv64-elf-as
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
CFLAGS += -Iinclude

ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif

LDFLAGS =
LDFLAGS += -T kernel/kernel.ld
LDFLAGS += -nostdlib

QEMU_FLAGS =
QEMU_FLAGS += -machine virt
QEMU_FLAGS += -smp 1
QEMU_FLAGS += -bios none
QEMU_FLAGS += -m 128
QEMU_FLAGS += -serial mon:stdio
QEMU_FLAGS += -nographic
QEMU_FLAGS += -device virtio-keyboard-device

KSRC_FILES = kernel/kernel.c kernel/uart.c kernel/console.c kernel/rv.c

KOBJ_FILES = $(patsubst %.c, %.o, $(KSRC_FILES))
KOBJ_FILES += kernel/entry.o

qemu: kernel/kernel.elf
	$(QEMU) $(QEMU_FLAGS) -kernel $<

gdb: QEMU_FLAGS += -s -S
gdb: QEMU_FLAGS += -d int
gdb: QEMU_FLAGS += -no-reboot
gdb: QEMU_FLAGS += -no-shutdown
gdb: kernel/kernel.elf
	setsid $(QEMU) $(QEMU_FLAGS) -kernel $< & true
	gdb-multiarch -x .gdbinit

kernel/kernel.elf: $(KOBJ_FILES)
	$(LD) $(LDFLAGS) $(KOBJ_FILES) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.S
	$(AS) $< -o $@

.PHONY: clean
clean:
	rm -f kernel/*.o kernel/*.elf kernel/*.d
