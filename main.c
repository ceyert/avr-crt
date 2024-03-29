#include "math_util.h"

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE (((F_CPU / (BAUD * 16UL))) - 1)
#define CYCLES_PER_MS (F_CPU / 1000UL)
#define DELAY_LOOP_CYCLES 4

static void delay_my_ms(unsigned short ms)
{
    unsigned long count = (CYCLES_PER_MS * ms) / DELAY_LOOP_CYCLES;

    while (count--)
    {
        __asm__ volatile("nop");
    }
}

typedef unsigned char uint8_t;

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

void USART_Init(void)
{
    // Set baud rate
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)UBRR_VALUE;

    // Set frame format: 8 data bits, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Enable transmitter and receiver
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void USART_Transmit(uint8_t data)
{
    // Wait for empty transmit buffer
    while ((UCSR0A & (1 << UDRE0)) == 0)
    {
    };  // Do nothing until UDR is ready

    // Put data into buffer, send the data
    UDR0 = data;
}

void USART_SendString(const char* str)
{
    while (*str != '\0')
    {
        USART_Transmit((uint8_t)*str);
        str++;
    }
}

#define MAX_SIZE 50

void USART_SendInt(int value)
{
    char buffer[MAX_SIZE + 1];          // +1 for null terminator
    char* str = &buffer[0] + MAX_SIZE;  // Start at the end of the buffer
    *str = '\0';

    int isNegative = value < 0;

    if (value == 0)
    {
        *--str = '0';
    }
    else
    {
        // Process each digit; this works even if value is negative
        while (value != 0)
        {
            int digit = value % 10;
            if (digit < 0)
            {
                digit = -digit;  // Make digit positive
            } 
            *--str = '0' + digit;
            value /= 10;
        }
        if (isNegative)
        {
            *--str = '-'; 
        }
    }

    USART_SendString(str);
}

int global_counter = 50;

int main(void)
{
    USART_Init();

    while (1)
    {
        USART_SendString("Counter value: ");
        USART_SendInt(global_counter);
        USART_SendString("\r\n");

        global_counter++;
        delay_my_ms(1000);
    }
}