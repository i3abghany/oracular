.globl timer_vec
.align 4
timer_vec:
    # scratch memory contains hart-local context that we only access in the
    # machine mode. We set appropriate values for it in `prekernel.c`.
    csrrw a0, mscratch, a0 # Put scratch memory address for this hart in `a0`.

    sd a1, 0(a0)           # Push the used registers in the empty scratch part.
    sd a2, 8(a0)
    sd a3, 16(a0)

    ld a1, 24(a0)          # Load the CLINT_MTIMECMP address of the current hart.
    ld a2, 32(a0)          # Load the interval.
    ld a3, 0(a1)           # Load the value inside CLINT_MTIMECMP.
    add a3, a3, a2         # Add the interval.
    sd a3, 0(a1)           # Re-store the value.

    li a1, (1 << 1)
    csrw sip, a1           # Set software interrupt pending.

    ld a1, 0(a0)           #
    ld a2, 8(a0)           # Restore state.
    ld a3, 16(a0)          #
    csrrw a0, mscratch, a0

    mret
