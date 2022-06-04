#include <lib/string.h>
#include <stdint.h>

void *memset(void *dest, int c, size_t size)
{
    uint8_t *d = (uint8_t *) dest;

    for (size_t i = 0; i < size; i++) {
        d[i] = c;
    }

    return dest;
}
