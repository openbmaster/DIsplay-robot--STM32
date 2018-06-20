#ifndef __UART_H
#define __UART_H

#include "ucos_ii.h"

#define	BUFFER_SIZE		256

void uart1_init(u32 bound);
void Task_Uart1_Recv(void *arg);
void Task_Uart1_Send(void *arg);

#endif
