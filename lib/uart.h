void	uart_init(void);
void	uart_interrupt_rx(unsigned char enable);
void	uart_interrupt_tx(unsigned char enable);

#define UART_BAUD			19200			// UART speed in bauds
