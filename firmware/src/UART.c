#include "jigbox.h"

#include <stdarg.h>
#include <stdio.h>

#define rxBufferSize 128
#define txBufferSize 128

static volatile uint8_t rxBuffer[rxBufferSize];
static volatile uint8_t rxBufferOut, rxBufferIn;

static volatile uint8_t txBuffer[txBufferSize];
static volatile uint8_t txBufferOut, txBufferIn;

void UART5_IRQHandler(void)
{
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */
        uint8_t c = USART_ReceiveData(UART5);

        if(rxBufferIn < rxBufferSize)
        {
            rxBuffer[rxBufferIn++] = c;
        }
    }

    if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
    {
        if (txBufferOut < txBufferIn)
        {
            /* Write one byte to the transmit data register */
            USART_SendData(UART5, txBuffer[txBufferOut++]);
        }

        if (txBufferOut >= txBufferIn)
        {
            txBufferOut = txBufferIn = 0;
            USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
        }
    }
}

// PC12 UART5_TX alternate function
// PD2 UART5_RX alternate function
void UART_Init(void)
{
    // enable UART5 and GPIOA/AFIO clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

    // configure GPIOC.12 as push-pull
    GPIO_InitTypeDef gpio_init;
    gpio_init.GPIO_Pin   = GPIO_Pin_12;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &gpio_init);

    // and GPIOD.2 as input floating
    gpio_init.GPIO_Pin  = GPIO_Pin_2;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &gpio_init);

    USART_InitTypeDef usart_init;
    usart_init.USART_BaudRate            = UART_DATA_RATE;
    usart_init.USART_WordLength          = USART_WordLength_8b;
    usart_init.USART_StopBits            = USART_StopBits_1;
    usart_init.USART_Parity              = USART_Parity_No;
    usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART5, &usart_init);

    // enable interrupts
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    USART_ITConfig(UART5, USART_IT_TXE, ENABLE);

    NVIC_SetPriority(UART5_IRQn, 0x02);
    NVIC_EnableIRQ(UART5_IRQn);

    USART_Cmd(UART5, ENABLE);          /* enable UART5 */
}

bool UART_transmitByte(uint8_t byte)
{
    bool retval = false;
    // if the transmit buffer is empty
    if (USART_GetFlagStatus(UART5, USART_FLAG_TXE) != RESET)
    {
        /* Write one byte to the transmit data register */
        USART_SendData(UART5, byte);
        retval = true;
    }
    else
    {
        __disable_irq();
        if (txBufferIn < txBufferSize)
        {
            txBuffer[txBufferIn++] = byte;
            retval                 = true;
            USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
        }
        __enable_irq();
    }
    return retval;
}

bool UART_receiveByte(uint8_t *buffer, uint16_t timeout)
{
    bool done        = false;
    uint32_t endTime = systemTime + timeout;

    while (systemTime < endTime)
    {
        __disable_irq();
        if (rxBufferOut < rxBufferIn)
        {
            *buffer = rxBuffer[rxBufferOut++];
            done    = true;
        }
        __enable_irq();
        if (done)
            return true;
    }
    return false;                      // timeout
}

void UART_flushReceivedCharacters(void)
{
    __disable_irq();
    rxBufferOut = rxBufferIn = 0;
    __enable_irq();
}

// receive up to bufsize characters, or until a CR or LF character.
// CR characters are turned into LF characters.
// The LF character will be included as the last character of the line.
uint8_t UART_receiveLine(uint8_t *buffer, uint8_t bufsize, uint16_t timeout)
{
    uint8_t nRead    = 0;
    uint32_t endTime = systemTime + timeout;

    while (systemTime < endTime)
    {
        uint8_t c;
        __disable_irq();
        while (nRead < bufsize && rxBufferOut < rxBufferIn)
        {
            c = rxBuffer[rxBufferOut++];
            if (c == '\r')
                c = '\n';
            *buffer++ = c;
            nRead++;
        }
        if ((c == '\n') || (nRead >= bufsize))
            break;
        else
            __enable_irq();
    }
    // reset rcv indexes
    rxBufferIn = rxBufferOut = 0;
    __enable_irq();
    return nRead;                      // timeout
}

void UART_printString(const char *s)
{
    for (const char *p = s; *p; p++)
    {
        UART_transmitByte(*p);
    }
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
