#include <kernel/plic.h>

void plic_init()
{
    *PLIC_IRQ_PRIORITY(PLIC_VIRTIO_IRQ) = 1;
    *PLIC_IRQ_PRIORITY(PLIC_UART0_IRQ) = 1;
    plic_enable_irq(PLIC_VIRTIO_IRQ);
    plic_enable_irq(PLIC_UART0_IRQ);
    *PLIC_PTHREASHOLD = 0;
}

void plic_enable_irq(int irq)
{
    *PLIC_ENABLE |= (1 << irq);
}

int plic_claim()
{
    return *PLIC_CLAIM;
}

void plic_acknowledge(int irq)
{
    *PLIC_CLAIM = irq;
}
