/* ---------------------------------------------------------------------
 * File:   cll.PAPITIME.h
 * Date:   Wed April  2003
 * Description: Implementa el observable PAPIFILE 
 * Ver fichero de uso README.TXT en CALL/test/jgonzal/PAPITIME
 * ----------------------------------------------------------------------- */

#ifndef _CLL_PAPITIME_H
#define  _CLL_PAPITIME_H
#include <papi.h> /* Using the papi library for clocks */
#include <papiStdEventDefs.h>

#define PAPITIME_INITIALIZATION() FUNC_PAPITIME_INIT()

void FUNC_PAPITIME_INIT(void)
{
  int retvalue;
  retvalue = PAPI_library_init(PAPI_VER_CURRENT);
  if ( retvalue != PAPI_VER_CURRENT)  
      printf("\nError:PAPI is not ready in your system: Error %s\n",PAPI_strerror(retvalue));
}

/* --------------------------------------------------------------------
 * Observable PAPIFILE
 * -------------------------------------------------------------------- */
		
#define PAPIFILE_INIT(exp) FUNC_PAPITIME_INIT();
#define PAPIFILE_MAXOBSERVABLES 3
#define PAPIFILE_MAXCHAR 80

typedef struct  {
                int Counters[PAPIFILE_MAXOBSERVABLES];
	        long_long Values[PAPIFILE_MAXOBSERVABLES];
	        int NumCounters;	
		int EventSet;
		int MultiplexMode;
		char strHeaders[PAPIFILE_MAXCHAR];
		char strValues[PAPIFILE_MAXCHAR];
		long_long PAPI_REAL_USEC;
                } cll_PAPIFILE_OBSERVABLE;


void PAPITIME_PAPIFILE_Print_Values(cll_PAPIFILE_OBSERVABLE *o); 
void PAPITIME_PAPIFILE_Start_Counters(cll_PAPIFILE_OBSERVABLE *o); 
void PAPITIME_PAPIFILE_Stop_Counters(cll_PAPIFILE_OBSERVABLE *o);


void PAPITIME_PAPIFILE_Start_Counters(cll_PAPIFILE_OBSERVABLE *o) 
{
  int retvalue,i;

  o->EventSet = PAPI_NULL;

  if(o->NumCounters < 1)
      {
      o->PAPI_REAL_USEC = PAPI_get_real_usec();
      return;    /* nothing to do */
      }

  if(o->MultiplexMode == 1)
      {
      retvalue = PAPI_multiplex_init();
      if(retvalue != PAPI_OK)
          printf("\nError setting multiplex mode: %s\n",PAPI_strerror(retvalue));
      }
  
  retvalue = PAPI_create_eventset(&(o->EventSet));
  if(retvalue != PAPI_OK) 
      printf("\nError making EventSet %s\n",PAPI_strerror(retvalue));
  
  if(o->MultiplexMode == 1)
      { 
      retvalue = PAPI_set_multiplex(&(o->EventSet));
      if(retvalue != PAPI_OK)
          printf("\nError multiplexing set: %s\n",PAPI_strerror(retvalue));
      }
  
  for(i=0;i<o->NumCounters;i++)
      {
      retvalue = PAPI_add_event(&(o->EventSet),o->Counters[i]);
      if(retvalue != PAPI_OK)
          printf("\nError adding event %d to EventSet: %s\n",o->Counters[i],PAPI_strerror(retvalue)); 
      }
  retvalue = PAPI_start(o->EventSet);
  if(retvalue != PAPI_OK)
      printf("\nError %d starting counters: %s\n",retvalue,PAPI_strerror(retvalue));
  o->PAPI_REAL_USEC = PAPI_get_real_usec();
}



void PAPITIME_PAPIFILE_Stop_Counters(cll_PAPIFILE_OBSERVABLE *o) 
{
  int retvalue;
  long_long tmp;

  if(o->NumCounters < 1)
      {
      tmp = PAPI_get_real_usec();
      o->PAPI_REAL_USEC = tmp - o->PAPI_REAL_USEC;
      PAPITIME_PAPIFILE_Print_Values(o);
      return;    /* nothing to do */
      }

  retvalue = PAPI_stop(o->EventSet, o->Values);
  if(retvalue != PAPI_OK)
      printf("\nError %d stopping counters: %s\n",retvalue,PAPI_strerror(retvalue));
  else 
      {
      tmp = PAPI_get_real_usec();
      o->PAPI_REAL_USEC = tmp - o->PAPI_REAL_USEC;
      retvalue = PAPI_cleanup_eventset(&(o->EventSet));
      if(retvalue != PAPI_OK)
          printf("Error cleanning EventSet: %s",PAPI_strerror(retvalue));
      retvalue = PAPI_destroy_eventset(&(o->EventSet));
      if(retvalue != PAPI_OK)
          printf("Error destroying EventSet: %s",PAPI_strerror(retvalue));
      PAPITIME_PAPIFILE_Print_Values(o);
      }
}


