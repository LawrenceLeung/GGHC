#include "stm32F10x.h"
#include "stm32f10x_gpio.h"
// TODO Not sure if the include below is necessary even for Discovery build
#ifdef STM_DISCOVERY
// TODO #include "STM32vldiscovery.h"
#endif
#include "stm32f10x_rcc.h"
#include "jigbox.h"
#include "stm32f10x_rcc.h"
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

button_t buttons[5];

/* Initialize all 5 buttons */
void ButtonsInit()
{
  int i;
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO button Clock */
  // TODO: is this necessary?
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  /* Configure the GPIO button pins */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  for(i = 0; i < nBUTTONS; i++){
    GPIO_InitStructure.GPIO_Pin = BUTTON_PINS[i];
    GPIO_Init(BUTTON_PORTS[i], &GPIO_InitStructure);
    buttons[i].debounceTime = sysTime;
    buttons[i].pressed = 0;
  }
}

void ReadButtons(){
  int i, pressed;
  for(i = 0; i < nBUTTONS; i++){
    pressed = GPIO_ReadInputDataBit(BUTTON_PORTS[i], BUTTON_PINS[i]);
    if (pressed != buttons[i].pressed &&
        sysTime - buttons[i].debounceTime > buttonDebounceTime)
    {
      buttons[i].debounceTime = sysTime;
      buttons[i].pressed = pressed;
    }
  }
}