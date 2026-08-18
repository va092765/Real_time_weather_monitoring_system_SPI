				
#ifndef __UART_H__
#define __UART_H__

#include "types.h"

void Init_UART0(void);
void UART0_SendChar(u8 c);
void UART0_SendString(u8 *str);

#endif

