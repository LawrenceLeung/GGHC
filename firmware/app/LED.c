#include "stm32F10x.h"
#include "stm32f10x_gpio.h"
#include "STM32vldiscovery.h"
#include "stm32f10x_rcc.h"
#include "jigbox.h"

#define nLEDs 9

GPIO_TypeDef* GPIO_PORTS[nLEDs] = {EXT_LED1_PORT,
                               EXT_LED2_PORT,
                               EXT_LED3_PORT, 
                               EXT_LED4_PORT,
                               EXT_LED5_PORT,
                               EXT_LED6_PORT,
                               EXT_LED7_PORT,
                               EXT_LED8_PORT,
                               EXT_LED9_PORT};

const uint16_t GPIO_PINS[nLEDs] = {EXT_LED1_PIN, 
                                 EXT_LED2_PIN, 
                                 EXT_LED3_PIN, 
                                 EXT_LED4_PIN, 
                                 EXT_LED5_PIN, 
                                 EXT_LED6_PIN, 
                                 EXT_LED7_PIN, 
                                 EXT_LED8_PIN, 
                                 EXT_LED9_PIN};

/* Initialize all 9 external LEDs */
void LEDInit()
{
  int i;
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  /* Configure the GPIO_LED pins */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  for(i = 0; i < nLEDs; i++){
    GPIO_InitStructure.GPIO_Pin = GPIO_PINS[i];
    GPIO_Init(GPIO_PORTS[i], &GPIO_InitStructure);
  }
}

void LEDOn(int led)
{
  GPIO_PORTS[led]->BRR = GPIO_PINS[led];
}

void LEDOff(int led)
{
  GPIO_PORTS[led]->BSRR = GPIO_PINS[led];   
}