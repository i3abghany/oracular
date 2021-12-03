.section .init
.option norvc
 
.global start
start:
	.cfi_startproc

    /* init a stack for only one hart and then call kmain in `kernel.c`. */
    la sp, stack_base

	la 		a0, bss_start
	la		a1, bss_end
	bgeu	a0, a1, bss_init_loop_end
bss_init_loop:
	sd		zero, (a0)
	addi	a0, a0, 8
	bltu	a0, a1, bss_init_loop
    
bss_init_loop_end:

    call kmain

halt:
    wfi
    j halt

    .cfi_endproc

.end
