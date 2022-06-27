#include <kernel/console.h>
#include <kernel/slab_alloc.h>
#include <kernel/virtio_blk.h>
#include <libc/string.h>

#include "../test.h"

TEST_MODULE("test_virtio_blk");

void test_write_read()
{
    uint8_t write_char_block[512];
    uint8_t read_char_block[512];

    const char *sig = "The Oracular Operating System";
    for (size_t i = 0; i < strlen(sig); i++) {
        write_char_block[i] = sig[i];
    }

    virtio_blk_request(VIRTIO_BLK_T_OUT, 0, write_char_block);
    virtio_blk_request(VIRTIO_BLK_T_IN, 0, read_char_block);

    bool strings_equal = true;
    for (size_t i = 0; i < strlen(sig); i++) {
        strings_equal &= read_char_block[i] == write_char_block[i];
    }
    ASSERT_TRUE(strings_equal);
    PASS();
}

void virtio_blk_tests()
{
    test_write_read();
}
