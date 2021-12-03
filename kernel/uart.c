#include "uart.h"

#include "common_defs.h"

void uart0_init() {
    /* Mask all UART interrupts. */
    mmio_write_byte(UART0_BASE, UART_IER, 0x00);

    /* Enable Baud rate configuration */
    mmio_write_byte(UART0_BASE, UART_LCR, (1 << 7));

    /* Set Baud rate LSB: 115.2K, the fastest possible rate. */
    mmio_write_byte(UART0_BASE, UART_BAUD_LSB, 0x01);

    /* Set Baud rate MSB */
    mmio_write_byte(UART0_BASE, UART_BAUD_MSB, 0x00);

    /* Leave Baud configuration mode. */
    mmio_write_byte(UART0_BASE, UART_LCR, 0x00);

    /* Set the word length to 8 bits with no parity */
    mmio_write_byte(UART0_BASE, UART_LCR, 0x03);
}

void uart0_putc(char c) {
    while ((mmio_read_byte(UART0_BASE, UART_LSR) & (1 << 5)) == 0)
        ;
    mmio_write_byte(UART0_BASE, UART_THR, c);
}
