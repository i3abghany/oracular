#ifndef __KERNEL_COMMON_DEFS_H
#define __KERNEL_COMMON_DEFS_H

#include <stdint.h>

#define mmio_write_byte(base, offset, value) \
    *((volatile uint8_t *) ((base) + (offset))) = ((uint8_t) (value))

#define mmio_read_byte(base, offset) *((volatile uint8_t *) ((base) + (offset)))

#endif
