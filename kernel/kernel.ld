OUTPUT_ARCH("riscv");
ENTRY(start);

SECTIONS {
     /* The -kernel QEMU option copies the image to that specific address. */
    . = 0x80000000;

    .text : ALIGN(4K) {
        *(.init)
        *(.text .text.*)
        PROVIDE(text_end = .);
    }

    .bss : ALIGN(4K) {
        PROVIDE(bss_start = .);
        *(.bss .bss.*)
        . += 4096;
        PROVIDE(stack_base = .);
        PROVIDE(bss_end = .);
    }

    .rodata : ALIGN(4K) {
        *(.rodata .rodata.*)
    }

    .data : ALIGN(4K) {
        *(.data .data.*)
    }

    PROVIDE(end = .);
} 

