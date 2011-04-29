#include "jigbox.h"
#include "buttons.h"

GPIO_TypeDef* BUTTON_PORTS[nBUTTONS] = {BUTTON_1_PORT,
                                        BUTTON_2_PORT,
                                        BUTTON_3_PORT,
                                        BUTTON_4_PORT,
                                        BUTTON_5_PORT};

const uint16_t BUTTON_PINS[nBUTTONS] = {BUTTON_1_BIT,
                                        BUTTON_2_BIT,
                                        BUTTON_3_BIT,
                                        BUTTON_4_BIT,
                                        BUTTON_5_BIT};

button_t buttons[nBUTTONS];
const int buttonDebounceTime = 40;
/* Initialize all buttons */
void ButtonsInit()
{
  int i;
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Configure the GPIO button pins */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  for(i = 0; i < nBUTTONS; i++)
  {
    GPIO_InitStructure.GPIO_Pin = BUTTON_PINS[i];
    GPIO_Init(BUTTON_PORTS[i], &GPIO_InitStructure);
    buttons[i].debounceTime = systemTime;
    buttons[i].pressed = false;
  }
}

void ButtonsRead()
{
  int i;
  bool pressed;
  for(i = 0; i < nBUTTONS; i++)
  { /* Each button pulls input low when pressed */
    pressed = (GPIO_ReadInputDataBit(BUTTON_PORTS[i], BUTTON_PINS[i]) == 0);
    if (pressed != buttons[i].pressed &&
        systemTime - buttons[i].debounceTime > buttonDebounceTime)
    {
      buttons[i].debounceTime = systemTime;
      buttons[i].pressed = pressed;
    }
  }
}
