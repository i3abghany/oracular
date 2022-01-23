OUTPUT_ARCH("riscv");
ENTRY(start);

MEMORY {
 /* The -kernel QEMU option copies the image to that specific address. */
  ram (wxa) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS {
    /* Starts at the address 0x80000000. */

    .text : ALIGN(4K) {
        PROVIDE(_text_start = .);
        *(.init)
        *(.text .text.*)
        . = ALIGN(4K);
        PROVIDE(_text_end = .);
    } >ram

    .rodata : ALIGN(4K) {
        *(.rodata .rodata.*)
    } >ram

    .data : ALIGN(4K) {
        *(.data .data.*)
    } >ram

    .bss : ALIGN(4K) {
        PROVIDE(bss_start = .);
        *(.bss .bss.*)
        . += 4096;
        PROVIDE(stack_base = .);
        PROVIDE(bss_end = .);
    } >ram

    . = ALIGN(4K);

    PROVIDE(_kernel_mem_end = .);
    PROVIDE(_physical_mem_end = ORIGIN(ram) + LENGTH(ram));
} 

