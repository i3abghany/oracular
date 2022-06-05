#include <kernel/console.h>
#include <kernel/kassert.h>
#include <kernel/slab_alloc.h>
#include <kernel/virtio_blk.h>
#include <lib/string.h>
#include <stddef.h>
#include <string.h>

static struct virtio_blk blk_device;
static struct slab_t *virtio_blk_req_slab;

static void virtq_init()
{
    uint64_t desc_size = sizeof(struct virtq_desc) * QUEUE_SIZE;
    uint64_t avail_size = sizeof(struct virtq_avail);

    struct virtq_t *virtq = &blk_device.virtq;
    virtq->descriptors = (struct virtq_desc *) (&blk_device);  // Alignment 16
    virtq->avail =
        (struct virtq_avail *) ((uint64_t) &blk_device + desc_size);  // Alignment 2

    uint64_t used_addr = (uint64_t) &blk_device + desc_size + avail_size;
    uint64_t used_aligned_addr =
        used_addr % 4 != 0 ? used_addr + (4 - (used_addr % 4)) : used_addr;
    kassert(used_aligned_addr < ((uint64_t) &blk_device + sizeof(virtq->mem)));
    virtq->used = (struct virtq_used *) (used_aligned_addr);

#define LOW32(x)  (uint32_t)((uint64_t) x & 0xFFFFFFFF)
#define HIGH32(x) (uint32_t)(((uint64_t) x >> 32) & 0xFFFFFFFF)

    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DESC_LOW, LOW32(virtq->descriptors));
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DESC_HIGH, HIGH32(virtq->descriptors));
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DRIVER_LOW, LOW32(virtq->avail));
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DRIVER_HIGH, HIGH32(virtq->avail));
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DEVICE_LOW, LOW32(virtq->used));
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DEVICE_HIGH, HIGH32(virtq->used));
}

static void dev_init()
{
    memset(&blk_device, 0, sizeof(struct virtio_blk));
    virtq_init();

    for (int i = 0; i < QUEUE_SIZE; i++) {
        blk_device.free_desc[i] = true;
    }

    blk_device.is_init = true;
}

static void virtio_blk_slab_init()
{
    if (virtio_blk_req_slab == NULL) {
        virtio_blk_req_slab = slab_init("virtio_blk_req", sizeof(struct virtio_blk_req));
    }
}

void virtio_blk_init()
{
    uint32_t magic = VIRTIO_READ(VIRTIO_MMIO_MAGIC_VALUE);
    if (magic != VIRTIO_MAGIC) {
        panic("virtio_blk_init: wrong magic value: 0x%p", magic);
    }

    uint32_t version = VIRTIO_READ(VIRTIO_MMIO_VERSION);
    if (version != VIRTIO_VERSION) {
        panic("virtio_blk_init: wrong version: %d", version);
    }

    uint32_t device_id = VIRTIO_READ(VIRTIO_MMIO_DEVICE_ID);
    if (device_id != VIRTIO_BLK_DEVICE_ID) {
        panic("virtio_blk_init: wrong device id: %d", device_id);
    }

    /* Reset the device */
    uint32_t status = 0;
    VIRTIO_WRITE(VIRTIO_MMIO_STATUS, status);

    /* Set the ACKNOWLEDGE status bit: the guest OS has noticed the device. */
    status |= VIRTIO_STATUS_ACKNOWLEDGE;
    VIRTIO_WRITE(VIRTIO_MMIO_STATUS, status);

    /* Set the DRIVER status bit: the guest OS knows how to drive the device. */
    status |= VIRTIO_STATUS_DRIVER;
    VIRTIO_WRITE(VIRTIO_MMIO_STATUS, status);

    /*
     * Before reading from DeviceFeatures, the driver MUST write a value to
     * DeviceFeaturesSel. 0 means query feature bits from 0 to 31.
     *
     * Read device feature bits
     */
    VIRTIO_WRITE(VIRTIO_MMIO_DEVICE_FEATURES_SEL, 0);
    uint32_t device_features = VIRTIO_READ(VIRTIO_MMIO_DEVICE_FEATURES);

    kprintf("virtio_blk_init: Offered features: 0x%p\n", device_features);

    bool is_read_only = ((device_features & VIRTIO_BLK_F_RO) != 0);
    bool is_blk_size_available = ((device_features & VIRTIO_BLK_F_BLK_SIZE) != 0);
    (void) is_read_only;
    (void) is_blk_size_available;

    /*
     * Before writing to the DriverFeatures register, the driver MUST write a value
     * to the DriverFeaturesSel register.
     *
     * write the subset of feature bits understood by the OS and driver to the
     * device
     */
    VIRTIO_WRITE(VIRTIO_MMIO_DRIVER_FEATURES_SEL, 0);
    VIRTIO_WRITE(VIRTIO_MMIO_DRIVER_FEATURES, device_features);

    /* Set the FEATURES_OK status bit */
    status |= VIRTIO_STATUS_FEATURES_OK;
    VIRTIO_WRITE(VIRTIO_MMIO_STATUS, status);

    /*
     * Re-read device status to ensure the FEATURES_OK bit is still set:
     * otherwise, the device does not support our subset of features and the
     * device is unusable
     */
    uint32_t final_status = VIRTIO_READ(VIRTIO_MMIO_STATUS);
    if ((final_status & VIRTIO_STATUS_FEATURES_OK) == 0) {
        panic("virtio_blk_init: Device did not accept driver-ACK'd features.");
    }

    status |= VIRTIO_STATUS_DRIVER_OK;
    VIRTIO_WRITE(VIRTIO_MMIO_STATUS, status);

    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_SEL, 0);
    uint32_t queue_num_max = VIRTIO_READ(VIRTIO_MMIO_QUEUE_NUM_MAX);
    kprintf("QueueNumMax: 0x%p\n", queue_num_max);

    if (queue_num_max == 0) {
        panic("virtio_blk_init: Couldn't select queue #0");
    }

    if (queue_num_max < QUEUE_SIZE) {
        panic("virtio_blk_init: More descriptors available than supported by the device");
    }

    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_NUM, QUEUE_SIZE);
    dev_init();
    virtio_blk_slab_init();

    kprintf("virtio_blk_init: Device configuration parameters: \n");
    uint32_t config_gen_before = 0;
    uint32_t config_gen_after = 0;
    uint64_t capacity = 0;
    do {
        config_gen_before = VIRTIO_READ(VIRTIO_MMIO_CONFIG_GEN);
        capacity = VIRTIO_BLK_CFG_CAPACITY;
        config_gen_after = VIRTIO_READ(VIRTIO_MMIO_CONFIG_GEN);
    } while (config_gen_before != config_gen_after);
    kprintf("Capacity: 0x%p\n", capacity);
}

