#include <time.h>
#include <sys/times.h>

/*
 * What we do is just to load the function at least once...
 * This has proven to reduce the effect of the first observation.
 */
#define USERTIME_INITIALIZATION() {; }

/*
 *               PUSERTIME
 */
#define PUSERTIME_INIT(exp)

#define PUSERTIME_DECL long unsigned PUSERTIME

#define PUSERTIME_TMPS clock_t PUSERTIME1,PUSERTIME2

#define PUSERTIME_START_OBS(exp) ((exp).PUSERTIME1 = clock())

#define PUSERTIME_STOP_OBS(exp)  ((exp).PUSERTIME2 = clock())

#define PUSERTIME_SAVE_INSTANCE_OPT(exp2,exp1,inst) \
	((exp1).instance[(inst)].PUSERTIME = (exp2).PUSERTIME2 - (exp1).PUSERTIME1)

#define PUSERTIME_SAVE_INSTANCE(exp,inst) \
	PUSERTIME_SAVE_INSTANCE_OPT((exp),(exp),(inst)) 
	
#define PUSERTIME_PRINT_TITLE(exp) cll_print_field_title("%10s  ", "PUSERTIME"); 
#define PUSERTIME_PRINT_VALUE(exp,instance) \
	cll_print_field_value("%10lu  ", (instance).PUSERTIME); 

#define PUSERTIME_FINALIZE(exp)

/*
 *               NUSERTIME
 */
#define NUSERTIME_INIT(exp)

#define NUSERTIME_DECL long unsigned NUSERTIME

#define NUSERTIME_TMPS struct tms NUSERTIME1,NUSERTIME2

#define NUSERTIME_START_OBS(exp) (times(&((exp).NUSERTIME1)))

#define NUSERTIME_STOP_OBS(exp)  (times(&((exp).NUSERTIME2)))

#define NUSERTIME_SAVE_INSTANCE_OPT(exp2,exp1,inst) \
	((exp1).instance[(inst)].NUSERTIME = (exp2).NUSERTIME2.tms_utime - (exp1).NUSERTIME1.tms_utime)

#define NUSERTIME_SAVE_INSTANCE(exp,inst) \
	NUSERTIME_SAVE_INSTANCE_OPT((exp),(exp),(inst)) 
	
#define NUSERTIME_PRINT_TITLE(exp) cll_print_field_title("%10s  ", "NUSERTIME"); 
#define NUSERTIME_PRINT_VALUE(exp,instance) \
	cll_print_field_value("%10lu  ", (instance).NUSERTIME); 

#define NUSERTIME_FINALIZE(exp)

/*
 *               NSYSTEMTIME
 */
#define NSYSTEMTIME_INIT(exp)

#define NSYSTEMTIME_DECL long unsigned NSYSTEMTIME

#define NSYSTEMTIME_TMPS struct tms NSYSTEMTIME1,NSYSTEMTIME2

#define NSYSTEMTIME_START_OBS(exp) (times(&((exp).NSYSTEMTIME1)))

#define NSYSTEMTIME_STOP_OBS(exp)  (times(&((exp).NSYSTEMTIME2)))

#define NSYSTEMTIME_SAVE_INSTANCE_OPT(exp2,exp1,inst) \
	((exp1).instance[(inst)].NSYSTEMTIME = (exp2).NSYSTEMTIME2.tms_stime - (exp1).NSYSTEMTIME1.tms_stime)

#define NSYSTEMTIME_SAVE_INSTANCE(exp,inst) \
	NSYSTEMTIME_SAVE_INSTANCE_OPT((exp),(exp),(inst)) 
	
#define NSYSTEMTIME_PRINT_TITLE(exp) cll_print_field_title("%10s  ", "NSYSTEMTIME"); 
#define NSYSTEMTIME_PRINT_VALUE(exp,instance) \
	cll_print_field_value("%10lu  ", (instance).NSYSTEMTIME); 

#define NSYSTEMTIME_FINALIZE(exp)

/*
 *               NCHILDUSERTIME
 */
#define NCHILDUSERTIME_INIT(exp)

#define NCHILDUSERTIME_DECL long unsigned NCHILDUSERTIME

#define NCHILDUSERTIME_TMPS struct tms NCHILDUSERTIME1,NCHILDUSERTIME2

#define NCHILDUSERTIME_START_OBS(exp) (times(&((exp).NCHILDUSERTIME1)))

#define NCHILDUSERTIME_STOP_OBS(exp)  (times(&((exp).NCHILDUSERTIME2)))

#define NCHILDUSERTIME_SAVE_INSTANCE_OPT(exp2,exp1,inst) \
	((exp1).instance[(inst)].NCHILDUSERTIME = (exp2).NCHILDUSERTIME2.tms_cutime - (exp1).NCHILDUSERTIME1.tms_cutime)

#define NCHILDUSERTIME_SAVE_INSTANCE(exp,inst) \
	NCHILDUSERTIME_SAVE_INSTANCE_OPT((exp),(exp),(inst)) 
	
#define NCHILDUSERTIME_PRINT_TITLE(exp) cll_print_field_title("%10s  ", "NCHILDUSERTIME"); 
#define NCHILDUSERTIME_PRINT_VALUE(exp,instance) \
	cll_print_field_value("%10lu  ", (instance).NCHILDUSERTIME); 

#define NCHILDUSERTIME_FINALIZE(exp)

/*
 *               NCHILDSYSTEMTIME
 */
#define NCHILDSYSTEMTIME_INIT(exp)

#define NCHILDSYSTEMTIME_DECL long unsigned NCHILDSYSTEMTIME

#define NCHILDSYSTEMTIME_TMPS struct tms NCHILDSYSTEMTIME1,NCHILDSYSTEMTIME2

#define NCHILDSYSTEMTIME_START_OBS(exp) (times(&((exp).NCHILDSYSTEMTIME1)))

#define NCHILDSYSTEMTIME_STOP_OBS(exp)  (times(&((exp).NCHILDSYSTEMTIME2)))

#define NCHILDSYSTEMTIME_SAVE_INSTANCE_OPT(exp2,exp1,inst) \
	((exp1).instance[(inst)].NCHILDSYSTEMTIME = (exp2).NCHILDSYSTEMTIME2.tms_cstime - (exp1).NCHILDSYSTEMTIME1.tms_cstime)

#define NCHILDSYSTEMTIME_SAVE_INSTANCE(exp,inst) \
	NCHILDSYSTEMTIME_SAVE_INSTANCE_OPT((exp),(exp),(inst)) 
	
#define NCHILDSYSTEMTIME_PRINT_TITLE(exp) cll_print_field_title("%10s  ", "NCHILDSYSTEMTIME"); 
#define NCHILDSYSTEMTIME_PRINT_VALUE(exp,instance) \
	cll_print_field_value("%10lu  ", (instance).NCHILDSYSTEMTIME); 

#define NCHILDSYSTEMTIME_FINALIZE(exp)

