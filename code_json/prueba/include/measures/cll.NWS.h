/* cll.NWS.h by diego (2006-01-21)
 *
 * An NWS observable driver for CALL.
 *
 * IMPORTANT: 
 * - The NWS observable driver needs MPI code to work 
 *   properly.
 * - The "uses" CALL pragma of the NWS Observable driver 
 *   MUST BE AFTER the MPI "parallel" CALL pragma.
 * - Any CALL NWS experiment declaration MUST BE AFTER 
 *   the MPI_Init() function.
 *
 * USE:
 * - It is highly recommended to always use the NWS 
 *   observable driver as the only observable of a CALL 
 *   experiment. This CALL experiment must be declared 
 *   just after the MPI_Init() funtion and the "end" CALL
 *   pragma just before the MPI_Finalize() funtion. For 
 *   example:
 *    __________________________________________________
 *   |(...)                                             |
 *   |                                                  |
 *   | MPI_Init(argc, argv);                            |
 *   | #pragma cll nws_exp_name NWS_LBW=nws_exp_name[0] |
 *   |                                                  |
 *   |(...)                                             |
 *   |                                                  |
 *   | #pragma cll end nws_exp_name                     |
 *   | MPI_Finalize();                                  |
 *   |                                                  |
 *   |(...)                                             |
 *   |__________________________________________________|
 *
 *    NOTE: a monospace font is needed to a correct view 
 *          of the example.
 * 
 * DESCRIPTION:
 * 
 * - This driver only start and stop NWS activities. The 
 *   name server, the memory server and the needed sensors 
 *   must be running before the execution of the code. The 
 *   location of each component must be specified in the 
 *   'cll_nws_map' file. The format of this file must 
 *   match with the following example:
 *               _______________________________________
 *  line #1     | nameserver Name_of_nameserver         |
 *  line #2     | memoryserver Name_of_memoryserver     | 
 *  line #3     | sensors Number_of_sensors             |
 *  line #4     | sensor Name_of_sensor1                |
 *  line #5     | sensor Name_of_sensor2                |
 *  ...         | ...                                   |
 *  line #(N+3) | sensor Name_of_sensorN                |
 *  end of file |_______________________________________|
 *
 *    NOTE: a monospace font is needed to a correct view 
 *          of the example.
 *
 * - There is no any restriction on the number of 
 *   sensors but it must be greater than 2.
 *
 * - A TCP experiment with all the sensors is started. 
 *   Output file has XML format (cll_data.dtd).
 * 
 * - The obtained measurements refers to the latency and 
 *   bandwith between the Name_of_sensor1 and the other 
 *   sensors (the same idea as a broadcast). 
 *
 * KNOWN PROBLEMS:
 * - If there is a CALL NWS experiment and the 
 *   "cll report all" pragma is used, the CALL NWS 
 *   experiment will return an empty experiment.
 * - The GetMeasurements function uses 
 *   NWSAPI_BEGINNING_OF_TIME instead of (exp).TimeStamp1
 *   (when the NWS activity starts to monitorize the 
 *   network).
 *   
 */

#ifndef __CLL_NWS_H
#define __CLL_NWS_H  /* Avoid nested includes */

#include "nws_api.h"
#include "mpi.h"
/*#include <sys/time.h>*/
#include <time.h>
#include <stdlib.h>

/* xml */
#include <libxml/xmlmemory.h>

#define CLL_NWS_MAXCHARSIZE 40
#define CLL_NWS_PERIOD 10
#define CLL_NWS_SIZE 1

#define cll_xml_nws_headers(node) { \
  xmlNewChild(node,NULL,BAD_CAST "h", BAD_CAST "TimeStamp"); \
  xmlNewChild(node,NULL,BAD_CAST "h", BAD_CAST "Bandwidth"); \
  xmlNewChild(node,NULL,BAD_CAST "h", BAD_CAST "Latency"); \
}

