/* ---------------------------------------------------------------------
 * File:   cll.FLOAT.h
 * Date:   Wed April  2003
 * Description: Flops using the PAPI_flops function (PAPI High Level interface)
 * ----------------------------------------------------------------------- */

#ifndef _CLL_FLOAT_H
#define  _CLL_FLOAT_H
#include <papi.h> /* Using the papi library for clocks */
#include <papiStdEventDefs.h>

#define NDEBUG
#include <assert.h>

#ifdef _OPENMP
#error CALL Measure Driver FLOAT does not support OpenMP
#endif


unsigned long long cll_tsc_rate;

#define FLOAT_INITIALIZATION() 

/* --------------------------------------------------------------------
Macro: GETTIME
Description: 
----------------------------------------------------------------------- */
#define GETTIME(tv) (PAPI_flops(&tv.rtime, &tv.ptime, &tv.flpins, &tv.mflops)

/* --------------------------------------------------------------------
Macro: DIFTIME
----------------------------------------------------------------------- */
#define DIFTIME(tv2, tv1, t) ((t) = ((tv2).clk - (tv1).clk))

#define FLOAT_INIT(exp)

typedef struct { 
                 float rtime, ptime, mflops; 
                 long_long flpins; 
                } cll_FLOAT_OBSERVABLE;

#define FLOAT_DECL  cll_FLOAT_OBSERVABLE FLOAT

#define FLOAT_TMPS cll_FLOAT_OBSERVABLE FLOAT1, FLOAT2

#define FLOAT_START_OBS(exp) GETTIME((exp).FLOAT1)

#define FLOAT_STOP_OBS(exp) GETTIME((exp).FLOAT2)

#define FLOAT_SAVE_INSTANCE(exp,inst) \
	DIFTIME((exp).FLOAT2, (exp).FLOAT1, (exp).instance[(inst)].FLOAT)
	
#define FLOAT_PRINT_TITLE(exp)	cll_print_field_title("%12s  ", "FLOAT"); 

#define FLOAT_PRINT_VALUE(exp,instance) \
				/* cll_print_field_value("%llu  ", (instance).FLOAT); */ \
				cll_print_field_value("%Lf  ", (long double) ((long double) (instance).FLOAT)/ ((long double) cll_tsc_rate)); 

#define FLOAT_FINALIZE(exp)


#endif /* linux_intel_1 */

