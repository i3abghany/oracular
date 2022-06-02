#include <kernel/console.h>
#include <kernel/kassert.h>
#include <kernel/kmem.h>
#include <kernel/plic.h>
#include <kernel/rv.h>
#include <kernel/virtio.h>
#include <kernel/vm.h>
#include <stdint.h>

void kmain(void)
{
    uart0_init();
    puts("Oracular Spectacular\n");

    kprintf("enabling traps...");
    void trap_init();
    trap_init();
    kprintf("done\n");

    kprintf("initializing kernel memory...");
    kmem_init();
    kprintf("done\n");

    kvm_init();

    kprintf("enabling supervisor software interrupts...");
    set_sstatus(get_sstatus() | (1 << 1));
    kprintf("done\n");

    plic_init();

    void virtio_blk_init(void);
    virtio_blk_init();

    while (1)
        ;
}
