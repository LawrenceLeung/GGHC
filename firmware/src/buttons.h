#ifndef  __BUTTONS_H
#define  __BUTTONS_H

#include "qp_port.h"

#define nBUTTONS 5

extern const int buttonDebounceTime;

typedef struct button
{
    GPIO_TypeDef* port;
    const uint16_t pin;
    bool pressed;
    uint32_t debounceTime;
} button_t;

extern button_t buttons[nBUTTONS];

void ButtonsInit(void);
void ButtonsRead(void);

// EV_BUTTON_PRESSED_SIG
// EV_BUTTON_RELEASED_SIG
typedef struct
{
    QEvent super;
    uint8_t buttonMask;
} ButtonEvent;

#endif
