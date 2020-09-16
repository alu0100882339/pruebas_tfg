/* ---------------------------------------------------------------------
File:	cll.h
Date:	Sat Feb 10 12:43:47 GMT 2000

Description: PAPI
----------------------------------------------------------------------- */

#ifndef __CLL_PAPI_H
#define __CLL_PAPI_H  /* Avoid nested includes */

#include <papi.h> /* Using the papi library for clocks */
#include <papiStdEventDefs.h>

#define NDEBUG
#include <assert.h>

#ifdef _OPENMP
#include <omp.h>
#endif


#ifdef _OMPENMP
#define cll_THREADS { int retval; \
  retval = PAPI_thread_init((unsigned long (*)(void))(omp_get_thread_num),0); \
  if (retval != PAPI_OK) { \
    fprintf(stderr, "Can't initialize PAPI with OpenMP\n"); \
    exit(1); \
  } \
}
#else
#define cll_THREADS /* nothing */
#endif

/* 
 * GLOBAL INITIALIZATION OF PAPI
 */
#define PAPI_INITIALIZATION() { int tmp;\
                                        \
    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) exit(1);\
    if (PAPI_set_debug(PAPI_QUIET) != PAPI_OK) exit(1);\
    cll_THREADS\
    tmp = PAPI_get_opt(PAPI_DEFDOM, NULL);\
    printf("PAPI:Default domain is: %d (%s)\n", \
                    tmp, stringify_domain(tmp));\
    tmp = PAPI_get_opt(PAPI_DEFGRN, NULL);\
    printf("PAPI:Default granularity is: %d (%s)\n", \
                    tmp, stringify_domain(tmp));\
    tmp = PAPI_num_counters(); \
      printf("PAPI:There are %d hardware counters supported by the current substrate\n", tmp); \
      \
    /* return PAPI_OK; */\
}\
	
char *stringify_domain(int domain)
{
  switch(domain)
    {
    case PAPI_DOM_USER:
      return("PAPI_DOM_USER");
    case PAPI_DOM_KERNEL:
      return("PAPI_DOM_KERNEL");
    case PAPI_DOM_OTHER:
      return("PAPI_DOM_OTHER");
    case PAPI_DOM_ALL:
      return("PAPI_DOM_ALL");
    default:
      abort();
    }
  return(NULL);
}

char *stringify_granularity(int granularity)
{
  switch(granularity)
    {
    case PAPI_GRN_THR:
      return("PAPI_GRN_THR");
    case PAPI_GRN_PROC:
      return("PAPI_GRN_PROC");
    case PAPI_GRN_PROCG:
      return("PAPI_GRN_PROCG");
    case PAPI_GRN_SYS_CPU:
      return("PAPI_GRN_SYS_CPU");
    case PAPI_GRN_SYS:
      return("PAPI_GRN_SYS");
    default:
      abort();
    }
  return(NULL);
}

int add_test_event(int mask)
{
  int retval;
  int EventSet = PAPI_NULL;

  assert((retval = PAPI_num_counters()) >= PAPI_OK);

  if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
	printf("cll.PAPI.h:add_test_event: EventSet could not be created\n");
	exit(1);
  }	
    
  if (PAPI_query_event(mask) == PAPI_OK)
    {
      retval = PAPI_add_event(EventSet, mask);
      if (retval < PAPI_OK) {
	printf("cll.PAPI.h:add_test_event: %X can not be added\n", mask);
      }
    }
  else {
    printf("cll.PAPI.h:add_test_event: %X is not available\n", mask);
  }

  return(EventSet);
}

int add_test_event_r(int mask, int *handle)
{
  int retval;
  int EventSet = PAPI_NULL;

  assert((retval = PAPI_num_counters()) >= PAPI_OK);

  if (*handle != PAPI_NULL) {
	EventSet = *handle;
  }
  else {
	retval = PAPI_create_eventset(&EventSet);
  	assert(retval == PAPI_OK);
	*handle = EventSet;
  }
    
  if (PAPI_query_event(mask) == PAPI_OK)
    {
      retval = PAPI_add_event(EventSet, mask);
      if (retval < PAPI_OK) {
	printf("cll.PAPI.h:add_test_event_r: %X could not be added\n",
	       mask);
      }
    }
  else {
	printf("cll.PAPI.h:add_test_event_r: %X could is not available\n",
	       mask);
  }

  return(EventSet);
}


void remove_test_event(int *EventSet, int mask)
{
  int retval;

  retval = PAPI_remove_event(*EventSet, mask);
  assert(retval >= PAPI_OK);

  retval = PAPI_destroy_eventset(EventSet);
  assert(retval >= PAPI_OK); 
  assert(*EventSet == PAPI_NULL);

  return ;
}

/*
 * PAPI_CLOCK (USEC TO SEC) DEFINITIONS 
 */
#define PAPI_CLOCK_INIT(exp)

