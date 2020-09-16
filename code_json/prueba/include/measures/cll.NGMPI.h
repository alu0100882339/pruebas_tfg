/* cll.NG-MPI.h by diego (2008-02-22)
 *
 * An NWS observable driver for CALL.
 *
 * IMPORTANT:
 * - The NGMPI driver needs a MPI enviroment. Therefore,
 *   "uses" CALL pragma of NGMPI driver MUST BE AFTER the 
 *   MPI "parallel" CALL pragma.
 * - Any CALL NGMPI experiment declaration MUST BE AFTER 
 *   the MPI_Init() function.
 * 
 * USE:
 * - It is highly recommended to always use the NGMPI driver 
 *   as the only observable of a CALL experiment. This CALL 
 *   experiment must be declared after the MPI_Init() funtion 
 *   and the "end" CALL pragma just before the MPI_Finalize() 
 *   funtion. For example:
 *   __________________________________________________
 *   |(...)                                            |
 *   | MPI_Init(argc, argv);                           |
 *   |                                                 |
 *   | #pragma cll exp_name NGMPI=exp_name[0]          |
 *   |                                                 |
 *   |(...)                                            |
 *   |                                                 |
 *   | #pragma cll end exp_name                        |
 *   | MPI_Finalize();                                 |
 *   |                                                 |
 *   |(...)                                            |
 *   |_________________________________________________|
 *   NOTE: a monospace font is needed to a correct view
 *   of the example.
 * 
 * DESCRIPTION:
 * - The NGMPI driver measure the necessary data to obtain
 *   the LoOgGP parameters of the communication network in an
 *   MPI execution, using the NetGauge methodology [1].
 *   [1] http://www.unixer.de/research/netgauge/
 * - The measured data is stored in an XML files.
 * - The configuration parameters of the NGMPI driver can be
 *   tuned through the 'cll_ngmpi_map' file. This file MUST HAVE
 *   the following structure:
 *    ______________________________
 *   |                              |
 *   |CLL_NGMPI_SIZE_MAX 100000     |
 *   |CLL_NGMPI_N_PARAMETER 16      |
 *   |CLL_NGMPI_NUM_PROC 6          |
 *   |CLL_NGMPI_MODE 1              |
 *   |CLL_NGMPI_SUBSIZE 3           |
 *   |CLL_NGMPI_SUBNODES 1 3 5      |
 *   |CLL_NGMPI_TIMES 1             |
 *   |CLL_NGMPI_NUM_STEPS 2         |
 *   |CLL_NGMPI_STEP 1024 10000     |
 *   |CLL_NGMPI_STEP 10240 100000   |
 *   |______________________________|
 *   NOTE: a monospace font is needed to a correct view
 *   of the example.
 *
 * - CLL_NGMPI_SIZE_MAX: the maximun size message.
 * - CLL_NGMPI_N_PARAMETER: the 'n' parameter of NetGauge
 *   metodology.
 * - CLL_NGMPI_NUM_PROC: the number of monitored processor. It
 *   must be greater than 2 and smaller than the MPI size.
 * - CLL_NGMPI_MODE: 
 *   If 0, the driver randomly selects a pair for each message 
 *   size. 
 *   If 1, the driver performs the mode 0 but using the nodes
 *   specified in CLL_NGMPI_SUBNODES. The size of this subgroup
 *   is set by CLL_NGMPI_SUBSIZE. This options are only
 *   significant for mode 1. If other mode is used, 
 *   CLL_NGMPI_NUM_SUBSIZE and CLL_NGMPI_SUBNODES must be both 0.
 *   If 2, the number of processor are  grouped in pairs: 
 *   being P is the number of processors, floor(P/2) pairs. 
 *   If 2, all posible pairs between the processor are 
 *   considered: (P*P-P)/2 pairs.
 * - CLL_NGMPI_TIMES: repetitions of each pair measurement 
 *   proccess.
 * - CLL_NGMPI_NUM_STEPS: how many step different sections.
 * - CLL_NGMPI_STEP: the message size step and the maximum 
 *   message size this message size step is valid.
 *
 * KNOWN PROBLEMS:
 * - If there is a CALL NGMPI experiment and the "cll report all"
 *   pragma is used, the CALL NGMPI experiment will return an 
 *   empty experiment.
 *   
 */

