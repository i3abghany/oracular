#include <kernel/console.h>
#include <kernel/mmio.h>
#include <kernel/plic.h>
#include <kernel/trap.h>
#include <kernel/uart.h>

static void uart0_isr()
{
    char c = uart0_get();
    if (c == 0x7f) {
        putchar(8);
        putchar(' ');
        putchar(8);
    } else if (c == 10 || c == 13) {
        putchar('\n');
    } else {
        putchar(c);
    }
}

void uart0_init()
{
    /* Mask all UART interrupts. */
    mmio_write_byte(UART_IER, 0x00);

    /* Enable FIFO */
    mmio_write_byte(UART_FCR, (3 << 1) | (1 << 0));

    /* Enable Baud rate configuration by setting the DLAB bit. */
    mmio_write_byte(UART_LCR, (1 << 7));

    /*
     * The formula for calculating the Baud rate: BR = f_clk / (16 * divisor)
     * Therefore, divisor = f_clk / (16 * BR). If we want to acheive a BAUD rate
     * of 9600 bps, then the divisor must be 147.97526.
     */
    uint16_t divisor = 148;

    /* Set Baud rate LSB */
    mmio_write_byte(UART_BAUD_LSB, (divisor & 0xFF));

    /* Set Baud rate MSB */
    mmio_write_byte(UART_BAUD_MSB, (divisor >> 8));

    /* Leave Baud configuration mode. */
    mmio_write_byte(UART_LCR, 0x00);

    /* Set the word length to 8 bits with no parity */
    mmio_write_byte(UART_LCR, 0x03);

    /* Enable receive interrupts */
    mmio_write_byte(UART_IER, (1 << 0));

    register_isr(PLIC_UART0_IRQ, uart0_isr);
}

void uart0_put(uint8_t c)
{
    while ((mmio_read_byte(UART_LSR) & (1 << 5)) == 0)
        ;
    mmio_write_byte(UART_THR, c);
}

uint8_t uart0_get()
{
    while ((mmio_read_byte(UART_LSR) & (1 << 0)) == 0)
        ;
    return mmio_read_byte(UART_RHR);
}
