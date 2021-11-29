.section .init
.option norvc
 
.global start
start:
	.cfi_startproc

    /* init a stack for only one hart and then call kmain in `kernel.c`. */
    la sp, stack_base
    call kmain

halt:
    wfi
    j halt

    .cfi_endproc

.end
