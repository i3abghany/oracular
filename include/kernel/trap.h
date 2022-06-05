#ifndef _KERNEL_TRAP_H
#define _KERNEL_TRAP_H

#define N_IRQS 16

typedef void (*isr_t)(void);

void register_isr(uint32_t irq, isr_t);

#endif