#define cll_nws_init(exp) { \
  FILE *cll_nws_f; \
  char **cll_nws_activity_options; \
  const char **p2cll_nws_activity_options; \
  int cll_i; \
  \
  MPI_Comm_rank(MPI_COMM_WORLD,&(exp).cll_nws_myid); \
  if((exp).cll_nws_myid==0) { \
    cll_nws_f=fopen("cll_nws_map","r"); \
    fscanf(cll_nws_f,"nameserver %s \n",(exp).cll_nws_nameserver); \
    fscanf(cll_nws_f,"memoryserver %s \n",(exp).cll_nws_memoryserver); \
    fscanf(cll_nws_f,"sensors %d \n", &(exp).cll_nws_num_sensors); \
    (exp).cll_nws_sensor = (char **) malloc ((exp).cll_nws_num_sensors*sizeof(char *)); \
    for(cll_i=0;cll_i<(exp).cll_nws_num_sensors;cll_i++) { \
      (exp).cll_nws_sensor[cll_i] = (char *) malloc (CLL_NWS_MAXCHARSIZE*sizeof(char)); \
      fscanf(cll_nws_f,"sensor %s \n", (exp).cll_nws_sensor[cll_i]); \
    } \
    fclose(cll_nws_f); \
    \
    /* Set the NWS name server, the NWS memory server */ \
    NWSAPI_UseNameServer(NWSAPI_MakeHostSpec((const char*)(exp).cll_nws_nameserver, 8090)); \
    NWSAPI_UseMemory(NWSAPI_MakeHostSpec((const char*)(exp).cll_nws_memoryserver, 8050)); \
    /* Set the NWS sensor on which the NWS activity will be started */ \
    (exp).cll_nws_SENSOR = NWSAPI_MakeHostSpec((const char*)(exp).cll_nws_sensor[0], 8060); \
    \
    /* Options of the NWS activity */ \
    /* Options of the NWS activity: members of the NWS experiment */ \
    cll_nws_activity_options = (char **) malloc (((exp).cll_nws_num_sensors+2)*sizeof(char *)); \
    p2cll_nws_activity_options = (const char **) malloc (((exp).cll_nws_num_sensors+2)*sizeof(char *)); \
    for(cll_i=0; cll_i<(exp).cll_nws_num_sensors; cll_i++) { \
      cll_nws_activity_options[cll_i] = (char *) malloc (CLL_NWS_MAXCHARSIZE*sizeof(char)); \
      sprintf(cll_nws_activity_options[cll_i],"member:%s",(exp).cll_nws_sensor[cll_i]); \
      p2cll_nws_activity_options[cll_i]=&cll_nws_activity_options[cll_i][0]; \
    } \
    /* Options of the NWS activity: size of messages NWS experiment */ \
    cll_nws_activity_options[(exp).cll_nws_num_sensors] = (char *) malloc (CLL_NWS_MAXCHARSIZE*sizeof(char)); \
    sprintf(cll_nws_activity_options[(exp).cll_nws_num_sensors],"size:%d",CLL_NWS_SIZE); \
    p2cll_nws_activity_options[(exp).cll_nws_num_sensors]=&cll_nws_activity_options[(exp).cll_nws_num_sensors][0]; \
    \
    /* Options of the NWS activity: frecuency of messages NWS experiment */ \
    cll_nws_activity_options[(exp).cll_nws_num_sensors+1] = (char *) malloc (CLL_NWS_MAXCHARSIZE*sizeof(char)); \
    sprintf(cll_nws_activity_options[(exp).cll_nws_num_sensors+1],"period:%d",CLL_NWS_PERIOD); \
    p2cll_nws_activity_options[(exp).cll_nws_num_sensors+1]=&cll_nws_activity_options[(exp).cll_nws_num_sensors+1][0]; \
    \
    /* Beginning of the NWS activity */ \
    ((exp).TimeStamp1)=time( NULL ); \
    \
    /* Set the NWS activity name */ \
    (exp).cll_nws_activity = (char *) malloc (CLL_NWS_MAXCHARSIZE*sizeof(char)); \
    sprintf((exp).cll_nws_activity,"%s-%u",(exp).cll_nws_sensor[0],((exp).TimeStamp1)); \
    \
    /* Start NWS activity */ \
    /* for(cll_i=0; cll_i<((exp).cll_nws_num_sensors+2); cll_i++) */ \
    /*  fprintf(stderr,"[CALL]: NWS activity option: %s\n", p2cll_nws_activity_options[cll_i]); */ \
    if(NWSAPI_StartActivity((exp).cll_nws_SENSOR \
                          , (exp).cll_nws_activity \
                          , "clique" \
                          , "tcpMessageMonitor" \
                          , p2cll_nws_activity_options \
                          , ((exp).cll_nws_num_sensors+2) \
        )==0) { \
      fprintf(stderr,"[CALL ERROR]: unable to start Activity\n"); \
    } \
    else { \
      fprintf(stderr,"[CALL]: NWS activity was successfull started\n"); \
      sleep(CLL_NWS_PERIOD*3);\
    }\
  } \
}
 
 
#define cll_nws_stop(exp) { \
  FILE *cll_nws_f; \
  int cll_i, cll_j; \
  int cll_nws_num_measurements; \
  char cll_nws_filtro[4*CLL_NWS_MAXCHARSIZE]; \
  const char *cll_nws_seriesband, *cll_nws_serieslat; \
  NWSAPI_ObjectSet cll_nws_set_band, cll_nws_set_lat; \
  NWSAPI_Measurement *cll_nws_bandMeas, *cll_nws_latMeas; \
  unsigned int cll_nws_returned; \
  \
  /* XML output */ \
  xmlDocPtr doc = NULL; \
  xmlNodePtr root_node = NULL, node_exp = NULL, node = NULL, node1 = NULL; \
  xmlDtdPtr dtd = NULL; \
  char nwsdata[10]; \
  char cll_nws_Names[CLL_NWS_MAXCHARSIZE]; \
  \
  if((exp).cll_nws_myid==0) { \
    sleep(CLL_NWS_PERIOD*3);\
    /* Stop NWS activity */ \
    if(NWSAPI_HaltActivity((exp).cll_nws_SENSOR,(exp).cll_nws_activity)==0) { \
      fprintf(stderr,"[CALL ERROR]: unable to stop Activity\n"); \
    } \
    else { \
      fprintf(stderr,"[CALL]: NWS activity was successfull halted\n"); \
    } \
    ((exp).TimeStamp2)=time( NULL ); \
    cll_nws_num_measurements = (int)(1+(((exp).TimeStamp2)-((exp).TimeStamp1))/((int)CLL_NWS_PERIOD)); \
    \
    /* XML cll_data */ \
    doc = xmlNewDoc(BAD_CAST "1.0"); \
    root_node = xmlNewNode(NULL, BAD_CAST "cll_data"); \
    xmlDocSetRootElement(doc, root_node); \
    /* xml DTD declaration */ \
    dtd = xmlCreateIntSubset(doc, BAD_CAST CLL_DATA_DTD, NULL, NULL); \
    \
      cll_xml_clldata_attributes(root_node); \
    \
    /* Memory allocation for the returned measurments */ \
    cll_nws_bandMeas = (NWSAPI_Measurement *)malloc(cll_nws_num_measurements*sizeof(NWSAPI_Measurement) ); \
    cll_nws_latMeas = (NWSAPI_Measurement *)malloc(cll_nws_num_measurements*sizeof(NWSAPI_Measurement) ); \
    \
    /* For each member of the NWS activity... */ \
    for(cll_i=1;cll_i<(((exp).cll_nws_num_sensors));cll_i++) { \
      \
      /* XML experiment */ \
      node_exp=xmlNewChild(root_node, NULL, BAD_CAST "cll_experiment", NULL); \
        sprintf(cll_nws_Names,"%s-%s", ((exp).cll_nws_activity), ((exp).cll_nws_sensor[cll_i])); \
        xmlNewProp(node_exp, BAD_CAST "EXPERIMENT", BAD_CAST cll_nws_Names); \
      \
      /* XML machines */ \
      node=xmlNewChild(node_exp, NULL, BAD_CAST "machines", NULL); \
        xmlNewChild(node,NULL,BAD_CAST "node", BAD_CAST ((exp).cll_nws_sensor[0])); \
        xmlNewChild(node,NULL,BAD_CAST "node", BAD_CAST ((exp).cll_nws_sensor[cll_i])); \
      \
      /* XML headers */ \
      node=xmlNewChild(node_exp, NULL, BAD_CAST "headers", NULL); \
      cll_xml_nws_headers(node); \
      \
      /* Bandwidth: Searching the matching objects in NWS name server */ \
      sprintf(cll_nws_filtro,"(&(activity=%s)(resource=bandwidthTcp)(host=%s*)(target=%s*))", \
          (exp).cll_nws_activity,(exp).cll_nws_sensor[0],(exp).cll_nws_sensor[cll_i]); \
      if(NWSAPI_GetObjects(cll_nws_filtro, &cll_nws_set_band)==0) \
        fprintf(stderr,"ERROR: unable to get Objects: bandwidthTcp\n"); \
      \
      /* Latency: Searching the matching objects in NWS name server */ \
      sprintf(cll_nws_filtro,"(&(activity=%s)(resource=latencyTcp)(host=%s*)(target=%s*))", \
          (exp).cll_nws_activity,(exp).cll_nws_sensor[0],(exp).cll_nws_sensor[cll_i]); \
      /* fprintf(cll_nws_f,"%s\n",cll_nws_filtro); */ \
      if(NWSAPI_GetObjects(cll_nws_filtro, &cll_nws_set_lat)==0) \
        fprintf(stderr,"ERROR: unable to get Objects: LatencyTcp\n"); \
      \
      \
      /* Bandwidth: Getting the measurements in NWS memory server */ \
      cll_nws_seriesband=NWSAPI_NwsAttributeValue_r(NWSAPI_FirstNwsAttribute(NWSAPI_FirstObject(cll_nws_set_band))); \
      FreeObjectSet(&cll_nws_set_band); \
      NWSAPI_GetMeasurements( \
            cll_nws_seriesband \
          , NWSAPI_BEGINNING_OF_TIME /* ((double)((exp).TimeStamp1)) */  \
          , cll_nws_bandMeas \
          , cll_nws_num_measurements \
          , &cll_nws_returned \
          ); \
      \
      \
      /* Latency: Getting the measurements in NWS memory server */ \
      cll_nws_serieslat=NWSAPI_NwsAttributeValue_r(NWSAPI_FirstNwsAttribute(NWSAPI_FirstObject(cll_nws_set_lat))); \
      FreeObjectSet(&cll_nws_set_lat); \
      NWSAPI_GetMeasurements( \
            cll_nws_serieslat \
          , NWSAPI_BEGINNING_OF_TIME /* ((double)((exp).TimeStamp1)) */ \
          , cll_nws_latMeas \
          , cll_nws_num_measurements \
          , &cll_nws_returned \
          ); \
      \
      \
      /* Results: TimeStamp-Bandwidth-Latency */\
      /* XML sample */ \
      node=xmlNewChild(node_exp, NULL, BAD_CAST "sample", NULL); \
      for(cll_j=(cll_nws_returned-1);cll_j>=0;cll_j--) { \
        /* XML row */ \
        node1=xmlNewChild(node,NULL,BAD_CAST "row", NULL); \
        sprintf(nwsdata,"%.0lf",cll_nws_bandMeas[cll_j].timeStamp); \
        xmlNewChild(node1,NULL,BAD_CAST "c", BAD_CAST nwsdata); \
        sprintf(nwsdata,"%.5lf",cll_nws_bandMeas[cll_j].measurement); \
        xmlNewChild(node1,NULL,BAD_CAST "c", BAD_CAST nwsdata); \
        sprintf(nwsdata,"%.5lf",cll_nws_latMeas[cll_j].measurement); \
        xmlNewChild(node1,NULL,BAD_CAST "c", BAD_CAST nwsdata); \
      } \
      \
    } \
    free(cll_nws_bandMeas); \
    free(cll_nws_latMeas); \
    \
    /* xml output */ \
    /* Dumping document to stdio or file */ \
    (cll_output_name) = cll_foutname((cll_output_name)); \
    sprintf(cll_nws_Names,"%s.nws.xml",cll_output_name); \
    xmlSaveFormatFileEnc(cll_nws_Names, doc, "UTF-8", 1); \
    /*free the document */ \
    xmlFreeDoc(doc); \
    /* Free the global variables that may have been allocated by the parser */ \
    xmlCleanupParser(); \
    /* this is to debug memory for regression tests */ \
    xmlMemoryDump(); \
    \
  } \
}


/* NWS_LBW (NWS LATENCY & BANDWIDTH) DEFINITIONS */

#define NWS_INITIALIZATION()

#define NWS_LBW_INIT(exp) 

#define NWS_LBW_DECL

#define NWS_LBW_TMPS \
  time_t TimeStamp1, TimeStamp2; \
  int    cll_nws_myid; \
  char   cll_nws_nameserver[CLL_NWS_MAXCHARSIZE]; \
  char   cll_nws_memoryserver[CLL_NWS_MAXCHARSIZE]; \
  int    cll_nws_num_sensors; \
  char   *cll_nws_activity; \
  const  NWSAPI_HostSpec *cll_nws_SENSOR; \
  char   **cll_nws_sensor; \
  


#define NWS_LBW_START_OBS(exp) { \
  cll_nws_init(exp); \
}

#define NWS_LBW_STOP_OBS(exp) { \
  cll_nws_stop(exp); \
}

#define NWS_LBW_SAVE_INSTANCE(exp,inst)

#define NWS_LBW_PRINT_TITLE(exp)

#define NWS_LBW_PRINT_VALUE(exp,instance)

#define NWS_LBW_FINALIZE(exp) 

#endif
