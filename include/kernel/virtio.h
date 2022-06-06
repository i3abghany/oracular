#ifndef _KERNEL_VIRTIO_H
#define _KERNEL_VIRTIO_H

#include <stdint.h>

#define VIRTIO_BASE (uint64_t) 0x10001000

#define VIRTIO_READ(offset)       (*(volatile uint32_t *) (VIRTIO_BASE + offset))
#define VIRTIO_WRITE(offset, val) (*(volatile uint32_t *) (VIRTIO_BASE + offset)) = val

/* The driver MUST only use 32 bit wide and aligned reads and writes to access
 * the control registers */

/* The device MUST return 0x74726976 in MagicValue */
#define VIRTIO_MMIO_MAGIC_VALUE 0x0
#define VIRTIO_MAGIC            0x74726976
/* The device MUST return value 0x2 in Version */
#define VIRTIO_MMIO_VERSION 0x4
#define VIRTIO_VERSION      0x2
/* The driver MUST ignore a device with DeviceID 0x0 */
#define VIRTIO_MMIO_DEVICE_ID 0x8
#define VIRTIO_MMIO_VENDOR_ID 0xC
/*
 * Before reading from DeviceFeatures, the driver MUST write a value to
 * DeviceFeaturesSel.
 */
#define VIRTIO_MMIO_DEVICE_FEATURES     0x10
#define VIRTIO_MMIO_DEVICE_FEATURES_SEL 0x14
/*
 * Before writing to the DriverFeatures register, the driver MUST write a value
 * to the DriverFeaturesSel register.
 */
#define VIRTIO_MMIO_DRIVER_FEATURES     0x20
#define VIRTIO_MMIO_DRIVER_FEATURES_SEL 0x24
#define VIRTIO_MMIO_QUEUE_SEL           0x30
/*
 * The driver MUST write a value to QueueNum which is less than or equal to the
 * value presented by the device in QueueNumMax.
 */
#define VIRTIO_MMIO_QUEUE_NUM_MAX 0x34
#define VIRTIO_MMIO_QUEUE_NUM     0x38
/*
 * When QueueReady is not zero, the driver MUST NOT access QueueNum,
 * QueueDescLow, QueueDescHigh, QueueAvailLow, QueueAvailHigh, QueueUsedLow,
 * QueueUsedHigh.
 *
 * To stop using the queue the driver MUST write zero (0x0) to this QueueReady
 * and MUST read the value back to ensure synchronization.
 */
#define VIRTIO_MMIO_QUEUE_READY  0x44
#define VIRTIO_MMIO_QUEUE_NOTIFY 0x50
/*
 * The device MUST present each event by setting the corresponding bit in
 * InterruptStatus from the moment it takes place
 *
 * The driver MUST ignore undefined bits in InterruptStatus
 */
#define VIRTIO_MMIO_INTERRUPT_STATUS 0x60
/*
 * The driver acknowledges the interrupt by writing a corresponding bit mask to
 * the InterruptACK register.
 *
 * The driver MUST write a value with a bit mask describing events it handled
 * into InterruptACK when it finishes handling an interrupt and MUST NOT set any
 * of the undefined bits in the value.
 */
#define VIRTIO_MMIO_INTERRUPT_ACK 0x64
#define VIRTIO_MMIO_STATUS        0x70

#define VIRTIO_MMIO_QUEUE_DESC_LOW    0x80
#define VIRTIO_MMIO_QUEUE_DESC_HIGH   0x84
#define VIRTIO_MMIO_QUEUE_DRIVER_LOW  0x90
#define VIRTIO_MMIO_QUEUE_DRIVER_HIGH 0x94
#define VIRTIO_MMIO_QUEUE_DEVICE_LOW  0xa0
#define VIRTIO_MMIO_QUEUE_DEVICE_HIGH 0xa4

#define VIRTIO_MMIO_CONFIG_GEN 0xfc
#define VIRTIO_MMIO_CONFIG     0x100

#define VIRTIO_STATUS_ACKNOWLEDGE 1
#define VIRTIO_STATUS_DRIVER      2
#define VIRTIO_STATUS_FEATURES_OK 8
#define VIRTIO_STATUS_DRIVER_OK   4

#endif
