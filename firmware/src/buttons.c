#include "jigbox.h"

button_t buttons[nBUTTONS] =
{
    { BUTTON_1_PORT, BUTTON_1_BIT, false, 0 },
    { BUTTON_2_PORT, BUTTON_2_BIT, false, 0 },
    { BUTTON_3_PORT, BUTTON_3_BIT, false, 0 },
    { BUTTON_4_PORT, BUTTON_4_BIT, false, 0 },
    { BUTTON_5_PORT, BUTTON_5_BIT, false, 0 }
};

const int buttonDebounceTime = 40;
/* Initialize all buttons */
void ButtonsInit()
{
    /* Configure the GPIO button pins */
    GPIO_InitTypeDef GPIO_InitStructure =
    {
        .GPIO_Mode  = GPIO_Mode_IPU,
        .GPIO_Speed = GPIO_Speed_50MHz
    };

    for (int i = 0; i < nBUTTONS; i++)
    {
        GPIO_InitStructure.GPIO_Pin = buttons[i].pin;
        GPIO_Init(buttons[i].port, &GPIO_InitStructure);
        buttons[i].debounceTime     = systemTime;
        buttons[i].pressed          = false;
    }
}

void ButtonsRead()
{
    static uint16_t oldButtonStates;
    uint16_t buttonStates = 0;
    uint16_t mask = 1;

    for (int i = 0; i < nBUTTONS; i++)
    {
        /* Each button pulls input low when pressed */
        bool pressed = (GPIO_ReadInputDataBit(buttons[i].port, buttons[i].pin) == 0);
        if (pressed)
        {
            buttonStates |= mask;
            if ((pressed != buttons[i].pressed)
                && (systemTime - buttons[i].debounceTime > buttonDebounceTime))
            {
                buttons[i].debounceTime = systemTime;
                buttons[i].pressed      = pressed;
            }
        }
        mask <<= 1;
    }

    // if any buttons pressed
    uint16_t changed = oldButtonStates ^ buttonStates;
    if (changed)
    {
    	ButtonEvent *evt;
        // any pressed?
        uint16_t buttonMask = changed & buttonStates;
        if (buttonMask)
        {
            evt = Q_NEW(ButtonEvent, BUTTON_PRESSED_SIG);
            evt->buttonMask = buttonMask;
            QActive_postFIFO(AO_IOEventListener, (QEvent *)evt);
        }

        // any released?
        buttonMask = changed & oldButtonStates;
        if (buttonMask)
        {
            evt = Q_NEW(ButtonEvent, BUTTON_RELEASED_SIG);
            evt->buttonMask = buttonMask;
            QActive_postFIFO(AO_IOEventListener, (QEvent *)evt);
        }

        oldButtonStates = buttonStates;
    }
}
