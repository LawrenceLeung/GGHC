#include "jigbox.h"

Q_DEFINE_THIS_FILE

uint32_t volatile systemTime;

void SysTick_Handler(void)
{
    systemTime++;

    QF_tick();
#if 0
    static QEvent const tickEvt = { TIME_TICK_SIG, 0 };
    QActive_postFIFO(AO_IOEventListener, &tickEvt);
    QF_publish(&tickEvt);      /* publish the tick event to all subscribers */
#endif
    ButtonsRead();
}

void QF_onStartup(void)
{
    /* Set up and enable the SysTick timer.  It will be used as a reference
     * for delay loops in the interrupt handlers.  The SysTick timer period
     * will be set up for BSP_TICKS_PER_SEC.
     */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);
    systemTime = 0;
}

void QF_onCleanup(void)
{
}

void QF_onIdle(void) {              /* NOTE: entered with interrupts LOCKED */
    QF_INT_UNLOCK(dummy);                /* must at least unlock interrupts */
#if defined(NDEBUG)
    // TODO check for sleep mode support
    __WFI();                           /* wait for interrupt */
#endif
}

//   error routine that is called if the STM32 library encounters an error
void assert_failed(char const *file, int line)
{
    Q_onAssert(file, line);
}

void Q_onAssert(char const * const file, int line)
{
    UART_printf("ERROR: assert in file %s line %d\r\n", file, line);
    UART_flushTransmittedCharacters();
    QF_INT_LOCK();                     /* make sure that all interrupts are disabled */
    for (;; )                          /* NOTE: replace the loop with reset for final version */
    {
    }
}
