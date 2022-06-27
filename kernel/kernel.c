#include <kernel/console.h>
#include <kernel/kassert.h>
#include <kernel/kmem.h>
#include <kernel/kthread.h>
#include <kernel/list.h>
#include <kernel/plic.h>
#include <kernel/rv.h>
#include <kernel/slab_alloc.h>
#include <kernel/virtio_blk.h>
#include <kernel/vm.h>
#include <libc/string.h>
#include <stdint.h>

void thread3_proc()
{
    while (1) {
        kprintf("In Thread 3\n");
        for (int i = 0; i < 200000000; i++) {
            asm volatile("nop");
        }
    }
}

void thread1_proc()
{
    while (1) {
        kprintf("In Thread 1\n");
        for (int i = 0; i < 200000000; i++) {
            asm volatile("nop");
        }
    }
}

void thread2_proc()
{
    while (1) {
        kprintf("In Thread 2\n");
        for (int i = 0; i < 200000000; i++) {
            asm volatile("nop");
        }
    }
}

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

    kprintf("Initializing the platform-level interrupt controller (PLIC)...");
    plic_init();
    kprintf("done\n");

    kprintf("Initializing the VirtIO secondary storage block device...");
    virtio_blk_init();
    kprintf("done\n");

    add_thread(thread1_proc);
    add_thread(thread2_proc);
    add_thread(thread3_proc);

#ifdef KERNEL_TEST
    void slab_alloc_tests();
    slab_alloc_tests();
    void virtio_blk_tests();
    virtio_blk_tests();
    // qemu_virt_shutdown(PASS);
#endif

    while (1)
        ;
}
