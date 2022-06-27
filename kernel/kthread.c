#include <kernel/console.h>
#include <kernel/kmem.h>
#include <kernel/kthread.h>
#include <libc/stddef.h>

struct thread_cxt thread_pool[N_THREADS];
uint16_t next_available_tid = 1;

static uint64_t alloc_thread_stack()
{
    /* one page, for now */
    void *start = page_alloc();
    kprintf("alloc stack @ 0x%p\n", (uint64_t) start);
    return (uint64_t) start;
}

void add_thread(void (*f)(void))
{
    for (int i = 0; i < N_THREADS; i++) {
        if (thread_pool[i].state == UNUSED) {
            thread_pool[i].state = READY;
            thread_pool[i].stack_start = alloc_thread_stack();
            thread_pool[i].tf.regs[1] = thread_pool[i].stack_start + PAGE_SIZE;
            thread_pool[i].tid = next_available_tid++;
            thread_pool[i].priority = 1;
            thread_pool[i].pc = (uint64_t) f;
            return;
        }
    }

    panic("Can't add thread...");
}

void sched()
{
    struct thread_cxt *cxt_next = NULL;
    struct thread_cxt *cxt_curr = NULL;

    for (int i = 0; i < N_THREADS; i++) {
        if (thread_pool[i].state == RUNNING) {
            cxt_curr = &thread_pool[i];
            cxt_curr->priority--;
            break;
        }
    }

    int highest_priority = 0;
    for (int i = 0; i < N_THREADS; i++) {
        if (thread_pool[i].state == READY && highest_priority < thread_pool[i].priority) {
            cxt_next = &thread_pool[i];
            highest_priority = thread_pool[i].priority;
        }
    }

    if (!cxt_next) return;

    cxt_next->state = RUNNING;
    if (cxt_curr) {
        cxt_curr->state = READY;
    }

    for (int i = 0; i < N_THREADS; i++) {
        if (thread_pool[i].state == READY) {
            thread_pool[i].priority++;
        }
    }

    switch_from_to(cxt_curr, cxt_next);
}
