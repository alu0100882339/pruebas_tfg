/*************************************************/
/* Include here any code, that, depending        */
/* your driver, you need to be executed          */
/* after the declaration of the current          */
/* experiment in file.cll.h is performed         */
/* See in file codegen.c the function:           */
/* generate_experiment_header(Symbolptr sp)      */ 
/* sp : pointer to the current experiment        */
/* sp->name : the name of the current experiment */
/* sp->par : the parallel driver                 */
/*************************************************/

/* Code required for OpenMP.                     */
/* just a dirty botch for OMP.                   */

if (strcmp(sp->par,"OMP") == 0) {
	cout(NULL, CURRENT_TARGET.CLLH,"\n#pragma omp threadprivate(%s)\n\n", sp->name);
}

