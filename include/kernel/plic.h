#ifndef _KERNEL_PLIC_H
#define _KERNEL_PLIC_H

#include <stdint.h>

#define PLIC_BASE 0x0C000000

#define PLIC_PRIORITY    (uint32_t *) (PLIC_BASE + 0)
#define PLIC_PENDING     (uint32_t *) (PLIC_BASE + 0x1000)
#define PLIC_ENABLE      (uint32_t *) (PLIC_BASE + 0x2080)
#define PLIC_PTHREASHOLD (uint32_t *) (PLIC_BASE + 0x201000)
#define PLIC_CLAIM       (uint32_t *) (PLIC_BASE + 0x201004)

#define PLIC_UART0_IRQ 10

#define PLIC_IRQ_PRIORITY(irq_) (uint32_t *) (PLIC_PRIORITY + irq_)

void plic_init();
void plic_enable_irq(int irq);
int plic_claim();
void plic_acknowledge(int irq);

#endif