#ifndef __CLL_NGMPI_H
#define __CLL_NGMPI_H  /* Avoid nested includes */

#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
/* XML */
#include <libxml/xmlmemory.h>

/* DEFINITIONS */
#define DEBUG 1
#define CLL_NGMPI_MAXCHARSIZE 40
#define CLL_NG_N 16
#define DEFAULT_CLL_MASTER 0
#define DEFAULT_CLL_NGMPI_SIZE_MAX 100000
#define DEFAULT_CLL_NGMPI_MODE 0
#define DEFAULT_CLL_NGMPI_TIMES 1
#define MINIMUM_CLL_NGMPI_TIMES 5

/* Wait Function: d parameter */
#define cll_ngmpi_wait(d) { \
  double t; \
  \
  t=MPI_Wtime()+d/1e6; \
  do {} while (MPI_Wtime() < t); \
} 


/* Base experiment */
#define cll_ngmpi_exp(s,n,d,buff,srv,cln,myrank,prtt,pong) { \
  int i; \
  double mpiTime0, mpiTime1, mpiTime2; \
  MPI_Status status; \
  \
  if (myrank==srv) { \
  /* Server behavior */ \
    for(i=0; i<(n-1); i++) { \
      MPI_Recv(buff, s, MPI_BYTE, cln, 0, MPI_COMM_WORLD, &status); \
      if (pong!=0) MPI_Send(buff, s, MPI_BYTE, cln, 0, MPI_COMM_WORLD); \
    } \
    MPI_Recv(buff, s, MPI_BYTE, cln, 0, MPI_COMM_WORLD, &status); \
    MPI_Send(buff, s, MPI_BYTE, cln, 0, MPI_COMM_WORLD); \
  } \
  else if (myrank==cln) { \
  /* Client behavior */ \
    mpiTime0=MPI_Wtime(); \
    MPI_Wtime(); \
    mpiTime1=MPI_Wtime(); \
    MPI_Send(buff, s, MPI_BYTE, srv, 0, MPI_COMM_WORLD); \
    for(i=0; i<(n-1); i++) { \
      cll_ngmpi_wait(d); /* WAIT!! */ \
      if (pong!=0) MPI_Recv(buff, s, MPI_BYTE, srv, 0, MPI_COMM_WORLD, &status); \
      MPI_Send(buff, s, MPI_BYTE, srv, 0, MPI_COMM_WORLD); \
    } \
    MPI_Recv(buff, s, MPI_BYTE, srv, 0, MPI_COMM_WORLD, &status); \
    mpiTime2=MPI_Wtime(); \
    /* Elapsed time (usec) */ \
    *prtt=1e6*((mpiTime2-mpiTime1)-(mpiTime1-mpiTime0)); \
  } \
}

/* Metrics of NGMPI experiments */
#define cll_xml_ngmpi_headers(node) { \
  xmlNewChild(node,NULL,BAD_CAST "h", BAD_CAST "size"); \
  xmlNewChild(node,NULL,BAD_CAST "h", BAD_CAST "n"); \
  xmlNewChild(node,NULL,BAD_CAST "h", BAD_CAST "PRTT(1,0,s)"); \
  xmlNewChild(node,NULL,BAD_CAST "h", BAD_CAST "PRTT(n,0,s)"); \
  xmlNewChild(node,NULL,BAD_CAST "h", BAD_CAST "PRTT(n,PRTT(1,0,s),s)"); \
  xmlNewChild(node,NULL,BAD_CAST "h", BAD_CAST "PP(1,s)"); \
  xmlNewChild(node,NULL,BAD_CAST "h", BAD_CAST "delay"); \
}


