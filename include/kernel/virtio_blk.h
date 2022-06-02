#ifndef _KERNEL_VIRTIO_BLK_H
#define _KERNEL_VIRTIO_BLK_H

#include "virtio.h"

#define VIRTIO_BLK_DEVICE_ID 2

#define VIRTIO_BLK_CFG(offs, type) ((type) VIRTIO_READ((offs)))

#define VIRTIO_BLK_CFG_CAPACITY   VIRTIO_BLK_CFG(0, uint64_t)
#define VIRTIO_BLK_CFG_SIZE_MAX   VIRTIO_BLK_CFG(8, uint32_t)
#define VIRTIO_BLK_CFG_SEG_MAX    VIRTIO_BLK_CFG(12, uint32_t)
#define VIRTIO_BLK_CFG_GEOMETRY   VIRTIO_BLK_CFG(16, uint32_t)
#define VIRTIO_BLK_CFG_BLK_SIZE   VIRTIO_BLK_CFG(20, uint32_t)
#define VIRTIO_BLK_CFG_TOPOLOGY   VIRTIO_BLK_CFG(24, uint64_t)
#define VIRTIO_BLK_CFG_WRITE_BACK VIRTIO_BLK_CFG(30, uint8_t)

#define VIRTIO_BLK_F_SIZE_MAX     (1 << 1)
#define VIRTIO_BLK_F_SEG_MAX      (1 << 2)
#define VIRTIO_BLK_F_GEOMETRY     (1 << 4)
#define VIRTIO_BLK_F_RO           (1 << 5)
#define VIRTIO_BLK_F_BLK_SIZE     (1 << 6)
#define VIRTIO_BLK_F_FLUSH        (1 << 9)
#define VIRTIO_BLK_F_TOPOLOGY     (1 << 10)
#define VIRTIO_BLK_F_CONFIG_WCE   (1 << 11)
#define VIRTIO_BLK_F_DISCARD      (1 << 13)
#define VIRTIO_BLK_F_WRITE_ZEROES (1 << 14)

struct virtio_blk_req {
    uint32_t type;
    uint32_t reserved_;
    uint64_t sector;
    /* As per the reference, we don't need the *data* field as it's used for
     * discard and write zeros commands, which we don't use (for now) */
    uint8_t data[512];
    uint8_t status;
} __attribute__((packed));

#endif
