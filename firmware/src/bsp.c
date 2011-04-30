#include "qp_port.h"
#include "bsp.h"
#include "jigbox.h"

Q_DEFINE_THIS_FILE

uint32_t volatile systemTime;

void SysTick_Handler(void)
{
#if 0
    static QEvent const tickEvt = { TIME_TICK_SIG, 0 };

    QF_tick();
    QF_publish(&tickEvt);      /* publish the tick event to all subscribers */
#endif
    systemTime++;
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

void QK_onIdle(void)
{
    QF_INT_LOCK();
    // do idle-loop processing
    QF_INT_UNLOCK();

#if defined(NDEBUG)
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
    (void)file;                        /* avoid compiler warning */
    (void)line;                        /* avoid compiler warning */
    QF_INT_LOCK();                     /* make sure that all interrupts are disabled */
    for (;; )                          /* NOTE: replace the loop with reset for final version */
    {
    }
}
