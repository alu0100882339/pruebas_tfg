/* cll_exp.h
 *
 * OUTPUT FORMAT: XML (cll_data.dtd)
 *
 */


#ifndef __CLL_EXPERIMENT_HH__
#define __CLL_EXPERIMENT_HH__

#ifdef __cplusplus
extern "C" {
#endif

#define cll_open_file() { \
  if (!cll_fdata) {  \
    if (!(cll_output_name)) { \
      cll_fdata = stdout; \
    } \
    else { \
      (cll_output_name) = cll_foutname((cll_output_name)); \
      fprintf(stderr,"FILE: %s\n",(cll_output_name)); \
      if (!(cll_fdata = fopen((cll_output_name) , "w"))) { \
        fprintf(stdout, "error: cannot open output file\n"); \
        exit(1); \
      } \
    } \
  } \
}

struct json_object *json;
struct json_object *cll_experiment;
struct json_object *headers;
struct json_object *sample;
struct json_object *samples;
#define start_json() {\
  json = json_object_new_object(); \
}

#define cll_print_header(experiment,driver) { \
  if(!cll_first_report) { \
    struct utsname machine; \
    uname(&machine); \
    json_object *call_version = json_object_new_string(cll_version);\
    json_object *program = json_object_new_string(cll_programname);\
    json_object *nodename = json_object_new_string(machine.nodename);\
    json_object *sysname = json_object_new_string(machine.sysname);\
    json_object *release = json_object_new_string(machine.release);\
    json_object *version = json_object_new_string(machine.version);\
    json_object *parallel_id = json_object_new_string(driver);\
\
    json_object_object_add(json, "CALL_VERSION", call_version);\
    json_object_object_add(json, "PROGRAM", program);\
    json_object_object_add(json, "NODE_NAME", nodename);\
    json_object_object_add(json, "SYSNAME", sysname);\
    json_object_object_add(json, "RELEASE", release);\
    json_object_object_add(json, "VERSION", version);\
    json_object_object_add(json, "PARALLEL_ID", parallel_id);\
  \
    cll_first_report++; \
  } \
}

#define cll_print_exp_header(experiment)  { \
  unsigned *tmp; \
  char buffer [50];\
  int i; \
  cll_experiment = json_object_new_object();\
  json_object *experiments = json_object_new_string( (experiment).name );\
  \
  json_object *begins = json_object_new_array();  \
  for(tmp = (experiment).begin_lines; *tmp; tmp++){\
    json_object *begin_line = json_object_new_int(*tmp);\
    json_object_array_add(begins,begin_line);\
  }\
  \
  json_object *ends = json_object_new_array();\
  for(tmp = (experiment).end_lines; *tmp; tmp++){ \
  \
    json_object *end_line = json_object_new_int(*tmp);\
    json_object_array_add(ends,end_line);\
  }\
  \
  json_object *formula = json_object_new_string((experiment).formula);\
  json_object *informula = json_object_new_string((experiment).informula);\
  json_object *maxtest = json_object_new_int((experiment).maxtests);\
  json_object *dimension = json_object_new_int((experiment).dimension);\
  json_object *numidents = json_object_new_int((experiment).numidents);\
\
  json_object *idents = json_object_new_array();\
  for (i = (experiment).numidents -1; i>=0; i--) { \
    json_object *ident = json_object_new_string((experiment).idents[i]);\
    json_object_array_add(idents,ident);\
  }\
\
  json_object *observables = json_object_new_array();\
  for (i = (experiment).numobservables - 1; i >= 0; i--) {\
    json_object *observable = json_object_new_string((experiment).observables[i]);\
    json_object_array_add(observables,observable);\
  }\
\
  json_object *components = json_object_new_array(); \
  for(i = 0; i < (experiment).dimension; i++){ \
    json_object *component = json_object_new_string((experiment).components[i]);\
    json_object_array_add(components,component);\
  }\
