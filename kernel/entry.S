.section .init
.option norvc

.global start
start:
    .cfi_startproc

    # Initialize a kernel stack for only one hart.
    la sp, _stack_end

    # Zero out the BSS section.
    la 		a0, _bss_start
    la		a1, _bss_end
    bgeu	a0, a1, bss_init_loop_end
bss_init_loop:
    sd		zero, (a0)
    addi	a0, a0, 8
    bltu	a0, a1, bss_init_loop

bss_init_loop_end:

    # Only jump to `kmain` for hart #0 for now.
    csrr    a1, mhartid
    li      a2, 1
    bgeu    a1, a2, halt

    /* prekernel sets up the hardware and then jumps to supervisor mode and
     * calls `kmain` */
    call prekernel

halt:
    wfi
    j halt

    .cfi_endproc

.end
