#ifndef _KERNEL_TRAP_H
#define _KERNEL_TRAP_H

#define N_IRQS 16

#define SUPERVISOR_SOFTWARE_INTERRUPT 1
#define SUPERVISOR_EXTERNAL_INTERRUPT 9

typedef void (*isr_t)(void);

void register_isr(uint32_t irq, isr_t);

#endif
