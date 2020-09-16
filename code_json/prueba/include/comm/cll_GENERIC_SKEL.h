/*************************************************
 * This is a Generic Skeleton to help you write  *
 * new parallel communications drivers for CALL  *
 *                                               *
 * See the CALL Parallel Communications Drivers  *
 * Developers Guide for details.                 *
 *                                               *
 *    <NEW_DRIVER> should be substituted by the  *
 * name of your driver, giving the new driver    *
 * the name cll_<NEW_DRIVER>.h                   *
 *                                               *
 *************************************************/
#ifndef __CLL_<NEW_DRIVER>_H__
#define __CLL_<NEW_DRIVER>_H__

#include NEW_DRIVER_NEEDS_THIS_HEADER_FILES

#define CLL_SYNC		 /** PROVIDE IMPLEMENTATION **/
#define CLL_SYNC_(x) /** PROVIDE IMPLEMENTATION **/


/*************************************************
 * IF YOUR DRIVER USES FUNCTIONS OR MACROS THAT  *
 * ARE NOT PART OF THE CALL INTERFACE,  PRECEDE  *
 * THEIR NAMES WITH:                             *
 *                   CLL_<NEW_DRIVER>_           *
 * For example:                                  *
 *    CLL_<NEW_DRIVER>_get_NUMTESTS()            *
 *************************************************/

/*************************************************
 * WHAT FOLLOWS MUST BE A MACRO: DO NOT FORGET   *
 * TO END EACH LINE WITH A SLASH "\"!!!!!        *
 *************************************************/
#define cll_report(experiment)                     \
  {                                                \
   /********************************************** \
    * GATHER EVERY PROCESSOR NUMTESTS SOMEWHERE  * \
    * BEFORE YOU CALL cll_print_NUMTESTS()       * \
    **********************************************/\
                                                   \
   /********************************************** \
    * THINGS THAT THE I/O MASTER MUST ALWAYS DO: * \
    **********************************************/\
    cll_open_file();                               \
    cll_print_header((experiment),"<NEW_DRIVER>"); \
    cll_print_exp_header((experiment));            \
    cll_print_NUMTESTS(cll_numtests,numproc);      \
    cll_print_exp_fields_titles((experiment));     \
                                                   \
   /********************************************** \
    * NOW GATHER EVERY PROCESSOR SAMPLES DATA AND* \
    * PRINT IT ONE AT A TIME USING THE FOLLOWING * \
    * FUNCTION. ONLY THE I/O MASTER CAN CALL IT. * \
    *                                            * \
    * "experiment" refers to the whole experiment* \
    * structure of the processor whose data is   * \
    * being printed.                             * \
    **********************************************/\
    cll_print_experiment_samples(                  \
             (experiment).numidents,               \
             (experiment).instance,                \
             (experiment).numtests,                \
             process_id_of_the_current_data,       \
             numprocessors);                       \
                                                   \
   /********************************************** \
    * THINGS THAT THE I/O MASTER MUST ALWAYS DO: * \
    **********************************************/\
    cll_print_end_of_experiment(experiment);       \
}

#endif /* __CLL_<NEW_DRIVER>_H__ */
