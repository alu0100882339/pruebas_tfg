#ifndef __CLL_PUB_H__
#define __CLL_PUB_H__ /* avoid multiple inclusion */

#define IS_MAIN_CPU	!bsp_pid(CLL_BSP_MACHINE)
#define CLL_SYNC_(x) bsp_sync(x)
#define CLL_SYNC		 bsp_sync(CLL_BSP_MACHINE)

/* This will output the results of a given experiment 
   It's implemented as a MACRO (not a function). 
   Here 'experiment' refers to the struct 
   containing the experiment to be reported (see <call.h>  
   for the experiment_type struct type definition) */

/* NOTE: This is the PARALELL PUB (BSP) version, so it's
   slightly different. Now the bsp structure need to be
   passed with the experiment (as a pointer to a 
   tbsp structure). It's needed to obtain the current 
   bsp_pid, among other things */

/* Only the main processor (bsp_pid == 0) will print to 
   the standard output. Remaining processors (if any) 
   will send its data to the main one */

#define cll_report(experiment) cll_pub_report(experiment,CLL_BSP_MACHINE)
#define cll_pub_report(experiment,bsp)                    \
  { 	\
    int i;                                             \
\
    if (!bsp_pid((bsp))) {  \
			cll_open_file(); \
    } \
\
    bsp_send((bsp), 0, &(experiment).numtests, sizeof((experiment).numtests)); \
    bsp_sync((bsp)); \
    if ((!bsp_pid((bsp)))) { /* Main processor? */         \
			 unsigned *cll_nts; \
			 cll_print_header ((experiment),"PUB");\
			 cll_print_exp_header ((experiment)); \
			 /* ,bsp_nprocs(bsp); */\
			 \
			 cll_nts = malloc(sizeof(unsigned) *bsp_nprocs(bsp)); \
			 if (cll_nts == NULL) { \
				 fprintf(stderr, "PUB Driver: Not enough memory for NUMTESTS\n"); \
				 abort(); \
			 } \
       for (i = 0; i < bsp_nprocs((bsp)); i++) { \
           t_bspmsg *msg; \
           if (!(msg = bsp_findmsg((bsp), i, 0))) \
	      continue; \
           cll_nts[i] = *((unsigned *)bspmsg_data(msg)); \
	     } \
			 cll_print_NUMTESTS(cll_nts,bsp_nprocs(bsp)); \
			 cll_print_exp_fields_titles((experiment)); \
    } /* End of general report */                      \
  }\
  /* ----  XXXX  ---- */ \
	bsp_send((bsp), 0, &(experiment).instance, sizeof((experiment).instance[0]) * (experiment).numtests); \
	bsp_sync((bsp)); \
	if ((!bsp_pid((bsp)))) { \
		t_bspmsg *msg; \
		int j, nsize; \
		/* struct instance { */\
		/*	double id[2]; */\
		/*	double clock; */\
		/* } *msgdata; */\
		void *msgdata; \
 \
		for (j = 0; j < bsp_nprocs((bsp)); j++) { \
			if (!(msg = bsp_findmsg((bsp), j, 0))) \
				continue; \
			msgdata = bspmsg_data(msg); \
			nsize = bspmsg_size(msg) / sizeof((experiment).instance[0]); \
			memcpy(&(experiment).instance, msgdata, bspmsg_size(msg));\
			cll_print_experiment_samples(experiment.numidents, experiment.instance, nsize, j, bsp_nprocs(bsp)); \
    } \
		cll_print_end_of_experiment((experiment)); \
}

#endif /* __CLL_PUB_H__ */
