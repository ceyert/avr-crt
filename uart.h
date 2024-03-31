#ifndef UART_H
#define UART_H

#include "types.h"
#include "cpu_freq.h"

#define BAUD 9600
#define UBRR_VALUE (((F_CPU / (BAUD * 16UL))) - 1)

// UART Registers
#define UBRR0H *(volatile uint8_t*)(0xC5)
#define UBRR0L *(volatile uint8_t*)(0xC4)
#define UCSR0B *(volatile uint8_t*)(0xC1)
#define UCSR0C *(volatile uint8_t*)(0xC2)
#define UDR0 *(volatile uint8_t*)(0xC6)
#define UCSR0A *(volatile uint8_t*)(0xC0)

#define RXEN0 4
#define TXEN0 3
#define UCSZ00 1
#define UCSZ01 2
#define UDRE0 5

extern void USART_Init(void);

extern void USART_Transmit(uint8_t data);

extern void USART_SendString(const uint8_t* str);

extern void USART_SendCharArr(const uint8_t* buffer, uint8_t size);

extern void USART_SendInt(int value, uint8_t size);

#endif // UART_H
