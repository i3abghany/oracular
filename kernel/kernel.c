#include <kernel/console.h>
#include <kernel/kassert.h>
#include <kernel/kmem.h>
#include <kernel/list.h>
#include <kernel/plic.h>
#include <kernel/rv.h>
#include <kernel/slab_alloc.h>
#include <kernel/virtio_blk.h>
#include <kernel/vm.h>
#include <stdint.h>

uint8_t iiimem[512];

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

    virtio_blk_init();

    virtio_blk_request(VIRTIO_BLK_T_IN, 0, iiimem);

#ifdef KERNEL_TEST
    void slab_alloc_tests();
    slab_alloc_tests();
    qemu_virt_shutdown(PASS);
#endif

    while (1)
        ;
}
