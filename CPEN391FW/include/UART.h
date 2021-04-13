/*
 * UART.h
 *
 *  Created on: Mar 12, 2021
 *      Author: Jason Bai
 */

/*
 * Description
 * UART module.
 */

#ifndef UART_H_
#define UART_H_
/*------------------- Type Define -------------------*/
typedef enum
{
    UART_ePORT_WIFI = 0,
    UART_ePORT_BLUETOOTH,
} UART_ePORT;

/*------------------- Function Prototype -------------------*/
void UART_Init(UART_ePORT ePort);
int UART_putchar(UART_ePORT ePort, int c);
int UART_getchar(UART_ePORT ePort);
int UART_TestForReceivedData(UART_ePORT ePort);
void UART_Flush(UART_ePORT ePort);
void UART_puts(UART_ePORT ePort, char *buffer);
char *UART_gets(UART_ePORT ePort, char *buffer, int length, int mode);
#endif // UART_H_
