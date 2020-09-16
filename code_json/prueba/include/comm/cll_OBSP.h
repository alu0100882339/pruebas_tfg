#ifndef __CLL_OBSP_H__
#define __CLL_OBSP_H__ /* avoid multiple inclusion */

#include <bsp.h>

#define CLL_OBSP_IS_IO_MASTER	 !bsp_pid()
#define CLL_SYNC_(x) bsp_sync()
#define CLL_SYNC		 bsp_sync()

unsigned * CLL_OBSP_get_NUMTESTS(unsigned experiment_numtests) {
  unsigned * all_nt = NULL;
	int i;
	
	bsp_push_reg (&experiment_numtests, sizeof(unsigned));
	bsp_sync ();
	if (CLL_OBSP_IS_IO_MASTER) {
		all_nt = malloc(bsp_nprocs()*sizeof(unsigned));
		all_nt[0] = experiment_numtests;
		for (i = 1; i < bsp_nprocs(); i++) {
			bsp_hpget (i, &experiment_numtests, 0, &all_nt[i], sizeof(unsigned));
		}
	}
	bsp_sync();
	bsp_pop_reg (&experiment_numtests);
	bsp_sync ();

  return all_nt;
}

#define cll_report(experiment)     \
{ \
    int i; \
		unsigned *cll_numtests; \
\
	  cll_numtests = CLL_OBSP_get_NUMTESTS((experiment).numtests); \
		bsp_push_reg (&(experiment).instance,														\
									(experiment).numtests*sizeof((experiment).instance[0])); \
    if (CLL_OBSP_IS_IO_MASTER) { \
			cll_open_file(); \
			cll_print_header((experiment), "OBSP"); \
			\
			cll_print_exp_header((experiment)); \
			cll_print_NUMTESTS(cll_numtests,bsp_nprocs()); \
			cll_print_exp_fields_titles((experiment)); \
		} \
		bsp_sync(); \
		for (i = 1; i < bsp_nprocs(); i++) { \
			if (CLL_OBSP_IS_IO_MASTER) { \
				cll_print_experiment_samples((experiment).numidents, \
																		 (experiment).instance, \
				                             cll_numtests[i - 1], i - 1, \
				                             bsp_nprocs()); \
				bsp_hpget(i /*pid*/, &(experiment).instance, 0, \
				          &(experiment).instance, \
				          cll_numtests[i]*sizeof((experiment).instance[0]) );\
			} \
			bsp_sync(); \
		} \
		if (CLL_OBSP_IS_IO_MASTER) { \
			cll_print_experiment_samples((experiment).numidents, \
																	 (experiment).instance, \
			                             cll_numtests[i - 1], i - 1,bsp_nprocs()); \
			cll_print_end_of_experiment(experiment); \
		} \
		bsp_pop_reg (&(experiment).instance); \
		bsp_sync(); \
}

#endif /* __CLL_BSPLIB_H__ */
