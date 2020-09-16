/*
 * This header provides a series of functions that are able
 * to take measures in a Sunfire environment.
 */

/* 
 * PROVIDES THE FOLLOWING OBSERVABLES:
 *    CLOCK
 */

/* CLOCK */

#ifndef Sunfire_1
#define  Sunfire_1

#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#define _STRUCTURED_PROC 1
#include <sys/procfs.h>

/* Enable Microstate Accounting from inside the program */
/* Solaris 8 4/01 s28s_u4wos_08 SPARC */
/* Casiano at EPCC */
#define UNISTD_INITIALIZATION() {   \
  {  \
    /* SunOS 5.5. or higher */  \
    int ctlfd;  \
    long ctl[2];  \
    char procname[1024];  \
  \
    sprintf(procname,"/proc/%d/ctl",getpid());  \
    ctlfd = open(procname, O_WRONLY);  \
    if (ctlfd < 0) {  \
      fprintf(stderr,"Sunfire Observable Driver: open %s failed, errno = %d\n",  \
              procname, errno);  \
    }  \
    ctl[0] = PCSET;  \
    ctl[1] = PR_MSACCT;  \
    if (write(ctlfd, ctl, 2*sizeof(long)) < 0) {  \
      fprintf(stderr, "Sunfire Observable Driver: write failed, errno = %d\n",errno);  \
    }  \
    close(ctlfd);  \
          /*  \
          * What we do is just to load the function at least once...  \
          * This is an attempt to reduce the anomalous  \
          * effects usually observed for the first gauge.  \
          */  \
          { hrtime_t tv; tv = gethrtime(); }  \
    return;  \
  }  \
}

#define MICROSEC_PER_NANO_SEC 1000000000.0 

/* --------------------------------------------------------------------
Macro: GETTIME
Description: Gets system time in a hrtime_t  = long long  type (defined
  in <sys/time.h>) 
----------------------------------------------------------------------- */
#define GETTIME(tv) ((tv) = gethrtime())

/* --------------------------------------------------------------------
Macro: DIFTIME
Description: Compute the time in nsec between the time passing in
  tv2 and tv1. The result is giving in t (double).
   -------------------------------------------------------------------- */
#define DIFTIME(tv2,tv1,t) ((t)=(double)((tv2)-(tv1)))

#define CLOCK_INIT(exp)

#define CLOCK_DECL double CLOCK

#define CLOCK_TMPS hrtime_t CLOCK1,CLOCK2

#define CLOCK_START_OBS(exp) ((exp).CLOCK1 = gethrtime())

#define CLOCK_STOP_OBS(exp)  ((exp).CLOCK2 = gethrtime())

#define CLOCK_SAVE_INSTANCE_OPT(exp2,exp1,inst) \
	((exp1).instance[(inst)].CLOCK) = ((double)((exp2).CLOCK2 - (exp1).CLOCK1)/MICROSEC_PER_NANO_SEC)

#define CLOCK_SAVE_INSTANCE(exp,inst) \
	CLOCK_SAVE_INSTANCE_OPT((exp),(exp),(inst)) 
	
#define CLOCK_PRINT_TITLE(exp)	cll_print_field_title("%10s  ", "CLOCK"); 
#define CLOCK_PRINT_VALUE(exp,instance) \
				cll_print_field_value("%10.8f  ", (instance).CLOCK); 

#define CLOCK_FINALIZE(exp)


#endif /* Sunfire_1 */