static uint16_t get_free_desc()
{
    for (int i = 0; i < QUEUE_SIZE; i++) {
        if (blk_device.free_desc[i]) {
            blk_device.free_desc[i] = false;
            return i;
        }
    }
    return QUEUE_SIZE;
}

/*
 * Each request consists of a 3-descriptor chain. First one describes a 16-byte
 * buffer containing the first three, read-only fields of the virtio_blk_req
 * struct. The second describes a device-writable buffer, data, which points to
 * an externally allocated buffer. The third buffer points to the status field
 * of the request struct. This allows omitting a copy and directly write the
 * data in the incoming buffer.
 *
 * Currently, we don't need to translate addresses to physical as kernel
 * pages are mapped from virtual to identical physical addresses.
 */
void virtio_blk_request(uint32_t req_type, uint32_t sector, uint8_t *data)
{
    if (!blk_device.is_init) {
        panic("virtio_blk_request: virtio_blk device not initialized.");
    }

    struct virtio_blk_req *h = slab_alloc(virtio_blk_req_slab);
    h->type = req_type;
    h->sector = sector;

    uint16_t header_desc = get_free_desc();
    kassert(header_desc != QUEUE_SIZE);

    blk_device.virtq.descriptors[header_desc].addr = (uint64_t) h;
    blk_device.virtq.descriptors[header_desc].len = VIRTIO_BLK_REQ_HEADER_SIZE;
    blk_device.virtq.descriptors[header_desc].flags = VIRTQ_DESC_F_NEXT;

    uint16_t data_desc = get_free_desc();
    kassert(data_desc != QUEUE_SIZE);

    blk_device.virtq.descriptors[data_desc].addr = (uint64_t) data;
    blk_device.virtq.descriptors[data_desc].len = VIRTIO_BLK_REQ_SECTOR_SIZE;
    blk_device.virtq.descriptors[data_desc].flags = VIRTQ_DESC_F_NEXT;
    if (req_type == VIRTIO_BLK_T_IN) {
        blk_device.virtq.descriptors[data_desc].flags |= VIRTQ_DESC_F_WRITE;
    }

    uint16_t status_desc = get_free_desc();
    kassert(status_desc != QUEUE_SIZE);

    blk_device.virtq.descriptors[status_desc].addr = (uint64_t) & (h->status);
    blk_device.virtq.descriptors[status_desc].len = VIRTIO_BLK_REQ_STATUS_SIZE;
    blk_device.virtq.descriptors[status_desc].flags |= VIRTQ_DESC_F_WRITE;

    blk_device.virtq.descriptors[header_desc].next = data_desc;
    blk_device.virtq.descriptors[data_desc].next = status_desc;

    uint16_t next_idx = blk_device.virtq.avail->idx;
    blk_device.virtq.avail->ring[next_idx] = header_desc;

    int i = 0;
    while (i < 10000) i++;

    blk_device.virtq.avail->idx++;

    i = 0;
    while (i < 10000) i++;

    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_READY, 1);
    kprintf("ready: %d\n", VIRTIO_READ(VIRTIO_MMIO_QUEUE_READY));
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_NOTIFY, 0);
}
