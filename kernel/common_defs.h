#ifndef __KERNEL_COMMON_DEFS_H
#define __KERNEL_COMMON_DEFS_H

#include <stdint.h>

#define write_byte_to_reg(base, offset, value) \
    *((uint8_t *) (base + offset)) = (uint8_t) value

#define read_byte_from_reg(base, offset) *((uint8_t *) (base + offset))

#endif
