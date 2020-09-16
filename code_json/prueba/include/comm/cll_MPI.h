#ifndef __CLL_MPI_H__
#define __CLL_MPI_H__ /* avoid multiple inclusion */
 

/* OBSOLETE: #define IS_MAIN_CPU	!cll_myid() */
#define CLL_SYNC		MPI_Barrier(MPI_COMM_WORLD)
#define CLL_SYNC_(x) MPI_Barrier((x))

#include "mpi.h"
/* MPI interface for call */

#ifdef __cplusplus
extern "C" {
#endif
 
unsigned * cll_get_NUMTESTS(unsigned experiment_numtests, int numproc) {
  unsigned * all_nt = (unsigned *)malloc(numproc*sizeof(unsigned));

  /* Warning! the type is MPI_UNSIGNED since numtest has being declared as that */
  MPI_Gather(&experiment_numtests, 1, MPI_UNSIGNED, all_nt, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  return all_nt;
}

#define CLL_MPI_NODENAMES_SIZE 40

#define cll_print_mpi_nodes(myid,numproc){\
  int cll_mpi_proc; \
  struct utsname cll_mpi_machine;\
  char cll_mpi_nodename[CLL_MPI_NODENAMES_SIZE];\
\
  json_object *machines = json_object_new_array();\
  if(!myid) { \
    uname(&cll_mpi_machine); \
    json_object *machine = json_object_new_string(cll_mpi_machine.nodename);\
    json_object_array_add(machines,machine);\
    for(cll_mpi_proc=1; cll_mpi_proc<numproc; cll_mpi_proc++) {\
      MPI_Status cll_status;\
      MPI_Recv(cll_mpi_nodename, CLL_MPI_NODENAMES_SIZE*sizeof(char), MPI_BYTE, cll_mpi_proc, cll_mpi_proc, MPI_COMM_WORLD, &cll_status); \
      MPI_Barrier(MPI_COMM_WORLD); /* barrier */ \
      json_object *machine_2 = json_object_new_string(cll_mpi_machine.nodename);\
      json_object_array_add(machines,machine_2);\
    }\
    json_object_object_add(cll_experiment, "machines", machines);\
  }\
  else {\
    uname(&cll_mpi_machine);\
    \
    sprintf(cll_mpi_nodename,"%s", cll_mpi_machine.nodename); \
    for(cll_mpi_proc = 1; cll_mpi_proc < numproc; cll_mpi_proc++) {  \
      /* send data from processor proc to 0*/ \
      if (myid == cll_mpi_proc) \
        MPI_Send(cll_mpi_nodename, CLL_MPI_NODENAMES_SIZE*sizeof(char), MPI_BYTE, 0, myid, MPI_COMM_WORLD); \
      MPI_Barrier(MPI_COMM_WORLD); /* barrier */ \
    } \
  }\
}

#define end_of_json() {\
  json_object_object_add(cll_experiment, "sample", samples);\
  json_object_object_add(json, "cll_experiment", cll_experiment);\
  fprintf(cll_fdata, json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY));\
}


#define cll_report(experiment)                     \
  {                              \
    int i;                                              \
    int myid; \
    unsigned * cll_numtests; \
    int proc, numproc; \
 \
    /* fprintf(stderr,"cll_report MPI\n"); */ \
 \
    MPI_Comm_rank(MPI_COMM_WORLD,&myid); \
    MPI_Comm_size(MPI_COMM_WORLD,&numproc); \
    cll_numtests = cll_get_NUMTESTS((experiment).numtests, numproc);      \
 \
    if (!myid) { \
      cll_open_file(); \
      start_json();\
			cll_print_header((experiment),"MPI"); \
			cll_print_exp_header((experiment)); \
			cll_print_NUMTESTS(cll_numtests,numproc); \
      cll_print_mpi_nodes(myid, numproc);\
      cll_print_exp_fields_titles((experiment));\
      cll_print_experiment_samples((experiment).numidents, (experiment).instance, (experiment).numtests, myid, numproc);\
      for(proc = 1; proc < numproc; proc++) {  \
        MPI_Status cll_status; \
        /* receive & print data from processor proc */ \
        MPI_Recv((experiment).instance, cll_numtests[proc]*sizeof((experiment).instance[0]), MPI_BYTE, proc, proc, MPI_COMM_WORLD, &cll_status); \
        cll_print_experiment_samples((experiment).numidents, (experiment).instance, cll_numtests[proc], proc, numproc); \
        MPI_Barrier(MPI_COMM_WORLD); /* barrier */ \
      } \
      end_of_json();\
      cll_print_end_of_experiment(experiment);\
    }\
    else { \
      cll_print_mpi_nodes(myid, numproc); \
      for(proc = 1; proc < numproc; proc++) {  \
        /* send data from processor proc to 0*/ \
        if (myid == proc) \
          MPI_Send((experiment).instance, (experiment).numtests*sizeof((experiment).instance[0]), MPI_BYTE, 0, proc, MPI_COMM_WORLD);\
        MPI_Barrier(MPI_COMM_WORLD); /* barrier */ \
      } \
    }\
}


#ifdef __cplusplus
}
#endif


#endif /* __CLL_MPI_H__ */
