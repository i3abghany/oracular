#include "uart.h"

#include "common_defs.h"

void uart0_init() {
    /* Mask all UART interrupts. */
    write_byte_to_reg(UART0_BASE, UART_IER, 0x00);

    /* Enable Baud rate configuration */
    write_byte_to_reg(UART0_BASE, UART_LCR, (1 << 7));

    /* Set Baud rate LSB: 115.2K, the fastest possible rate. */
    write_byte_to_reg(UART0_BASE, UART_BAUD_LSB, 0x01);

    /* Set Baud rate MSB */
    write_byte_to_reg(UART0_BASE, UART_BAUD_MSB, 0x00);

    /* Leave Baud configuration mode. */
    write_byte_to_reg(UART0_BASE, UART_LCR, 0x00);

    /* Set the word length to 8 bits with no parity */
    write_byte_to_reg(UART0_BASE, UART_LCR, 0x03);
}

void uart0_putc(char c) {
    while ((read_byte_from_reg(UART0_BASE, UART_LSR) & (1 << 5)) == 0)
        ;
    write_byte_to_reg(UART0_BASE, UART_THR, c);
}