/* Point to point experiment */
#define cll_ngmpi_pair( \
     cll_server \
    ,cll_client \
    ,cll_master \
    ,mpirank \
    ,cll_n \
    ,cll_s \
    ,cll_ngmpi_buff \
    ,node \
) { \
  double prtt; \
  MPI_Comm world, workers; \
  MPI_Group world_group, worker_group; \
  char cll_ngmpi_data[CLL_NGMPI_MAXCHARSIZE]; \
  int ranks[2]; \
  double results[5]; \
  int cll_i, cll_d; \
  MPI_Status status; \
  xmlNodePtr node1=NULL; \
  double time0, time1, time2; \
  \
  /* Warm Up !! */ \
  if (mpirank==cll_server | mpirank==cll_client) { \
    for(cll_i=0; cll_i<128; cll_i++) { \
      cll_ngmpi_exp(1,cll_n,0,cll_ngmpi_buff,cll_server,cll_client,mpirank,&prtt,0); \
    } \
  }\
  world=MPI_COMM_WORLD; \
  MPI_Comm_group(world,&world_group); \
  ranks[0]=cll_server; ranks[1]=cll_client; \
  MPI_Group_incl(world_group,2,ranks,&worker_group); \
  MPI_Comm_create(world,worker_group,&workers); \
  \
  /* fprintf(stdout,"[%d]: Starting experiment\n",mpirank); */ \
  \
  if (mpirank==cll_server | mpirank==cll_client) { \
    /* Do experiments and send results to master */ \
    /* First Ping-Pong: Initialization only */ \
    cll_ngmpi_exp(cll_s,1,0,cll_ngmpi_buff,cll_server,cll_client,mpirank,&prtt,0); \
    if (mpirank==cll_client) cll_ngmpi_wait(prtt); /* Avoit network saturation */ \
    /* Second Ping-Pong: the real one */ \
    cll_ngmpi_exp(cll_s,1,0,cll_ngmpi_buff,cll_server,cll_client,mpirank,&prtt,0); \
    results[0]=prtt; \
    cll_d=2*prtt; \
    if (mpirank==cll_client) cll_ngmpi_wait(prtt); /* Avoit network saturation */ \
    /* First Ping-Ping: without delays */ \
    cll_ngmpi_exp(cll_s,cll_n,0,cll_ngmpi_buff,cll_server,cll_client,mpirank,&prtt,0); \
    results[1]=prtt; \
    if (mpirank==cll_client) cll_ngmpi_wait(prtt); /* Avoit network saturation */ \
    /* Second Ping-Ping: with delays */ \
    cll_ngmpi_exp(cll_s,cll_n,cll_d,cll_ngmpi_buff,cll_server,cll_client,mpirank,&prtt,0); \
    results[2]=prtt; \
    if (mpirank==cll_client) cll_ngmpi_wait(prtt); /* Avoit network saturation */ \
    /* Last Ping-Pong: latency measurement */ \
    cll_ngmpi_exp(1,cll_n,0,&cll_ngmpi_buff[0],cll_server,cll_client,mpirank,&prtt,1); \
    results[3]=prtt; \
    if (mpirank==cll_client) { \
      time0=MPI_Wtime(); \
      MPI_Wtime(); \
      time1=MPI_Wtime(); \
      cll_ngmpi_wait(cll_d); \
      time2=MPI_Wtime(); \
      results[4]=1e6*((time2-time1)-(time1-time0)); \
    } \
    if (mpirank==cll_client && cll_master!=cll_client) { \
      MPI_Send(results,5,MPI_DOUBLE,cll_master,cll_client,MPI_COMM_WORLD); \
    } \
    /* if (mpirank==cll_client) fprintf(stdout,"Client & server finished\n"); fflush(stdout); */ \
  } \
  if (mpirank==cll_master) { \
    if (cll_client!=cll_master) { \
      MPI_Recv(results,5,MPI_DOUBLE,cll_client,cll_client,MPI_COMM_WORLD,&status); \
    } \
    /* fprintf(stdout,"Registering experiment data\n"); fflush(stdout); */ \
    /* XML row */ \
    node1=xmlNewChild(node,NULL,BAD_CAST "row", NULL); \
    sprintf(cll_ngmpi_data,"%d",cll_s); \
    xmlNewChild(node1,NULL,BAD_CAST "c", BAD_CAST cll_ngmpi_data); \
    sprintf(cll_ngmpi_data,"%d",cll_n); \
    xmlNewChild(node1,NULL,BAD_CAST "c", BAD_CAST cll_ngmpi_data); \
    /* PRTT(1,0,s) */ \
    sprintf(cll_ngmpi_data,"%.4lf",results[0]); \
    xmlNewChild(node1,NULL,BAD_CAST "c", BAD_CAST cll_ngmpi_data); \
    /* PRTT(n,0,s) */ \
    sprintf(cll_ngmpi_data,"%.4lf",results[1]); \
    xmlNewChild(node1,NULL,BAD_CAST "c", BAD_CAST cll_ngmpi_data); \
    /* PRTT(n,d,s) */ \
    sprintf(cll_ngmpi_data,"%.4lf",results[2]); \
    xmlNewChild(node1,NULL,BAD_CAST "c", BAD_CAST cll_ngmpi_data); \
    /* PP(n,s) */ \
    sprintf(cll_ngmpi_data,"%.4lf",results[3]); \
    xmlNewChild(node1,NULL,BAD_CAST "c", BAD_CAST cll_ngmpi_data); \
    /* PP(n,s) */ \
    sprintf(cll_ngmpi_data,"%.4lf",results[4]); \
    xmlNewChild(node1,NULL,BAD_CAST "c", BAD_CAST cll_ngmpi_data); \
  } \
  MPI_Group_free(&worker_group); \
  MPI_Group_free(&world_group); \
}


