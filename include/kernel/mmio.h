#ifndef __KERNEL_MMIO_H
#define __KERNEL_MMIO_H

#include <stdint.h>

#define mmio_write_byte(addr, value) *((volatile uint8_t *) (addr)) = ((uint8_t) (value))

#define mmio_read_byte(addr) *((volatile uint8_t *) (addr))

#endif
