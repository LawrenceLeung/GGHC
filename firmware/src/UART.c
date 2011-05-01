#include "jigbox.h"

#include <stdarg.h>
#include <stdio.h>

// PC12 UART5_TX alternate function
// PD2 UART5_RX alternate function
void UART_Init(void)
{
    // enable UART5 and GPIOA/AFIO clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

    // configure GPIOC.12 as push-pull
    GPIO_InitTypeDef  gpio_init;
    gpio_init.GPIO_Pin   = GPIO_Pin_12;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &gpio_init);

    // and GPIOD.2 as input floating
    gpio_init.GPIO_Pin   = GPIO_Pin_2;
    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &gpio_init);

    USART_InitTypeDef usart_init;
    usart_init.USART_BaudRate            = UART_DATA_RATE;
    usart_init.USART_WordLength          = USART_WordLength_8b;
    usart_init.USART_StopBits            = USART_StopBits_1;
    usart_init.USART_Parity              = USART_Parity_No ;
    usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init.USART_Mode                = USART_Mode_Tx;
    USART_Init(UART5, &usart_init);

    USART_ClockInitTypeDef usart_clk_init;
    usart_clk_init.USART_Clock               = USART_Clock_Disable;
    usart_clk_init.USART_CPOL                = USART_CPOL_Low;
    usart_clk_init.USART_CPHA                = USART_CPHA_2Edge;
    usart_clk_init.USART_LastBit             = USART_LastBit_Disable;
    USART_ClockInit(UART5, &usart_clk_init);

    USART_Cmd(UART5, ENABLE);                             /* enable UART5 */
}

void UART_sendByte(uint8_t byte)
{
    /* Send one byte from UART */
    USART_SendData(UART5, byte);

    /* Loop until UART DR register is empty */ 
    while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET)
        /* spin */;
}

bool UART_receiveByte(uint8_t *buffer, uint16_t timeout)
{
    uint32_t endTime = systemTime + timeout;

    while(USART_GetFlagStatus(UART5, USART_FLAG_RXNE) == RESET)
    {
        if (systemTime > endTime)
            return false;
    }

    *buffer = USART_ReceiveData(UART5);
    return true;
}

void UART_printString(const char *s)
{
    for (const char *p = s; *p; p++)
        UART_sendByte(*p);
}

int UART_printf(const char *format, ...)
{
    static char buffer[90];
    va_list ap;
    va_start(ap, format);
    int n = vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);
    UART_printString(buffer);
    return n;
}
