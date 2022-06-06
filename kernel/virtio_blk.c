#include <kernel/console.h>
#include <kernel/kassert.h>
#include <kernel/plic.h>
#include <kernel/slab_alloc.h>
#include <kernel/trap.h>
#include <kernel/virtio_blk.h>
#include <lib/stddef.h>
#include <lib/string.h>

static struct virtio_blk blk_device;
static struct slab_t *virtio_blk_req_slab;

void virtio_blk_isr()
{
    uint32_t intr_status = VIRTIO_READ(VIRTIO_MMIO_INTERRUPT_STATUS);
    VIRTIO_WRITE(VIRTIO_MMIO_INTERRUPT_ACK, intr_status);

    while (blk_device.last_seen_used != blk_device.virtq.used->idx) {
        __sync_synchronize();
        struct virtq_used_elem elem =
            blk_device.virtq.used->ring[blk_device.last_seen_used % QUEUE_SIZE];
        uint16_t desc = elem.id;
        uint16_t chain_head = desc;
        struct virtio_blk_req *req =
            (struct virtio_blk_req *) blk_device.virtq.descriptors[desc].addr;
        if (req->status != 0) {
            panic("virtio_blk_isr: Request failed. status: 0x%p", req->status);
        }
        kassert((blk_device.virtq.descriptors[desc].flags & VIRTQ_DESC_F_NEXT) != 0);

        desc = blk_device.virtq.descriptors[desc].next;
        kassert((blk_device.virtq.descriptors[desc].flags & VIRTQ_DESC_F_NEXT) != 0);

        desc = blk_device.virtq.descriptors[desc].next;

        blk_device.last_seen_used++;
        desc_chain_free(chain_head);
        slab_free(virtio_blk_req_slab, (void *) req);
    }

    blk_device.is_currently_serving = false;
}

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

    /* 6. Write physical addresses of the queue’s Descriptor Area, Driver Area and Device
     * Area to (respectively) the QueueDescLow/QueueDescHigh,
     * QueueDriverLow/QueueDriverHigh and QueueDeviceLow/QueueDeviceHigh register pairs */

    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DESC_LOW, LOW32(virtq->descriptors));
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DESC_HIGH, HIGH32(virtq->descriptors));
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DRIVER_LOW, LOW32(virtq->avail));
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DRIVER_HIGH, HIGH32(virtq->avail));
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DEVICE_LOW, LOW32(virtq->used));
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_DEVICE_HIGH, HIGH32(virtq->used));

    /* 7. Write 0x1 to QueueReady. */
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_READY, 1);
}

uint16_t desc_free(uint16_t desc)
{
    kassert(desc <= QUEUE_SIZE);
    uint16_t next = QUEUE_SIZE;
    if ((blk_device.virtq.descriptors[desc].flags & VIRTQ_DESC_F_NEXT) != 0)
        next = blk_device.virtq.descriptors[desc].next;
    blk_device.free_desc[desc] = true;
    memset(&blk_device.virtq.descriptors[desc], 0, sizeof(struct virtq_desc));
    return next;
}

void desc_chain_free(uint16_t desc_head)
{
    uint16_t next = QUEUE_SIZE;
    while ((next = desc_free(desc_head)) != QUEUE_SIZE) desc_head = next;
    desc_free(desc_head);
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
    VIRTIO_WRITE(VIRTIO_MMIO_DRIVER_FEATURES, (device_features & ~(1 << 29)));

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
        panic("virtio_blk_init: Device did not accept driver-ACK'd features");
    }

    status |= VIRTIO_STATUS_DRIVER_OK;
    VIRTIO_WRITE(VIRTIO_MMIO_STATUS, status);

    /*                         Virtqueue initialization                      */

    /* 1. Select the queue writing its index (first queue is 0) to QueueSel. */
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_SEL, 0);

    /*2. Check if the queue is not already in use: read QueueReady, and expect a returned
     * value of zero (0x0).*/
    uint32_t queue_ready = VIRTIO_READ(VIRTIO_MMIO_QUEUE_READY);
    if (queue_ready != 0) {
        panic("virtio_blk_init: Couldn't select queue #0");
    }

    /* 3. Read maximum queue size (number of elements) from QueueNumMax. If the returned
     * value is zero */
    uint32_t queue_num_max = VIRTIO_READ(VIRTIO_MMIO_QUEUE_NUM_MAX);
    kprintf("QueueNumMax: 0x%p\n", queue_num_max);

    if (queue_num_max == 0) {
        panic("virtio_blk_init: Virtqueue #0 is not available");
    }

    if (queue_num_max < QUEUE_SIZE) {
        panic("virtio_blk_init: More descriptors available than supported by the device");
    }

    /* 4. Allocate and zero the queue memory, making sure the memory is physically
     * contiguous. (virtq_init) */
    dev_init();

    /* 5. Notify the device about the queue size by writing the size to QueueNum. */
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_NUM, QUEUE_SIZE);
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

    register_isr(PLIC_VIRTIO_IRQ, virtio_blk_isr);
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
    memset(h, 0, sizeof(struct virtio_blk_req));
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

    uint16_t next_idx = blk_device.virtq.avail->idx % QUEUE_SIZE;
    blk_device.virtq.avail->ring[next_idx] = header_desc;
    __sync_synchronize();

    blk_device.virtq.avail->idx++;
    __sync_synchronize();

    blk_device.is_currently_serving = true;
    VIRTIO_WRITE(VIRTIO_MMIO_QUEUE_NOTIFY, 0);

    while (blk_device.is_currently_serving)
        ;
}
