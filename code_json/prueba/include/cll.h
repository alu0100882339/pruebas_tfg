/* ---------------------------------------------------------------------
File:	cll.h
Date:	Mon Apr  30 09:02:42 GMT 2001

Description: This header contains macros for getting time using
  the routine from the specified software library. By default,
  time will be taken using the system routine gettimeofday().
----------------------------------------------------------------------- */

#ifndef __CLL_H__
#define __CLL_H__ /* avoid multiple inclusion */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <math.h>
#include <string.h>
#include "json-c/json.h"

#define  CLL_OK     0

/* --------------------------------------------------------------- */
/* General funtions                                                */
/* --------------------------------------------------------------- */

char cll_file_exists(char *s) /* returns 1 if file exists, 0 if not */
{                        /* -1 otherwise (errors) */
  FILE *f;
  char result;

  if (!s)
    return -1;

  result = ((f = fopen(s, "r")) != NULL);
  if (f)
    fclose(f);

  return result;
}


char *cll_foutname(char *s) /* Creates a new name appending a number */
{                           /* at the end, checking first the file   */
                            /* not exists or can't be oppened        */
  int counter = 0;
  char *result = NULL;

  if (!s || !s[0])
    return NULL; /* stdout */

  if (!cll_file_exists(s) || (cll_opt_overwrite == -1))
          result = s;
  else
  {
    result = (char *)malloc((int)strlen(s) + 32);
    do 
      sprintf(result, "%s.%u", s, counter++);
    while (cll_file_exists(result));
  }

  return result;
}

/* 
 * This functions creates a new name for the file appending a number
 * at the end. It checks that the number is the highest one assigned.
 *
 * THIS IS NOT SO EASY.... Have to think a little bit
 * The name has to be split in two parts: directory and name
#include <dirent.h>
char *cll_proposed_foutname(char *s) 
{
  int counter = 0;
  char *result = NULL;

  if (!s || !s[0])
    return NULL; 

  if (!cll_file_exists(s) || (cll_opt_overwrite == -1))
          result = s;
  else
  {
		struct dirent **namelist;
		int n;
		
		n = scandir(".", &namelist, 0, alphasort);
		if (n < 0) {
			perror("scandir");
		} 
		else {	
			while(n--) {
				printf("%s\n", namelist[n]->d_name);
				free(namelist[n]);
			}
		}
		result = malloc(strlen(s) + 32);
		do 
			sprintf(result, "%s.%u", s, counter++);
		while (cll_file_exists(result));
  }

  return result;
}
 */


/* --------------------------------------------------------------- */
/* Report generation MACROS                                        */
/* --------------------------------------------------------------- */

extern char * cll_programname;
extern char * cll_version;

int cll_user_measurements_libs_init();

static int cll_Init(void)
{
	cll_user_measurements_libs_init();
  return CLL_OK;
}


/*
 * When whether the following declaration
 * or one with (void) or one with "arg,..." 
 * is stated and the arguments doesn't match
 * an error is reported and compilation stopped.
double unknown() {
	return 0.0;
}
 * So I'll make it an empty "define", this works
 * as a function is *always* used as:
 * unknown(..., ..., ...)
 * well, sintactically it is correct to have
 * (..., ..., ...) (it returns the value of its
 * rightmost expression)
 */
#define unknown 


/* XML output */

