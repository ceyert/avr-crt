#include "math_util.h"
#include "uart.h"

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

void USART_SendString(const uint8_t* str)
{
    while (*str != '\0')
    {
        USART_Transmit((uint8_t)*str);
        str++;
    }
}

void USART_SendCharArr(const uint8_t* buffer, uint8_t size)
{
    uint8_t idx = 0;
    while (idx < size)
    {
        USART_Transmit(buffer[idx]);
        idx++;
    }
}


void USART_SendInt(int value, uint8_t size)
{
    uint8_t buffer[size + 1];       
    uint8_t* str = buffer + (sizeof(uint8_t) + (size + 1));  
    *str-- = '\0';
    
    uint8_t digit_idx = size + 1;

    while (digit_idx > 0)
    {
        uint8_t digit = value % 10;
        *str-- = '0' + digit;
        value /= 10;
        USART_SendString(str);
        --digit_idx;
    }
}

uint8_t buffer[10] = {'H', 'E', 'L', 'L', 'O', ' ', 'A', 'V', 'R', '!'};

int global_value = 12345;

int main(void)
{
    USART_Init();

    USART_SendInt(global_value, 5);

    while (1)
    {
        USART_SendCharArr(buffer, 10);
        delay_my_ms(1000);
    }

    return 0;
}