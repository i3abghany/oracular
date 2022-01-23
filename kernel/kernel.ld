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

    .data : ALIGN(4K) {
        PROVIDE(_data_start = .);
        *(.data .data.*)
        . = ALIGN(16);
        *(.sdata .sdata.*)
        . = ALIGN(4K);
        PROVIDE(_data_end = .);
    } >ram

    .rodata : ALIGN(4K) {
        PROVIDE(_rodata_start = .);
        *(.rodata .rodata.*)
        . = ALIGN(16);
        *(.srodata .srodata.*)
        . = ALIGN(4K);
        PROVIDE(_rodata_end = .);
    } >ram

    .bss : ALIGN(4K) {
        PROVIDE(_bss_start = .);
        *(.bss .bss.*)
        . = ALIGN(4K);
        PROVIDE(_stack_start = .);
        . += 0x10000;
        PROVIDE(_stack_end = .);
        PROVIDE(_bss_end = .);
    } >ram

    . = ALIGN(4K);

    PROVIDE(_kernel_mem_end = .);
    PROVIDE(_physical_mem_end = ORIGIN(ram) + LENGTH(ram));
} 

