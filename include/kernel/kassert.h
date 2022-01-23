#ifndef __KERNEL_KASSERT_H
#define __KERNEL_KASSERT_H

#define kassert(exp) if (!exp) panic("assert")

#endif