#define PAPI_CLOCK_DECL  double PAPI_CLOCK
#define PAPI_CLOCK_TMPS  long long PAPI_CLOCK1,PAPI_CLOCK2

#define PAPI_CLOCK_START_OBS(exp) ((exp).PAPI_CLOCK1) = PAPI_get_real_usec()
#define PAPI_CLOCK_STOP_OBS(exp)  ((exp).PAPI_CLOCK2) = PAPI_get_real_usec()

#define PAPI_CLOCK_DIFTIME(tv2,tv1,t) ((t)=((double)((tv2)-(tv1))) / 1.0e6)
#define PAPI_CLOCK_SAVE_INSTANCE_OPT(exp2,exp1,inst)  \
        PAPI_CLOCK_DIFTIME((exp2).PAPI_CLOCK2,(exp1).PAPI_CLOCK1, \
                           (exp1).instance[(inst)].PAPI_CLOCK)

#define PAPI_CLOCK_SAVE_INSTANCE(exp,inst) \
        PAPI_CLOCK_SAVE_INSTANCE_OPT((exp),(exp),(inst))

#define PAPI_CLOCK_PRINT_TITLE(exp)	\
       	cll_print_field_title("%10s  ", "PAPI_CLOCK")
#define PAPI_CLOCK_PRINT_VALUE(exp,instance) \
        cll_print_field_value("%10.8f  ", (instance).PAPI_CLOCK); 

#define PAPI_CLOCK_FINALIZE(exp)

/*
 * PAPI_REAL_USEC DEFINITIONS 
 */
#define PAPI_REAL_USEC_INIT(exp)

#define PAPI_REAL_USEC_DECL  long long PAPI_REAL_USEC
#define PAPI_REAL_USEC_TMPS  long long PAPI_REAL_USEC1,PAPI_REAL_USEC2

#define PAPI_REAL_USEC_START_OBS(exp) \
        ((exp).PAPI_REAL_USEC1) = PAPI_get_real_usec()
#define PAPI_REAL_USEC_STOP_OBS(exp)  \
        ((exp).PAPI_REAL_USEC2) = PAPI_get_real_usec()

#define PAPI_REAL_USEC_DIFTIME(tv2,tv1,t) ((t)=(((tv2)-(tv1))))
#define PAPI_REAL_USEC_SAVE_INSTANCE_OPT(exp2,exp1,inst)  \
        PAPI_REAL_USEC_DIFTIME((exp2).PAPI_REAL_USEC2,    \
                               (exp1).PAPI_REAL_USEC1, \
                               (exp1).instance[(inst)].PAPI_REAL_USEC)

#define PAPI_REAL_USEC_SAVE_INSTANCE(exp,inst) \
        PAPI_REAL_USEC_SAVE_INSTANCE_OPT((exp),(exp),(inst))

#define PAPI_REAL_USEC_PRINT_TITLE(exp)	\
        cll_print_field_title( "PAPI_REAL_USEC" )
#define PAPI_REAL_USEC_PRINT_VALUE(exp,instance) \
        cll_print_field_value( (instance).PAPI_REAL_USEC ); 

#define PAPI_REAL_USEC_FINALIZE(exp)

/*
 * PAPI_REAL_CYC DEFINITIONS 
 */
#define PAPI_REAL_CYC_INIT(exp)
				
#define PAPI_REAL_CYC_DECL  long long PAPI_REAL_CYC
#define PAPI_REAL_CYC_TMPS  long long PAPI_REAL_CYC1,PAPI_REAL_CYC2

#define PAPI_REAL_CYC_START_OBS(exp) \
        ((exp).PAPI_REAL_CYC1) = PAPI_get_real_cyc()
#define PAPI_REAL_CYC_STOP_OBS(exp)  \
        ((exp).PAPI_REAL_CYC2) = PAPI_get_real_cyc()

#define PAPI_REAL_CYC_DIFTIME(tv2,tv1,t) ((t)=(((tv2)-(tv1))))
#define PAPI_REAL_CYC_SAVE_INSTANCE_OPT(exp2,exp1,inst)  \
        PAPI_REAL_CYC_DIFTIME((exp2).PAPI_REAL_CYC2,(exp1).PAPI_REAL_CYC1, \
                              (exp1).instance[(inst)].PAPI_REAL_CYC)

#define PAPI_REAL_CYC_SAVE_INSTANCE(exp,inst) \
        PAPI_REAL_CYC_SAVE_INSTANCE_OPT((exp),(exp),(inst))

#define PAPI_REAL_CYC_PRINT_TITLE(exp)	\
        cll_print_field_title("%13s  ", "PAPI_REAL_CYC")
#define PAPI_REAL_CYC_PRINT_VALUE(exp,instance) \
        cll_print_field_value("%13lld  ", (instance).PAPI_REAL_CYC); 

#define PAPI_REAL_CYC_FINALIZE(exp)

#include <measures/cll.PAPI_EVENTS.h>

#endif /* __CLL_PAPI_H */
