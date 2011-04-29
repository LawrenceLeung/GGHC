#ifndef __INCLUDED_BSP_H
#define __INCLUDED_BSP_H

#define BSP_TICKS_PER_SEC 1000

extern uint32_t volatile systemTime;

extern void assert_failed(char const *file, int line);

#endif
