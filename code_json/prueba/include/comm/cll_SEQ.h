#ifndef __CLL_SEQ_H__
#define __CLL_SEQ_H__ /* avoid multiple inclusion */

#define CLL_SYNC_(x)
#define CLL_SYNC


/* ------------------------------------------------------------- */
/* SEQUENTIAL experiment report                                  */
/* ------------------------------------------------------------- */

/* This will output the results of a given experiment 
   (implemented as a MACRO). 
   Here 'experiment' refers to the struct 
   containing the experiment to be reported (see <call.h>  
   for the 'experiment_type' struct type definition) */

#define cll_report(experiment) \
{ \
	cll_open_file(); \
  cll_print_header((experiment), "SEQ"); \
	cll_print_exp_header((experiment)); \
	cll_print_NUMTESTS(&(experiment).numtests,1); \
	cll_print_exp_fields_titles((experiment)); \
	cll_print_experiment_samples((experiment).numidents,  \
	                             (experiment).instance,   \
	                             (experiment).numtests,   \
	                             0 /* cpu id */, 1 /* ncpus */); \
	cll_print_end_of_experiment((experiment)); \
}

#endif /* __CLL_SEQ_H__ */
