#include "common_defs.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * Initialize the UART
 */
void uart_init(void)
{
#if F_CPU < 2000000UL && defined(U2X)
  UCSRA = _BV(U2X);             /* improve baud rate error by using 2x clk */
  UBRRL = (F_CPU / (8UL * UART_BAUD)) - 1;
#else
  UBRRL = (F_CPU / (16UL * UART_BAUD)) - 1;
#endif
  UCSRB = _BV(TXEN) | _BV(RXEN); /* tx/rx enable */
}

/*
 * Enable/disable interrupts
 */
void uart_interrupt_rx(byte enable)
{
  if (enable)
    UCSRB |= BV(RXCIE);
   else
    UCSRB &= ~BV(RXCIE);
}

void uart_interrupt_tx(byte enable)
{
  if (enable)
    UCSRB |= BV(TXCIE);
   else
    UCSRB &= ~BV(TXCIE);
}
