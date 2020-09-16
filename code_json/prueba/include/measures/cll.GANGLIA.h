/* cll.GANGLIA.h by diego (2006-01-21)
 *
 * A GANGLIA observable driver for CALL.
 *
 * IMPORTANT: 
 * - The GANGLIA observable driver only work on linux
 *   systems.
 * - The "uses" CALL pragma of the GANGLIA Observable 
 *   driver MUST BE AFTER the MPI "parallel" CALL pragma.
 * - Any CALL GANGLIA experiment declaration MUST BE 
 *   AFTER the MPI_Init() function.
 * 
 * TO DO LIST:
 * - Allow to ask to GANGLIA servers other than 
 *   'localhost'.
 *
 * USE:
 * - It is highly recommended to always use the GANGLIA 
 *   observable driver as the only observable of a CALL 
 *   experiment. This CALL experiment must be declared 
 *   just after the MPI_Init() funtion and the "end" CALL
 *   pragma just before the MPI_Finalize() funtion. For 
 *   example:
 *    __________________________________________________
 *   |(...)                                             |
 *   |                                                  |
 *   | MPI_Init(argc, argv);                            |
 *   | #pragma cll exp_name GANGLIA_MPI=exp_name[0]     |
 *   |                                                  |
 *   |(...)                                             |
 *   |                                                  |
 *   | #pragma cll end exp_name                         |
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
 * - The GANGLIA driver ask the GANGLIA server 
 *   information using the telnet protocol on 8649 port.
 *   This information is saved in a file. The location
 *   of the Ganglia server may be specified in the 
 *   'cll_ganglia_map' file that contains just one line
 *   "server Name_of_Ganglia_Server". If this file is
 *   not present, the driver check if localhost is the
 *   Ganglia Server.
 *
 * KNOWN PROBLEMS:
 * - If there is a CALL GANGLIA experiment and the 
 *   "cll report all" pragma is used, the CALL GANGLIA 
 *   experiment will return an empty experiment.
 *   
 */

#ifndef __CLL_GANGLIA_H
#define __CLL_GANGLIA_H  /* Avoid nested includes */

#include <mpi.h>
/* Sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define CLL_GANGLIA_MAXCHARSIZE 100

#define CLL_GANCLIA_PORT 8649
#define CLL_GANGLIA_BUFFSIZE 2048

#define cll_telnet_socket(outputFile,serverName) { \
  FILE *cgo; \
  char of[CLL_GANGLIA_MAXCHARSIZE]; \
  int fd, nb; \
  char buff[CLL_GANGLIA_BUFFSIZE+1]; \
  struct hostent *he; \
  struct sockaddr_in server; \
  \
  if ((he=gethostbyname(serverName))==NULL){ \
    fprintf(stderr,"[CLL_GANGLIA]: gethostbyname(%s) error\n",serverName); \
    return(1); \
  } \
  if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1){ \
    fprintf(stderr,"[CLL_GANGLIA]: socket() error\n"); \
    return(1); \
  } \
  server.sin_family = AF_INET; \
  server.sin_port = htons(CLL_GANCLIA_PORT); \
  server.sin_addr = *((struct in_addr *)he->h_addr); \
  bzero(&(server.sin_zero),8); \
  \
  if(connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr))==-1){ \
    fprintf(stderr,"[CLL_GANGLIA]: connect() error\n"); \
    return(1); \
  } \
  sprintf(of,"%s.ganglia.xml",outputFile); \
  cgo=fopen(of,"w"); \
  if (cgo==NULL) { \
    fprintf(stderr,"[CLL_GANGLIA]: fopen(%s,\"w\") error \n",of); \
    return(1); \
  }  \
  do { \
    if ((nb=recv(fd,buff,CLL_GANGLIA_BUFFSIZE,0)) == -1){ \
      fprintf(stderr,"[CLL_GANGLIA]: recv() \n"); \
      return(1); \
    } \
    buff[nb]='\0'; \
    fprintf(cgo,"%s",buff); \
  } while (nb>0); \
  fclose(cgo); \
  close(fd); \
}
  

#define cll_ganglia_get(exp) { \
  FILE *cll_ganglia_f; \
  int cll_gan_myid; \
  char cll_ganglia_command[CLL_GANGLIA_MAXCHARSIZE]; \
  char cll_ganglia_serverName[CLL_GANGLIA_MAXCHARSIZE]; \
  \
  MPI_Comm_rank(MPI_COMM_WORLD,&cll_gan_myid); \
  if(cll_gan_myid==0) { \
    \
    cll_ganglia_f=fopen("cll_ganglia_map","r"); \
    if (cll_ganglia_f == NULL) { \
      fprintf(stdout,"Trying localhost as Ganglia Server\n"); \
      sprintf(cll_ganglia_serverName,"localhost"); \
    } \
    else { \
      fscanf(cll_ganglia_f,"server %s \n",cll_ganglia_serverName); \
    } \
    fclose(cll_ganglia_f); \
    (cll_output_name) = cll_foutname((cll_output_name)); \
    cll_telnet_socket(cll_output_name,cll_ganglia_serverName); \
  } \
}

 
/* GANGLIA DEFINITIONS */

#define GANGLIA_INITIALIZATION()
 
#define GANGLIA_MPI_INIT(exp)

#define GANGLIA_MPI_DECL

#define GANGLIA_MPI_TMPS 

#define GANGLIA_MPI_START_OBS(exp)

#define GANGLIA_MPI_STOP_OBS(exp)  { \
  cll_ganglia_get(exp); \
}

#define GANGLIA_MPI_SAVE_INSTANCE(exp,inst)

#define GANGLIA_MPI_PRINT_TITLE(exp)

#define GANGLIA_MPI_PRINT_VALUE(exp,instance)

#define GANGLIA_MPI_FINALIZE(exp)
 
#endif
