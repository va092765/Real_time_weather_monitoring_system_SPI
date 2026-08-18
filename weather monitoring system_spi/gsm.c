#include <LPC21xx.h>
#include "types.h"
#include "uart.h"
#include "gsm.h"
extern void delay_milliseconds(u32 ms);   /* defined in lcd.c */
void GSM_Init(void)
{
	Init_UART0();
	delay_milliseconds(2000);   /* let SIM800C boot/register on network */
}
void GSM_SendSMS(u8 *number, u8 *message)
{
	UART0_SendString("AT\r\n");
	delay_milliseconds(500);

	UART0_SendString("AT+CMGF=1\r\n");        /* text mode */
	delay_milliseconds(500);

	UART0_SendString("AT+CMGS=\"+919489559453");
	//UART0_SendString(number);
	UART0_SendString("\"\r\n");
	delay_milliseconds(500);

	UART0_SendString(message);
	delay_milliseconds(500);

	UART0_SendChar(0x1A);      /* Ctrl+Z sends the message */
	delay_milliseconds(3000);
}
