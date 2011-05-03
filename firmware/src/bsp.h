#ifndef __INCLUDED_BSP_H
#define __INCLUDED_BSP_H

typedef uint32_t Ticks;

#define BSP_TICKS_PER_SEC 1000

// msec -> Ticks
#define MSEC(msec)  (msec)
// sec -> Ticks
#define SEC(sec)    ((sec)*BSP_TICKS_PER_SEC)

extern Ticks volatile systemTime;

extern void assert_failed(char const *file, int line);

#endif
