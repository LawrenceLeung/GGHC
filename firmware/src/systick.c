#include "systick.h"
#include "jigbox.h"
static uint32_t volatile mainLoopDelay;

/* Interrupt Handler */
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    if (mainLoopDelay != 0)
        mainLoopDelay--;
    /* other work here */
}

// returns the SysTick value
uint32_t systemTime(void)
{
    return SysTick->VAL;
}


void Init_SysTick(void)
{
    mainLoopDelay = 0;

    /* Setup SysTick Timer for desired interrupt rate */
    if (SysTick_Config(SystemCoreClock / SYS_TICK_USEC))
    {
        assert(0);
    }
}

// function to be called from the main loop to do an inefficient but timed delay
// ONLY CALL THIS FROM MAIN LOOP!
void delay(uint32_t ticks)
{
    mainLoopDelay = ticks;

    while (mainLoopDelay != 0)
    {
        /* spin */
    }
}