\
  json_object_object_add(cll_experiment, "EXPERIMENT", experiments);\
  json_object_object_add(cll_experiment, "BEGIN_LINE", begins);\
  json_object_object_add(cll_experiment, "END_LINE", ends);\
  json_object_object_add(cll_experiment, "FORMULA", formula);\
  json_object_object_add(cll_experiment, "INFORMULA", informula);\
  json_object_object_add(cll_experiment, "MAXTESTS", maxtest);\
  json_object_object_add(cll_experiment, "DIMENSION", dimension);\
  json_object_object_add(cll_experiment, "NUMIDENTS", numidents);\
  json_object_object_add(cll_experiment, "IDENTS", idents);\
  json_object_object_add(cll_experiment, "OBSERVABLES", observables);\
  json_object_object_add(cll_experiment, "COMPONENTS", components);\
\
}

#define cll_print_NUMTESTS(cll_nt, len) { \
  int cll_i;\
  json_object *numtests = json_object_new_array();\
  for(cll_i = 0; cll_i < (len); cll_i++){\
    json_object *numtest = json_object_new_int( (cll_nt)[cll_i]);\
    json_object_array_add(numtests,numtest);\
 }\
  json_object_object_add(cll_experiment, "NUMTESTS", numtests);\
  \
}


#define cll_print_field_title(str) { \
  json_object *string = json_object_new_string(str);\
  json_object_array_add(headers,string);\
}


#define cll_print_field_value(str) { \
  json_object *string = json_object_new_int(str);\
  json_object_array_add(sample,string);\
}

/*#define cll_print_exp_fixed_fields_titles() { \
	fprintf(cll_fdata, "      <h>%s</h>\n", "CPU"); \
	fprintf(cll_fdata, "      <h>%s</h>\n", "NCPUS"); \
}*/


#define cll_print_exp_fixed_fields(processor,ncpus) { \
    json_object *pro = json_object_new_int(processor);\
    json_object *ncpu = json_object_new_int(ncpus);\
    json_object_array_add(sample,pro);\
    json_object_array_add(sample,ncpu);\
}


#define cll_print_exp_fields_titles(experiment) { \
  int cll_i;\
  headers = json_object_new_array();\
  json_object *cpu = json_object_new_string("CPU");\
  json_object *ncpus = json_object_new_string("NCPUS");\
  json_object_array_add(headers,cpu);\
  json_object_array_add(headers,ncpus);\
  \
  for (cll_i = (experiment).numidents - 1; cll_i >= 0; cll_i--) { \
		json_object *experiments = json_object_new_string( (experiment).idents[cll_i] );\
    json_object_array_add(headers,experiments);\
	} \
  CLL_USERDEF_TITLES(); \
  json_object_object_add(cll_experiment, "headers", headers);\
  samples = json_object_new_array(); \
}


#define cll_print_experiment_samples(numidents,instances,numberoftests,processor,ncpus) { \
  int cll_i, cll_k; \
  for (cll_i = 0; cll_i < (numberoftests); cll_i++){ \
    sample = json_object_new_array();\
    cll_print_exp_fixed_fields(processor,ncpus);\
    for(cll_k = 0; cll_k <numidents; cll_k++){\
       json_object *instance = json_object_new_int(instances[cll_i].id[cll_k]);\
       json_object_array_add(sample,instance);\
    }\
    CLL_USERDEF_VALUES(instances[cll_i]); \
    json_object_array_add(samples,sample);\
  } \
}

/*
#define cll_print_end_of_experiment(experiment) fprintf(cll_fdata," </cll_experiment>\n")
*/
#define cll_print_end_of_experiment(experiment) { \
  fpos_t cll_fdata_end; \
  fgetpos(cll_fdata, &cll_fdata_end); \
  fsetpos(cll_fdata, &cll_fdata_end); \
}

#ifdef __cplusplus
}
#endif


#endif /* __CLL_EXPERIMENT_HH__ */