#define cll_ngmpi_init(exp) { \
  FILE *cll_ngmpi_f; \
  int cll_i, cll_offset; \
  int mpirank, mpisize; \
  int cll_size_max, cll_num, cll_times, cll_mode, cll_index; \
  int cll_n, cll_s, cll_p, cll_q, cll_pairs, cll_local_times; \
  int *cll_index_step, *cll_step; \
  char *cll_ngmpi_buff; \
  char cll_ngmpi_name[CLL_NGMPI_MAXCHARSIZE]; \
  char cll_ngmpi_exp[CLL_NGMPI_MAXCHARSIZE]; \
  int cll_server, cll_client, cll_master=DEFAULT_CLL_MASTER; \
  int *subnodes, sizeNodes; \
  /* XML output */ \
  xmlDocPtr doc = NULL; \
  xmlNodePtr root_node = NULL, node_exp = NULL, node = NULL; \
  xmlDtdPtr dtd = NULL; \
  \
  /* Set MPI parameters */ \
  MPI_Comm_size(MPI_COMM_WORLD,&mpisize); \
  MPI_Comm_rank(MPI_COMM_WORLD,&mpirank); \
  fprintf(stderr,"[NG_MPI] mpirank = %d\n", mpirank);\
  fprintf(stderr,"[NG MPI] mpisize = %d\n", mpisize);\
  if(mpirank==cll_master) fprintf(stdout,"Starting: LogGP experiment\n"); \
  \
  /* Get NGMPI configuration */ \
  cll_ngmpi_f=fopen("cll_ngmpi_map","r"); \
  if (cll_ngmpi_f == NULL) { \
    if(mpirank==cll_master) fprintf(stdout,"Using default values for NG parameters\n"); \
    cll_size_max=DEFAULT_CLL_NGMPI_SIZE_MAX; \
    cll_n=CLL_NG_N; \
    cll_num=mpisize; \
    cll_mode=DEFAULT_CLL_NGMPI_MODE; \
    cll_times=DEFAULT_CLL_NGMPI_TIMES; \
    cll_index_step=(int *) malloc (2*sizeof(int)); \
    cll_step=(int *) malloc (2*sizeof(int)); \
    cll_index_step[0]=10000; cll_index_step[1]=100000; \
    cll_step[0]=1024; cll_step[1]=2560; \
  } \
  else { \
    fscanf(cll_ngmpi_f,"CLL_NGMPI_SIZE_MAX %d\n", &cll_size_max); \
    fscanf(cll_ngmpi_f,"CLL_NGMPI_N_PARAMETER %d\n", &cll_n); \
    fscanf(cll_ngmpi_f,"CLL_NGMPI_NUM_PROC %d\n", &cll_num); \
    if (cll_num<2 | cll_num>mpisize) { \
      if (mpisize<2) { \
        if (mpirank==cll_master) \
          fprintf(stderr,"NGMPI ERROR: the minimum size is two processors. Aborting execution.\n"); \
        MPI_Finalize(); \
        exit(EXIT_FAILURE); \
      } \
      if (cll_num>mpisize & cll_num>=2) { \
        cll_num=mpisize; \
        if (mpirank==cll_master) fprintf(stderr,"The number of proccesors is too high. Using MPI size.\n"); \
      } \
      else if (cll_num<2 & mpisize>1) { \
        cll_num=2; \
        if (mpirank==cll_master) fprintf(stderr,"The number of proccesors is too small. Using 2 processors.\n"); \
      } \
    } \
    fscanf(cll_ngmpi_f,"CLL_NGMPI_MODE %d\n", &cll_mode); \
    fscanf(cll_ngmpi_f,"CLL_NGMPI_SUBSIZE %d\n", &sizeNodes); \
    if (sizeNodes<2) sizeNodes=1; \
    subnodes=(int *) malloc (sizeNodes*sizeof(int)); \
    fscanf(cll_ngmpi_f,"CLL_NGMPI_SUBNODES %d",&subnodes[0]); \
    cll_offset=0; \
    for (cll_i=1;cll_i<sizeNodes;cll_i++) { \
      fscanf(cll_ngmpi_f," %d", &subnodes[cll_i-cll_offset]); \
      if(subnodes[cll_i-cll_offset]>=mpisize) cll_offset++; \
    } \
    sizeNodes=sizeNodes-cll_offset; \
    if (sizeNodes<2 && cll_mode==1) { \
      fprintf(stdout,"Subgroup size too small. Using mode 0 (RANDOM).\n"); \
      cll_mode=0; \
    } \
    fscanf(cll_ngmpi_f,"\n"); \
    if (mpirank==cll_master && sizeNodes>=2 && cll_mode==1) { \
      fprintf(stdout,"Using the following nodes: "); \
      for (cll_i=0;cll_i<sizeNodes;cll_i++) fprintf(stdout," %d",subnodes[cll_i]); \
      fprintf(stdout,"\n"); \
    } \
    fflush(stdout); fflush(stderr); \
    fscanf(cll_ngmpi_f,"CLL_NGMPI_TIMES %d\n", &cll_times); \
    if (cll_times<MINIMUM_CLL_NGMPI_TIMES) { \
      cll_times=MINIMUM_CLL_NGMPI_TIMES; \
      if (mpirank==cll_master) fprinft(stdout,"Times is too small. Using default\n"); \
    } \
    if (mpirank==cll_master) fprintf(stdout,"Times : %d\n",cll_times); \
    fscanf(cll_ngmpi_f,"CLL_NGMPI_NUM_STEPS %d\n", &cll_i); \
    cll_index_step=(int *) malloc (cll_i*sizeof(int)); \
    cll_step=(int *) malloc (cll_i*sizeof(int)); \
    cll_i=0; \
    while (EOF != fscanf(cll_ngmpi_f,"CLL_NGMPI_STEP %d %d\n",&cll_step[cll_i],&cll_index_step[cll_i])) cll_i++; \
    fclose(cll_ngmpi_f); \
  } \
  \
  if (cll_num<2) { \
    if (mpirank==cll_master) \
      fprintf(stderr,"NGMPI ERROR: number of proccesors must be greater than 1.\n"); \
    MPI_Finalize(); \
    exit(EXIT_FAILURE); \
  } \
  fflush(stdout); \
  fflush(stderr); \
  MPI_Barrier(MPI_COMM_WORLD); \
  \
  /* Initializing data buffer */ \
  cll_ngmpi_buff=(char *) malloc ((cll_size_max+1)*sizeof(char)); \
  for(cll_i=0; cll_i<cll_size_max; cll_i++) { \
    cll_ngmpi_buff[cll_i] = cll_i & 0xff; \
  } \
  \
  /* Initializing XML structure */ \
  if (mpirank==cll_master) { \
    doc = xmlNewDoc(BAD_CAST "1.0"); \
    root_node = xmlNewNode(NULL, BAD_CAST "cll_data"); \
    xmlDocSetRootElement(doc, root_node); \
    /* xml DTD declaration */ \
    dtd = xmlCreateIntSubset(doc, BAD_CAST CLL_DATA_DTD, NULL, NULL); \
    cll_xml_clldata_attributes(root_node); \
  } \
  \
  if (mpirank==cll_master) fprintf(stdout,"GO!\n"); \
  \
  /* MODE 0: SAMPLING */ \
  if (cll_mode==0) { \
    if (mpirank==cll_master) {fprintf(stdout,"Using SAMPLING mode\n");} \
    MPI_Barrier(MPI_COMM_WORLD); \
    \
    /* Pair experiment */ \
    if (mpirank==cll_master) { \
      /* XML experiment */ \
      node_exp=xmlNewChild(root_node, NULL, BAD_CAST "cll_experiment", NULL); \
      sprintf(cll_ngmpi_exp,"sampling",cll_server,cll_client); \
      xmlNewProp(node_exp, BAD_CAST "EXPERIMENT", BAD_CAST cll_ngmpi_exp); \
      \
      /* XML headers */ \
      node=xmlNewChild(node_exp, NULL, BAD_CAST "headers", NULL); \
      cll_xml_ngmpi_headers(node); \
      \
      /* XML sample */ \
      node=xmlNewChild(node_exp, NULL, BAD_CAST "sample", NULL); \
    } \
    \
    /* LOOP repetitions */ \
    /* cll_times=MINIMUM_CLL_NGMPI_TIMES; */ \
    /* for (cll_i=0; cll_i<cll_times; cll_i++) { */ \
      /* LOOP message size */ \
      cll_s=1; \
      cll_index=0; \
      while (cll_s<cll_size_max) { \
    for (cll_i=0; cll_i<cll_times; cll_i++) { \
        cll_server=(int)((1.*(double)rand()/(1.+(double)RAND_MAX))*(mpisize)); \
        cll_client=(int)((1.*(double)rand()/(1.+(double)RAND_MAX))*(mpisize)); \
        while (cll_client==cll_server) { \
          cll_client=(int)((1.*(double)rand()/(1.+(double)RAND_MAX))*(mpisize)); \
        } \
        /* if(mpirank==cll_master) fprintf(stdout,"[NGMPI DRIVER]: Using %d and %d in size %d\n",cll_server,cll_client,cll_s); fflush(stdout); */ \
        cll_ngmpi_pair(cll_server, cll_client, cll_master, mpirank, \
            cll_n, cll_s, cll_ngmpi_buff, node); \
        /* MPI_Barrier(MPI_COMM_WORLD); */ \
        if (mpirank==cll_client) cll_ngmpi_wait(10); \
    } \
        \
        cll_s+=cll_step[cll_index]; \
        if (cll_s>cll_index_step[cll_index]) cll_index++; \
      } \
    /* } */ \
  } \
  /* MODE 1: SUBGROUP */ \
  else if (cll_mode==1) { \
    if (mpirank==cll_master) {fprintf(stdout,"Using SUBGROUP mode\n");} \
    MPI_Barrier(MPI_COMM_WORLD); \
    \
    /* Pair experiment */ \
    if (mpirank==cll_master) { \
      /* XML experiment */ \
      node_exp=xmlNewChild(root_node, NULL, BAD_CAST "cll_experiment", NULL); \
      sprintf(cll_ngmpi_exp,"sampling-subgroup",cll_server,cll_client); \
      xmlNewProp(node_exp, BAD_CAST "EXPERIMENT", BAD_CAST cll_ngmpi_exp); \
      \
      /* XML headers */ \
      node=xmlNewChild(node_exp, NULL, BAD_CAST "headers", NULL); \
      cll_xml_ngmpi_headers(node); \
      \
      /* XML sample */ \
      node=xmlNewChild(node_exp, NULL, BAD_CAST "sample", NULL); \
    } \
    \
    /* LOOP repetitions */ \
    /* cll_times=MINIMUM_CLL_NGMPI_TIMES; */ \
    for (cll_i=0; cll_i<cll_times; cll_i++) { \
      /* LOOP message size */ \
      cll_s=1; \
      cll_index=0; \
      while (cll_s<cll_size_max) { \
        cll_server=(int)((1.*(double)rand()/(1.+(double)RAND_MAX))*(sizeNodes)); \
        cll_client=(int)((1.*(double)rand()/(1.+(double)RAND_MAX))*(sizeNodes)); \
        while (cll_client==cll_server) { \
          cll_client=(int)((1.*(double)rand()/(1.+(double)RAND_MAX))*(sizeNodes)); \
        } \
        cll_server=subnodes[cll_server]; \
        cll_client=subnodes[cll_client]; \
        if (mpirank==cll_master) fprintf(stdout,"Pair %d-%d\n",cll_server,cll_client); fflush(stdout); \
        cll_ngmpi_pair(cll_server, cll_client, cll_master, mpirank, \
            cll_n, cll_s, cll_ngmpi_buff, node); \
        MPI_Barrier(MPI_COMM_WORLD); \
        \
        cll_s+=cll_step[cll_index]; \
        if (cll_s>cll_index_step[cll_index]) cll_index++; \
      } \
    } \
  } \
  /* MODE 2: PAIRS */ \
  else if (cll_mode==2) { \
    if (mpirank==cll_master) {fprintf(stdout,"Using PAIR mode\n");} \
    MPI_Barrier(MPI_COMM_WORLD); \
    cll_pairs=cll_num/2; \
    /* cll_times correction */ \
    /* if (cll_pairs*cll_times<MINIMUM_CLL_NGMPI_TIMES) { */ \
    /*  cll_times=MINIMUM_CLL_NGMPI_TIMES/cll_pairs; */ \
    /*  if (MINIMUM_CLL_NGMPI_TIMES%cll_pairs>0) cll_times++; */ \
    /* }  */\
    for (cll_p=0; cll_p<cll_pairs; cll_p++) { \
      \
      MPI_Barrier(MPI_COMM_WORLD); \
      cll_ngmpi_wait(2e6); \
      \
      cll_server=2*cll_p; \
      cll_client=2*cll_p+1; \
      if (mpirank==cll_master) fprintf(stdout,"Pair %d-%d\n",cll_server,cll_client); fflush(stdout); \
      \
      /* Pair experiment */ \
      if (mpirank==cll_master) { \
        /* XML experiment */ \
        node_exp=xmlNewChild(root_node, NULL, BAD_CAST "cll_experiment", NULL); \
        sprintf(cll_ngmpi_exp,"nodes_%d_%d",cll_server,cll_client); \
        xmlNewProp(node_exp, BAD_CAST "EXPERIMENT", BAD_CAST cll_ngmpi_exp); \
        \
        /* XML headers */ \
        node=xmlNewChild(node_exp, NULL, BAD_CAST "headers", NULL); \
        cll_xml_ngmpi_headers(node); \
        \
        /* XML sample */ \
        node=xmlNewChild(node_exp, NULL, BAD_CAST "sample", NULL); \
      } \
      \
      /* LOOP repetitions */ \
      for (cll_i=0; cll_i<cll_times; cll_i++) { \
        /* LOOP message size */ \
        cll_s=1; \
        cll_index=0; \
        while (cll_s<cll_size_max) { \
          cll_ngmpi_pair(cll_server, cll_client, cll_master, mpirank, \
              cll_n, cll_s, cll_ngmpi_buff, node); \
          \
          cll_s+=cll_step[cll_index]; \
          if (cll_s>cll_index_step[cll_index]) cll_index++; \
        } \
      } \
    } \
  } \
  /* MODE 3: FULL */ \
  else if (cll_mode==3) { \
    if (mpirank==cll_master) {fprintf(stdout,"Using FULL mode\n");} \
    MPI_Barrier(MPI_COMM_WORLD); \
    cll_pairs=(cll_num*cll_num-cll_num)/2; \
    /* cll_times correction */ \
    /* if (cll_pairs*cll_times<MINIMUM_CLL_NGMPI_TIMES) { */ \
    /*  cll_times=MINIMUM_CLL_NGMPI_TIMES/cll_pairs; */ \
    /*  if (MINIMUM_CLL_NGMPI_TIMES%cll_pairs>0) cll_times++; */ \
    /* } */ \
    for (cll_p=0; cll_p<(cll_num-1); cll_p++) { \
      for (cll_q=cll_p+1; cll_q<cll_num; cll_q++) { \
        \
        MPI_Barrier(MPI_COMM_WORLD); \
        cll_ngmpi_wait(2e6); \
        \
        cll_server=cll_p; \
        cll_client=cll_q; \
        if (mpirank==cll_master) fprintf(stdout,"Pair %d-%d\n",cll_server,cll_client); fflush(stdout); \
        \
        /* Pair experiment */ \
        if (mpirank==cll_master) { \
          /* XML experiment */ \
          node_exp=xmlNewChild(root_node, NULL, BAD_CAST "cll_experiment", NULL); \
          sprintf(cll_ngmpi_exp,"nodes_%d_%d",cll_server,cll_client); \
          xmlNewProp(node_exp, BAD_CAST "EXPERIMENT", BAD_CAST cll_ngmpi_exp); \
          \
          /* XML headers */ \
          node=xmlNewChild(node_exp, NULL, BAD_CAST "headers", NULL); \
          cll_xml_ngmpi_headers(node); \
          \
          /* XML sample */ \
          node=xmlNewChild(node_exp, NULL, BAD_CAST "sample", NULL); \
        } \
        \
        /* LOOP repetitions */ \
        for (cll_i=0; cll_i<cll_times; cll_i++) { \
          /* LOOP message size */ \
          cll_s=1; \
          cll_index=0; \
          while (cll_s<cll_size_max) { \
            cll_ngmpi_pair(cll_server, cll_client, cll_master, mpirank, \
                cll_n, cll_s, cll_ngmpi_buff, node); \
            \
            cll_s+=cll_step[cll_index]; \
            if (cll_s>cll_index_step[cll_index]) cll_index++; \
          } \
        } \
      } \
    } \
  } \
  else { \
    if(mpirank==cll_master) { \
      fprintf(stderr,"NGMPI ERROR: Unknown MODE\nPlease, use:\n"); \
      fprintf(stderr,"\tNGMPI_MODE=0 (RANDOM)\n\tNGMPI_MODE=1 (SUBGROUP)\n"); \
      fprintf(stderr,"\tNGMPI_MODE=2 (PAIRS)\n\tNGMPI_MODE=3 (FULL)\n"); \
    } \
    MPI_Finalize(); \
    exit(EXIT_FAILURE); \
  } \
  \
  /* XML output */ \
  if (mpirank==cll_master) { \
  /* Dumping document to stdio or file */ \
    (cll_output_name) = cll_foutname((cll_output_name)); \
    sprintf(cll_ngmpi_name,"%s.ngmpi.xml",cll_output_name); \
    xmlSaveFormatFileEnc(cll_ngmpi_name, doc, "UTF-8", 1); \
  } \
  /*free the document */ \
  xmlFreeDoc(doc); \
  /* Free the global variables that may have been allocated by the parser */ \
  xmlCleanupParser(); \
  /* this is to debug memory for regression tests */ \
  xmlMemoryDump(); \
  \
  if (mpirank==cll_master) fprintf(stdout,"Finish: LogGP experiment\n"); \
}


/* NGMPI */

#define NGMPI_INITIALIZATION()

#define NGMPI_LOGGP_INIT(exp) 

#define NGMPI_LOGGP_DECL

#define NGMPI_LOGGP_TMPS 

#define NGMPI_LOGGP_START_OBS(exp) { \
  cll_ngmpi_init(exp); \
}

#define NGMPI_LOGGP_STOP_OBS(exp)

#define NGMPI_LOGGP_SAVE_INSTANCE(exp,inst)

#define NGMPI_LOGGP_PRINT_TITLE(exp)

#define NGMPI_LOGGP_PRINT_VALUE(exp,instance)

#define NGMPI_LOGGP_FINALIZE(exp) 

#endif
