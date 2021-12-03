#ifndef __KERNEL_UART_H
#define __KERNEL_UART_H

#include <stdint.h>

#define UART0_BASE (uint8_t *) 0x10000000

#define UART_RHR 0 /* Receive Hold Register     */
#define UART_THR 0 /* Transmit Hold Register    */
#define UART_IER 1 /* Interrupt Enable Register */
#define UART_FCR 2 /* FIFO Control Register     */
#define UART_ISR 2 /* Interrupt Status Register */
#define UART_LCR 3 /* Line Control Register     */
#define UART_LSR 5 /* Line Status Register      */

#define UART_BAUD_LSB 0
#define UART_BAUD_MSB 1

void uart0_init();
void uart0_put(uint8_t c);
uint8_t uart0_get();

#endif
