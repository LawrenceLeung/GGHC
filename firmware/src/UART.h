#ifndef __INCLUDED_UART_H
#define __INCLUDED_UART_H

#define UART_DATA_RATE 115200

void UART_Init(void);

// simple, polled send
void UART_sendByte(uint8_t byte);

// simple, polled receive with timeout in system ticks
bool UART_receiveByte(uint8_t *buffer, uint16_t timeout);

void UART_printString(const char *s);

int UART_printf(const char *format, ...);

#endif
