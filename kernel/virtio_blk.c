#include <kernel/console.h>
#include <kernel/kassert.h>
#include <kernel/virtio_blk.h>

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
    uint8_t is_read_only = ((device_features & VIRTIO_BLK_F_RO) != 0);
    uint8_t is_blk_size_available = ((device_features & VIRTIO_BLK_F_BLK_SIZE) != 0);
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

    kprintf("virtio_blk_init: device configuration parameters: \n");
    uint32_t config_gen_before = 0;
    uint32_t config_gen_after = 0;
    uint64_t capacity = 0;
    do {
        config_gen_before = VIRTIO_READ(VIRTIO_MMIO_CONFIG_GEN);
        capacity = VIRTIO_BLK_CFG_CAPACITY;
        config_gen_after = VIRTIO_READ(VIRTIO_MMIO_CONFIG_GEN);
    } while (config_gen_before != config_gen_after);
    kprintf("capacity: 0x%p\n", capacity);
    kassert(0);
}
