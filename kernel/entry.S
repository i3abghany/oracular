.section .init
.option norvc
 
.global start
start:
	.cfi_startproc

    # Initialize a stack for only one hart.
    la sp, stack_base

    # Zero out the BSS section.
	la 		a0, bss_start
	la		a1, bss_end
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

    call kmain

halt:
    wfi
    j halt

    .cfi_endproc

.end
