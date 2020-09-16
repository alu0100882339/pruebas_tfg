/*************************************************/
/* Include here any code, that, depending        */
/* your driver, you need to be executed          */
/* after the report (file.cll.c).                */
/* See in file call.y the actions related with   */
/* the production rules:                         */
/*    call    : ...                              */
/*            | REPORT opt_inline reportlist     */
/*            | REPORT opt_inline ALL            i*/
/*************************************************/

/* Code required for OpenMP.                     */
/* another dirty botch for OMP.                  */

if (strcmp(parallel,"OMP") == 0) {
	cout(NULL, CURRENT_TARGET.CLLC,"  }\n");
}