void PAPITIME_PAPIFILE_Print_Values(cll_PAPIFILE_OBSERVABLE *o) 
{
  int i;
  char strtmp[20];

  sprintf(o->strValues," %15.6lf",(double)o->PAPI_REAL_USEC/1000000.0);
  for(i=0;i<o->NumCounters;i++) 
      {
      sprintf(strtmp," %15lld",o->Values[i]);
      strcat(o->strValues,strtmp);
      }
}

void PAPITIME_PAPIFILE_Read_File(cll_PAPIFILE_OBSERVABLE *o) 
{
  FILE *finput;
  char cad[100],cad2[100];
  int retval,code;

  o->NumCounters = -1;
  finput = fopen("papitime.obs","r");
  if (finput==NULL)
     printf("\nError openning papitime.obs file\n");
  if(!feof(finput))
      {
      retval = fscanf(finput,"%s\n",cad);
      if(retval <=0)
          printf("\nError reading file events header");
      else
	  {
          if(strcmp(cad,"MULTIPLEX")==0)
              o->MultiplexMode = 1;
	  else
              {
              if(strcmp(cad,"NOMULTIPLEX")==0)
	          o->MultiplexMode = 0;
	      else
		  printf("\nError getting multiplex mode from file papitime.obs line 1 %s\n",cad);
	      }
	  }
      }
  strcpy(o->strHeaders,"  PAPI_REAL_USEC");
  while (!feof(finput)) 
      {
      retval = fscanf(finput,"%s\n",cad);
      if(retval <= 0)
         printf("\nError reading line from file\n");
      else
	 {
         if(cad[0] != 35 && cad[1] != 35)
	     {
             if(PAPI_event_name_to_code(cad,&code) != PAPI_OK)
                 printf("\nError getting code for event name %s\n",cad);
             else 
	         {
                 o->NumCounters++;
                 o->Counters[o->NumCounters]=code;
		 sprintf(cad2," %15s",cad);
		 strcat(o->strHeaders,cad2);
                 }
	     }
	 }
     }
  o->NumCounters++;
  retval = PAPI_get_opt(PAPI_GET_MAX_HWCTRS,NULL);
  if(retval < o->NumCounters)
      {
      printf("\nWarning: Your system has only %d hardware counters and you try to set %d counters\n",retval,o->NumCounters);
      if(o->MultiplexMode == 0)
	  printf("\nError: Please, set Multiplex Mode to tray set more than %d counters\n",retval);
      else
	  printf("\nWarning: In Multiplex Mode, values can't be exactily\n");
      }
}




void FUNC_PAPIFILE_START_OBS(cll_PAPIFILE_OBSERVABLE *o) {
	PAPITIME_PAPIFILE_Read_File(o);
	PAPITIME_PAPIFILE_Start_Counters(o);
}

void FUNC_PAPIFILE_STOP_OBS(cll_PAPIFILE_OBSERVABLE *o) {
	PAPITIME_PAPIFILE_Stop_Counters(o);
}

void FUNC_PAPIFILE_SAVE_INSTANCE(cll_PAPIFILE_OBSERVABLE *o,cll_PAPIFILE_OBSERVABLE *o2) {
     int i;
     
     for(i=0;i<PAPIFILE_MAXOBSERVABLES;i++) {
	o2->Counters[i]=o->Counters[i];
        o2->Values[i]=o->Values[i];
        }
     o2->NumCounters=o->NumCounters;	
     strcpy(o2->strHeaders,o->strHeaders);
     strcpy(o2->strValues,o->strValues);
}

#define PAPIFILE_DECL  cll_PAPIFILE_OBSERVABLE PAPIFILE

#define PAPIFILE_TMPS cll_PAPIFILE_OBSERVABLE PAPIFILE1
#define PAPIFILE_START_OBS(exp) (FUNC_PAPIFILE_START_OBS(&((exp).PAPIFILE1)))

#define PAPIFILE_STOP_OBS(exp) (FUNC_PAPIFILE_STOP_OBS(&((exp).PAPIFILE1)))

#define PAPIFILE_SAVE_INSTANCE(exp,inst) FUNC_PAPIFILE_SAVE_INSTANCE(&((exp).PAPIFILE1),&((exp).instance[(inst)].PAPIFILE))
	
#define PAPIFILE_PRINT_TITLE(exp) cll_print_field_title("%s",(exp).PAPIFILE1.strHeaders)
#define PAPIFILE_PRINT_VALUE(exp,inst) cll_print_field_value("%s",(inst).PAPIFILE.strValues)

#define PAPIFILE_FINALIZE(exp)


#endif /* _CLL_PAPITIME_H */