#define CLL_DATA_DTD "cll_data [\n  <!ELEMENT cll_data (cll_experiment+)>\n    <!ATTLIST cll_data CALL_VERSION CDATA #REQUIRED>\n    <!ATTLIST cll_data PROGRAM      CDATA #REQUIRED>\n    <!ATTLIST cll_data NODE_NAME    CDATA #IMPLIED>\n    <!ATTLIST cll_data SYSNAME      CDATA #REQUIRED>\n    <!ATTLIST cll_data RELEASE      CDATA #REQUIRED>\n     <!ATTLIST cll_data VERSION      CDATA #REQUIRED>\n    <!ATTLIST cll_data PARALLEL_ID  CDATA #REQUIRED>\n  <!ELEMENT cll_experiment (machines*,headers,sample)>\n    <!ATTLIST cll_experiment EXPERIMENT   CDATA #REQUIRED>\n    <!ATTLIST cll_experiment BEGIN_LINE           CDATA #IMPLIED>\n    <!ATTLIST cll_experiment END_LINE             CDATA #IMPLIED>\n    <!ATTLIST cll_experiment FORMULA              CDATA #IMPLIED>\n    <!ATTLIST cll_experiment INFORMULA            CDATA #IMPLIED>\n    <!ATTLIST cll_experiment MAXTESTS             CDATA #IMPLIED>\n    <!ATTLIST cll_experiment DIMENSION            CDATA #IMPLIED>\n    <!ATTLIST cll_experiment PARAMETERS           CDATA #IMPLIED>\n    <!ATTLIST cll_experiment NUMIDENTS            CDATA #IMPLIED>\n    <!ATTLIST cll_experiment IDENTS               CDATA #IMPLIED>\n    <!ATTLIST cll_experiment OBSERVABLES        CDATA #IMPLIED>\n    <!ATTLIST cll_experiment COMPONENTS           CDATA #IMPLIED>\n    <!ATTLIST cll_experiment POSTFIX_COMPONENT_0  CDATA #IMPLIED>\n    <!ATTLIST cll_experiment POSTFIX_COMPONENT_1  CDATA #IMPLIED>\n    <!ATTLIST cll_experiment NUMTESTS             CDATA #IMPLIED>\n  <!ELEMENT machines (node)+>\n  <!ELEMENT node (#PCDATA)>\n      <!ELEMENT headers (h)+>\n  <!ELEMENT h (#PCDATA)>\n  <!ELEMENT sample (row)+>\n  <!ELEMENT row (c)+>\n  <!ELEMENT c (#PCDATA)>\n]"

#define cll_xml_clldata_attributes(root_node) { \
    struct utsname machine; \
    xmlNewProp(root_node, BAD_CAST "CALL_VERSION", BAD_CAST cll_version); \
    xmlNewProp(root_node, BAD_CAST "PROGRAM", BAD_CAST cll_programname); \
    uname(&machine); \
    xmlNewProp(root_node, BAD_CAST "SYSNAME", BAD_CAST machine.sysname); \
    xmlNewProp(root_node, BAD_CAST "RELEASE", BAD_CAST machine.release); \
    xmlNewProp(root_node, BAD_CAST "VERSION", BAD_CAST machine.version); \
    xmlNewProp(root_node, BAD_CAST "PARALLEL_ID", BAD_CAST "NWS_MPI"); \
}

#define cll_xml_print_dtd() { \
  fprintf(cll_fdata,"<!DOCTYPE cll_data [\n");\
  fprintf(cll_fdata,"  <!ELEMENT cll_data (cll_experiment+)>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_data CALL_VERSION CDATA #REQUIRED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_data PROGRAM      CDATA #REQUIRED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_data NODE_NAME    CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_data SYSNAME      CDATA #REQUIRED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_data RELEASE      CDATA #REQUIRED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_data VERSION      CDATA #REQUIRED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_data PARALLEL_ID  CDATA #REQUIRED>\n");\
  fprintf(cll_fdata,"  <!ELEMENT cll_experiment (machines*,headers,sample)>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment EXPERIMENT   CDATA #REQUIRED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment BEGIN_LINE           CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment END_LINE             CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment FORMULA              CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment INFORMULA            CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment MAXTESTS             CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment DIMENSION            CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment PARAMETERS           CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment NUMIDENTS            CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment IDENTS               CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment OBSERVABLES          CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment COMPONENTS           CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment POSTFIX_COMPONENT_0  CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment POSTFIX_COMPONENT_1  CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"    <!ATTLIST cll_experiment NUMTESTS             CDATA #IMPLIED>\n");\
  fprintf(cll_fdata,"  <!ELEMENT machines (node)+>\n");\
  fprintf(cll_fdata,"  <!ELEMENT node (#PCDATA)>\n");\
  fprintf(cll_fdata,"  <!ELEMENT headers (h)+>\n");\
  fprintf(cll_fdata,"  <!ELEMENT h (#PCDATA)>\n");\
  fprintf(cll_fdata,"  <!ELEMENT sample (row)+>\n");\
  fprintf(cll_fdata,"  <!ELEMENT row (c)+>\n");\
  fprintf(cll_fdata,"  <!ELEMENT c (#PCDATA)>\n");\
  fprintf(cll_fdata,"]>\n\n");\
}

#ifdef __cplusplus
}
#endif

#endif /* __CLL_H__ */
