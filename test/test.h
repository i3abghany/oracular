#ifndef __TEST_H
#define __TEST_H

#include <kernel/kassert.h>

#ifdef ORACULAR_KERNEL
#define ASSERT kassert
#else
#define ASSERT assert
#endif

#define ASSERT_EQUAL(a, b) ASSERT((a) == (b))
#define ASSERT_NOT_NULL(a) ASSERT((a) != 0)

#endif
