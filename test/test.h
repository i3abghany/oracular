#ifndef __TEST_H
#define __TEST_H

#include <kernel/kassert.h>

#ifdef ORACULAR_KERNEL
#define ASSERT kassert
#else
#define ASSERT assert
#endif

#define TEST_MODULE(name) static const char *__test_module_name = name

#define ASSERT_EQUAL(a, b) ASSERT((a) == (b))
#define ASSERT_NULL(a)     ASSERT((a) == 0)
#define ASSERT_NOT_NULL(a) ASSERT((a) != 0)
#define ASSERT_TRUE(a) ASSERT((a))

#define PASS() kprintf("%s: %s \033[0;32m[pass]\33[0m\n", __test_module_name, __func__);

#endif
