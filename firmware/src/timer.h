/*
 * @author Alan Widmer
 */

#ifndef TIMER_H_
#define TIMER_H_

void InitTimers(void);
void Clk_Init(void);
extern volatile bool LedUpdate;
extern volatile bool ButtonUpdate;

extern uint32_t SempPeriod, DeltaPer;

#endif /* TIMER_H_ */
