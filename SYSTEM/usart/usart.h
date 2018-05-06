#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 


void uart_init(u32 bound);
void uart_init_speaker(u32 bound);
void UART2_SendByte(u16 Data);
#endif


