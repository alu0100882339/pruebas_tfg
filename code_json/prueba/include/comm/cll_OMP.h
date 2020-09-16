/*************************************************
 * This is the OMP Skeleton                      *
 * new parallel communications drivers for CALL  *
 *                                               *
 * See the cll_OMP.tex file                      *
 * for details.                                  *
 *                                               *
 *************************************************/
#ifndef __CLL_OMP_H__
#define __CLL_OMP_H__

#ifdef _OPENMP
#include <omp.h>

#ifndef CLL_omp_MAX_NUM_THREADS
#define CLL_omp_MAX_NUM_THREADS 1024
#endif

/* variables used to implement the barrier */
int cll_omp_ARRIVED[CLL_omp_MAX_NUM_THREADS];

/* variable used to implement the communication of 
   the number of tests performed by each thread 
*/
unsigned cll_omp_all_nt[CLL_omp_MAX_NUM_THREADS];

/* variable used to implement the communication of the 
   experiments
*/
void * cll_omp_all_experiments[CLL_omp_MAX_NUM_THREADS];

void cll_omp_barrier() {
  if (omp_get_thread_num() == 0) {
    int i, ended;
    do { /* wait until every proc has arrived to the barrier */
      ended = 1;
      for(i=1; i < omp_get_num_threads(); i++)
        if (!(cll_omp_ARRIVED[i]))
          ended = 0;
    } while (!ended);
    /* reset the ARRIVED array so that the other procs
       can continue
    */
    for(i=1; i < omp_get_num_threads(); i++)
        cll_omp_ARRIVED[i] = 0;
/* The flush directives produce a warning when compiled
	 in the Origin systems 
	 #pragma omp flush(cll_omp_ARRIVED) */
  } /* end MASTER */
  else {
    cll_omp_ARRIVED[omp_get_thread_num()] = 1;
    while (cll_omp_ARRIVED[omp_get_thread_num()]) {
/* #pragma omp flush(cll_omp_ARRIVED) */
    }
  }
}

#define CLL_SYNC cll_omp_barrier()
#define CLL_SYNC_(x) cll_omp_barrier()

/* "bug" cll_omp_numidents: when using omni openmp we have found
 * that  (experiment).numidents  is zero for other processors
 * than the master. This is probably due to the fact
 * that the compiler does the intialization of threadprivate
 * variables only in the master but not in the others.
 *
 */

#define cll_report(experiment)     { \
    int cll_omp_myid; \
    int cll_omp_thread, cll_omp_num_threads; \
    int cll_omp_numidents; \
 \
    if (omp_in_parallel()) {\
      cll_omp_num_threads = omp_get_num_threads(); \
      cll_omp_myid = omp_get_thread_num(); \
      cll_omp_all_nt[cll_omp_myid] = (experiment).numtests; \
      cll_omp_numidents = (experiment).numidents; \
      cll_omp_all_experiments[cll_omp_myid] = (void *) &(experiment); \
      cll_omp_barrier(); \
      if (!cll_omp_myid) { \
        cll_open_file(); \
        cll_print_header((experiment), "OMP"); \
        cll_print_exp_header((experiment)); \
        cll_print_NUMTESTS(cll_omp_all_nt, cll_omp_num_threads); \
        cll_print_exp_fields_titles((experiment)); \
        cll_print_experiment_samples(cll_omp_numidents, (experiment).instance, (experiment).numtests, cll_omp_myid, cll_omp_num_threads); \
        for(cll_omp_thread=1; cll_omp_thread < cll_omp_num_threads; cll_omp_thread++) {\
          /* warning not optimized. Copies more that what it needs */\
          memcpy(&(experiment), cll_omp_all_experiments[cll_omp_thread], sizeof(experiment));\
          cll_print_experiment_samples(cll_omp_numidents, (experiment).instance, (experiment).numtests, cll_omp_thread, cll_omp_num_threads); \
        }\
        cll_print_end_of_experiment(experiment); \
      }\
      cll_omp_barrier(); \
    }\
    else {\
      fprintf(stderr,"Sorry. The cll REPORT clause has to be invoked from inside a parallel region\n");\
      fprintf(stderr,"No report was generated for %s\n",experiment.name);\
    }\
}
#else /* if defined _OPENMP */

#include "comm/cll_SEQ.h"

#endif /* if defined _OPENMP */
#endif /* __CLL_OMP_H__ */
