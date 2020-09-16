#ifndef __CLL_PVM_H__
#define __CLL_PVM_H__ /* avoid multiple inclusion */


#include "pvm3.h"


#ifndef CLL_NUM_PROCS
#define CLL_NUM_PROCS	2
#endif

#define CLL_SYNC     {pvm_joingroup("cll_pvm_group"); pvm_barrier("cll_pvm_group",CLL_NUM_PROCS); pvm_lvgroup("cll_pvm_group");}
/*#define CLL_SYNC_(x) pvm_barrier((x, ))*/

/* PVM interface for call */


unsigned * cll_get_NUMTESTS(unsigned experiment_numtests, int numproc) {
  int * all_nt = malloc(numproc*sizeof(int));
 
  pvm_gather(all_nt, &(experiment_numtests), 1, PVM_INT, 0, "cll_pvm_group", 0);
   return all_nt;
}

#define cll_report(experiment)   \
  {           \
    int i;    \
    int myid; \
    int mytid; \
    int bufid; \
    int * cll_numtests; \
    int proc, numproc; \
 \
      pvm_joingroup("cll_pvm_group"); \
      pvm_barrier("cll_pvm_group", CLL_NUM_PROCS);  \
      mytid = pvm_mytid(); \
      myid = pvm_getinst("cll_pvm_group", mytid); \
      numproc = pvm_gsize("cll_pvm_group");  \
      cll_numtests = cll_get_NUMTESTS((experiment).numtests, numproc);      \
 \
    if (!myid) { \
      	cll_open_file(); \
      	cll_print_header((experiment), "PVM"); \
      	cll_print_exp_header((experiment)); \
      	cll_print_NUMTESTS(cll_numtests,numproc); \
	cll_print_exp_fields_titles((experiment)); \
      	cll_print_experiment_samples((experiment).numidents, (experiment).instance, (experiment).numtests, myid, numproc); \
        for(proc = 1; proc < numproc; proc++) {  \
        /* receive & print data from processor proc */ \
\
	bufid= pvm_recv(pvm_gettid("cll_pvm_group", proc), proc); \
        pvm_upkbyte((char *)(experiment).instance, cll_numtests[proc]*sizeof((experiment).instance[0]),1); \
        cll_print_experiment_samples((experiment).numidents, (experiment).instance, cll_numtests[proc], proc, numproc); \
        pvm_barrier("cll_pvm_group", CLL_NUM_PROCS); /* barrier */ \
      } \
      cll_print_end_of_experiment(experiment); \
    }\
    else { \
      pvm_initsend(PvmDataDefault); \
      for(proc = 1; proc < numproc; proc++) {  \
        /* send data from processor proc to 0*/ \
        if (myid == proc) \
        pvm_pkbyte((char *)(experiment).instance, (experiment).numtests*sizeof((experiment).instance[0]),1); \
        pvm_send(pvm_gettid("cll_pvm_group", 0), proc); \
        pvm_barrier("cll_pvm_group", CLL_NUM_PROCS); /* barrier */ \
      } \
    }\
    pvm_lvgroup("cll_pvm_group"); \
}

#endif /* __CLL_PVM_H__ */
