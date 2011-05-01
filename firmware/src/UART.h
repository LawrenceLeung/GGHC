#ifndef __INCLUDED_UART_H
#define __INCLUDED_UART_H

#define UART_DATA_RATE 115200

extern void UART5_IRQHandler(void);

extern void UART_Init(void);

void UART5_IRQHandler(void);

// interrupt-driven transmit byte; returns false if no room
bool UART_transmitByte(uint8_t byte);

// interrupt-driven receive byte; returns false if bytes by timeout
bool UART_receiveByte(uint8_t *buffer, uint16_t timeout);

// throw away received characters
void UART_flushReceivedCharacters(void);

// receive up to bufsize characters, or until a CR or LF character.
// CR characters are turned into LF characters.
// The LF character will be included as the last character of the line.
uint8_t UART_receiveLine(uint8_t *buffer, uint8_t bufsize, uint16_t timeout);

// print a string to the UART
void UART_printString(const char *s);

// printf via UART
int UART_printf(const char *format, ...);

#endif
