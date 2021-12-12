.globl timer_vec
.align 4
timer_vec:
    # scratch memory contains hart-local context that we only access in the M
    # mode. We set appropriate values for it in `start.c`.
    csrrw a0, mscratch, a0 # Put scratch memory address for this hart in `a0`.
    ld a1, 24(a0)          # Load the CLINT_MTIMECMP address of the current hart.
    ld a2, 32(a0)          # Load the interval.
    ld a3, 0(a1)           # Load the value inside CLINT_MTIMECMP.
    add a3, a3, a2         # Add the interval.
    sd a3, 0(a1)           # Re-store the value.

    sd a1, 0(a0)
    li a1, 69             # indicate to ktrap that it's a timer interrupt
    call ktrap

    mret
