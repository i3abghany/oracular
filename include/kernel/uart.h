#ifndef __KERNEL_UART_H
#define __KERNEL_UART_H

#include <stdint.h>

#define UART0_BASE 0x10000000

#define UART_RHR (uint8_t *) (UART0_BASE + 0) /* Receive Hold Register     */
#define UART_THR (uint8_t *) (UART0_BASE + 0) /* Transmit Hold Register    */
#define UART_IER (uint8_t *) (UART0_BASE + 1) /* Interrupt Enable Register */
#define UART_FCR (uint8_t *) (UART0_BASE + 2) /* FIFO Control Register     */
#define UART_ISR (uint8_t *) (UART0_BASE + 2) /* Interrupt Status Register */
#define UART_LCR (uint8_t *) (UART0_BASE + 3) /* Line Control Register     */
#define UART_LSR (uint8_t *) (UART0_BASE + 5) /* Line Status Register      */

#define UART_BAUD_LSB (uint8_t *) (UART0_BASE + 0)
#define UART_BAUD_MSB (uint8_t *) (UART0_BASE + 1)

void uart0_init();
void uart0_put(uint8_t c);
uint8_t uart0_get();
void uart0_isr();

#endif
