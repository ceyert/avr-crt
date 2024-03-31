#include "cpu_freq.h"
#include "math_util.h"
#include "uart.h"

/*
extern void preMainInit(void) __attribute__((section(".ctors")));

void preMainInit(void) 
{
    
}
*/

#define UBRR_VALUE (((F_CPU / (BAUD * 16UL))) - 1)
#define CYCLES_PER_MS (F_CPU / 1000UL)
#define DELAY_LOOP_CYCLES 4

static void delay_ms(unsigned short ms)
{
    unsigned long count = (CYCLES_PER_MS * ms) / DELAY_LOOP_CYCLES;

    while (count--)
    {
        __asm__ volatile("nop");
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
        delay_ms(1000);
    }

    return 0;
}