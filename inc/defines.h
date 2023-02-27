#ifndef __DEFINES_H
#define __DEFINES_H

/***********************
 *         CPU         *
 ***********************/

/* CPU frequency */
#define F_CPU 1000000UL
//#define F_CPU 2000000UL

// cpu cycles per microsecond
#define CYCLES_PER_US ((F_CPU+500000)/1000000)

//#define DEBUG_LED PORTD ^=  BV(PD7);
#include <avr/io.h>


//static inline void dbg_off(void) { PORTE |= (1 << 3); }
//static inline void dbg_on(void)  { PORTE &= ~(1 << 3); }

#endif
