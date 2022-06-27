#ifndef _KERNEL_KTHREAD_H
#define _KERNEL_KTHREAD_H

#include <stdint.h>

enum THREAD_STATE { UNUSED = 0, RUNNING, READY };

struct trap_frame {
    uint64_t regs[31];
} __attribute__((packed));

struct thread_cxt {
    struct trap_frame tf;  // [0   -> 247]
    uint64_t stack_start;  // [248 -> 255]
    uint64_t pc;           // [256 -> 263]
    uint16_t tid;
    uint16_t priority;
    enum THREAD_STATE state;
} __attribute__((packed));

#define N_THREADS 16

extern void switch_from_to(struct thread_cxt *curr, struct thread_cxt *next);
void add_thread(void (*f)(void));
void sched();

#endif
