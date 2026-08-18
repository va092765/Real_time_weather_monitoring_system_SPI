						/* uart.c */
#include <LPC21xx.h>
#include "types.h"
#include "uart.h"

/* PCLK = 15MHz (matches T0PR=15000-1 used for 1ms tick in lcd.c) */

void Init_UART0(void)
{
	PINSEL0 |= 0x00000005;  /* P0.0 = TXD0, P0.1 = RXD0 */
	U0LCR = 0x83;             /* 8N1, DLAB=1 to access divisor latches */
	U0DLL = 98;                /* 15000000 / (16 * 9600) = 98 */
	U0DLM = 0;
	U0LCR = 0x03;             /* DLAB=0, 8N1 normal operation */
}

void UART0_SendChar(u8 c)
{
	U0THR = c;
	while (!(U0LSR & 0x20));  /* wait for THRE (TX holding reg empty) */
}

void UART0_SendString(u8 *str)
{
	while (*str)
		UART0_SendChar(*str++);
}
						


