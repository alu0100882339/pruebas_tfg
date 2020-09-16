/*
 * PAPI_EVENT DEFINITIONS 
 */
#define PAPI_EVENT_INIT(exp) \
	 (exp).PAPI_EVENT_MASK = PAPI_EVENT; \
   (exp).PAPI_EVENT_SET = add_test_event((exp).PAPI_EVENT_MASK) 
	 /* ; printf("INIT:PAPI_EVENT_SET handle: %d\n",(exp).PAPI_EVENT_SET); */
				
#define PAPI_EVENT_TMPS  int PAPI_EVENT_SET, \
                             PAPI_EVENT_MASK; \
                         long long PAPI_EVENT_VALUE
												 
#define PAPI_EVENT_DECL  long long PAPI_EVENT_VAR

#define PAPI_EVENT_START_OBS(exp) \
				 /*{  */\
				 /*  int retval;  */\
         /*  retval = */\
             PAPI_start((exp).PAPI_EVENT_SET) 
				 /*		; \*/
				 /*  if (retval != PAPI_OK) {\*/
				 /*  printf("ERROR:PAPI START: %s\n", "EVENT", (exp).numtests);\*/
				 /*		exit(1);\*/
				 /*	 }\*/
				 /*}*/

#define PAPI_EVENT_STOP_OBS(exp)  \
				 /*{  */\
				 /*  int retval;  */\
         /*	 retval =  */\
             PAPI_stop((exp).PAPI_EVENT_SET,&(exp).PAPI_EVENT_VALUE) 
				 /*		; \*/
				 /*  if (retval != PAPI_OK) {\*/
				 /*    printf("ERROR:PAPI STOP: %s\n", "EVENT", (exp).numtests);\*/
				 /*			exit(1);\*/
				 /*	 }\*/
				 /*}*/ 


#define PAPI_EVENT_SAVE_INSTANCE_OPT(exp2,exp1,inst)  \
        (exp1).instance[(inst)].PAPI_EVENT_VAR = (exp1).PAPI_EVENT_VALUE

#define PAPI_EVENT_SAVE_INSTANCE(exp,inst) \
        PAPI_EVENT_SAVE_INSTANCE_OPT((exp),(exp),(inst))

#define PAPI_EVENT_PRINT_TITLE(exp)	\
        cll_print_field_title("%14s  ", "PAPI_EVENT")
#define PAPI_EVENT_PRINT_VALUE(exp,instance) \
        cll_print_field_value("%14lld  ", (instance).PAPI_EVENT_VAR); 


#define PAPI_EVENT_FINALIZE(exp) \
   remove_test_event(&(exp).PAPI_EVENT_SET,(exp).PAPI_EVENT_MASK)


