#ifndef _KERNEL_VIRTIO_BLK_H
#define _KERNEL_VIRTIO_BLK_H

#include "../lib/stdbool.h"
#include "kmem.h"
#include "virtio.h"

#define VIRTIO_BLK_DEVICE_ID 2

#define VIRTIO_BLK_CFG(offs, type) ((type) VIRTIO_READ((VIRTIO_MMIO_CONFIG + offs)))

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

#define QUEUE_SIZE 16

struct virtq_desc {
    uint64_t addr; /* Physical guest memory address */
    uint32_t len;
/* This marks a buffer as continuing via the next field. */
#define VIRTQ_DESC_F_NEXT 1
/* This marks a buffer as device write-only (otherwise device read-only). */
#define VIRTQ_DESC_F_WRITE 2
/* This means the buffer contains a list of buffer descriptors. */
#define VIRTQ_DESC_F_INDIRECT 4
    /* The flags as indicated above. */
    uint16_t flags;
    /* Next field if flags & NEXT */
    uint16_t next;
} __attribute__((packed));

struct virtq_avail {
#define VIRTQ_AVAIL_F_NO_INTERRUPT 1
    uint16_t flags;
    uint16_t idx;
    uint16_t ring[QUEUE_SIZE];
} __attribute__((packed));

struct virtq_used_elem {
    /* Index of start of used descriptor chain. */
    uint32_t id;
    /* Total length of the descriptor chain which was used (written to) */
    uint32_t len;
};

struct virtq_used {
#define VIRTQ_USED_F_NO_NOTIFY 1
    uint16_t flags;
    uint16_t idx;
    struct virtq_used_elem ring[QUEUE_SIZE];
} __attribute__((packed));

struct virtq_t {
    uint8_t mem[2 * PAGE_SIZE];  // Other fields are situated here.
    struct virtq_desc *descriptors;
    struct virtq_avail *avail;
    struct virtq_used *used;
} __attribute__((packed)) __attribute__((aligned(PAGE_SIZE)));

struct virtio_blk {
    struct virtq_t virtq;
    uint8_t free_desc[QUEUE_SIZE];
};

void virtio_blk_init();

#endif
