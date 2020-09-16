#ifndef linux_intel_1
#define  linux_intel_1
#include <unistd.h>
#include <asm/msr.h>

/*
 * What we do is just to load the function at least once...
 * to reduce the effect of the first observation.
 */

unsigned long long cll_tsc_rate;

#define UNISTD_INITIALIZATION() { \
  cll_linuxintel_CLOCK tv1, tv2; \
  rdtsc(tv1.T.lo, tv1.T.hi); sleep(1); rdtsc(tv2.T.lo, tv2.T.hi); \
  cll_tsc_rate = ((tv2).clk - (tv1).clk); \
}

/* --------------------------------------------------------------------
Macro: GETTIME
Description: Gets system time in a data struct of type cll_linuxintel_CLOCK 
(see below)
----------------------------------------------------------------------- */
#define GETTIME(tv) rdtsc(tv.T.lo, tv.T.hi)

/* --------------------------------------------------------------------
Macro: DIFTIME
----------------------------------------------------------------------- */
#define DIFTIME(tv2, tv1, t) ((t) = ((tv2).clk - (tv1).clk))

#define CLOCK_INIT(exp)

typedef union { 
  unsigned long long clk; 
  struct { unsigned long lo, hi; } T; 
} cll_linuxintel_CLOCK;

#define CLOCK_DECL unsigned long long CLOCK

#define CLOCK_TMPS cll_linuxintel_CLOCK CLOCK1, CLOCK2

#define CLOCK_START_OBS(exp) GETTIME((exp).CLOCK1)

#define CLOCK_STOP_OBS(exp) GETTIME((exp).CLOCK2)

#define CLOCK_SAVE_INSTANCE(exp,inst) \
	DIFTIME((exp).CLOCK2, (exp).CLOCK1, (exp).instance[(inst)].CLOCK)
	
#define CLOCK_PRINT_TITLE(exp)	cll_print_field_title("%12s  ", "CLOCK"); 

#define CLOCK_PRINT_VALUE(exp,instance) \
				/* cll_print_field_value("%llu  ", (instance).CLOCK); */ \
				cll_print_field_value("%Lf  ", (long double) ((long double) (instance).CLOCK)/ ((long double) cll_tsc_rate)); 

#define CLOCK_FINALIZE(exp)


#endif /* linux_intel_1 */

