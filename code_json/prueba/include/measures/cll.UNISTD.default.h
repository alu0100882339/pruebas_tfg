#include <sys/time.h>

/*
 * What we do is just to load the function at least once...
 * This has proven to reduce the effect of the first observation.
 */
#define UNISTD_INITIALIZATION() {struct timeval tv; gettimeofday(&tv,NULL); }

#define MICROSEC_PER_SEC 1000000 

/* --------------------------------------------------------------------
Macro: GETTIME
Description: Gets system time in a data struct of type timeval (defined
  in <sys/time.h> 
----------------------------------------------------------------------- */
#define GETTIME(tv) gettimeofday(&(tv), NULL)

/* --------------------------------------------------------------------
Macro: DIFTIME
Description: Compute the time in microsec between the time passing in
  the data structs tv2 and tv1. The result is giving in t (double).
   -------------------------------------------------------------------- */
#define DIFTIME(tv2,tv1,t) ((t)=(double)((tv2).tv_sec-(tv1).tv_sec)+  \
               (double)((tv2).tv_usec-(tv1).tv_usec) / MICROSEC_PER_SEC)

#define CLOCK_INIT(exp)

#define CLOCK_DECL double CLOCK

#define CLOCK_TMPS struct timeval CLOCK1,CLOCK2

#define CLOCK_START_OBS(exp) GETTIME((exp).CLOCK1)

#define CLOCK_STOP_OBS(exp) GETTIME((exp).CLOCK2)

#define CLOCK_SAVE_INSTANCE_OPT(exp2,exp1,inst) \
	DIFTIME((exp2).CLOCK2, (exp1).CLOCK1, (exp1).instance[(inst)].CLOCK)

#define CLOCK_SAVE_INSTANCE(exp,inst) \
	CLOCK_SAVE_INSTANCE_OPT((exp),(exp),(inst)) 
	
#define CLOCK_PRINT_TITLE(exp)	cll_print_field_title("%s", "CLOCK"); 
#define CLOCK_PRINT_VALUE(exp,instance) \
				cll_print_field_value("%10.8f", (instance).CLOCK); 

#define CLOCK_FINALIZE(exp)


/* --------------------------------------------------------------------
Function: clockPrec
Description: Return time spent in the execution of GETTIME
----------------------------------------------------------------------- *
double clockPrec(int nTest) {
  int i;
  double aa, asum;
  struct timeval tv1, tv2;

  for(asum=0,i=0;i<nTest;i++) {
    GETTIME(tv1);
    GETTIME(tv2);
    DIFTIME(tv2, tv1, aa);
    asum += aa;
  }
  asum /= nTest;
  return asum;
}
*/

