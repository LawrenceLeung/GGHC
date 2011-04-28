#include "systick.h"
#include "jigbox.h"
static uint32_t volatile mainLoopDelay;

// TODO Just temp till I figure out the system tick
static uint32_t systemTick = 0;

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

// TODO This does not work as expected, kludging because I need to make progress :(
// returns the SysTick value
uint32_t systemTime(void)
{
	// TODO return SysTick->VAL;
    return systemTick;
}

void incSystemTime(void)
{
	systemTick++;
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
