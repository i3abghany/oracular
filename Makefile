ARCH = riscv64
TOOLCHAIN_PREFIX = ${HOME}/opt/cross/bin

ifneq (, $(shell which riscv64-elf-gcc))
CC = riscv64-elf-gcc
else ifneq (, $(shell which riscv64-unknown-elf-gcc))
CC = riscv64-unknown-elf-gcc
else
CC = $(TOOLCHAIN_PREFIX)/riscv64-elf-gcc
$(info Defaulting gcc path to $(CC))
endif

ifneq (, $(shell which riscv64-elf-ld))
LD = riscv64-elf-ld
else ifneq (, $(shell which riscv64-unknown-elf-ld))
LD = riscv64-unknown-elf-ld
else
LD = $(TOOLCHAIN_PREFIX)/riscv64-elf-ld
$(info Defaulting ld path to $(LD))
endif

ifneq (, $(shell which riscv64-elf-as))
AS = riscv64-elf-as
else ifneq (, $(shell which riscv64-unknown-elf-as))
AS = riscv64-unknown-elf-as
else
AS = $(TOOLCHAIN_PREFIX)/riscv64-elf-as
$(info Defaulting as path to $(AS))
endif

QEMU = qemu-system-riscv64

CFLAGS =
CFLAGS += -Wall
CFLAGS += -O3
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

DEBUG ?=

ifeq ($(DEBUG),ON)
CFLAGS += -DKERNEL_DEBUG
endif

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

KSRC_FILES = kernel/kernel.c    \
			 kernel/uart.c      \
			 kernel/console.c   \
			 kernel/rv.c        \
			 kernel/prekernel.c \
			 kernel/kalloc.c    \
			 kernel/vm.c        \
			 kernel/trap.c

KASM_FILES = kernel/entry.S     \
			 kernel/kvec.S      \
			 kernel/tvec.S

KOBJ_FILES = $(patsubst %.c, %.o, $(KSRC_FILES))
KOBJ_FILES += $(patsubst %.S, %.o, $(KASM_FILES))

qemu: kernel/kernel.elf
	$(QEMU) $(QEMU_FLAGS) -kernel $<

gdb: QEMU_FLAGS += -s -S
gdb: QEMU_FLAGS += -d int
gdb: QEMU_FLAGS += -no-reboot
gdb: QEMU_FLAGS += -no-shutdown
gdb: kernel/kernel.elf
	$(QEMU) -kernel $< $(QEMU_FLAGS)

kernel/kernel.elf: $(KOBJ_FILES)
	echo $(KOBJ_FILES)
	$(LD) $(LDFLAGS) $(KOBJ_FILES) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.S
	$(AS) -g $< -o $@

.PHONY: clean
clean:
	rm -f kernel/*.o kernel/*.elf kernel/*.d
