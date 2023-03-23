/*
	the errors that are handeld by the functions and a function to 
	report the errors to the standard output.
*/

#ifndef _ERRORA
#define _ERROR
#define SUCC			0	/* no error */

/* fatal errors, need to stop the program */
#define ERR_MEM			1	/* there was a problem in memory allocation */
#define ERR_FOE			2	/* can't open file */
#define ERR_PRINT		3	/* couldn't print to a file */
#define ERR_FCE			4	/* couldn't close a file */



#define MEM_END		255	/* End of memory location */


#define LBLLENMAX 30 /* the max amount of chars label's name can have */

/* errors in the code provided by the user */
#define ERR_FULL_MEM		5       /* reached the end of the allowed memory (MEM_END) */
#define ERR_COM_IL		6	/* illegal comma (comma somewhere before the first operand) */
#define ERR_COM_MI		7	/* missing comma between operands */
#define ERR_COM_MU		8	/* multiple consecutive commas */
#define ERR_LINE_EXR		9       /* the line is too long */
#define ERR_LINE_OLBL		10	/* there is only label declaration in the line */
#define ERR_OPD			11	/* illegal operand */
#define ERR_OPT			12	/* unknown operation (not instruction or guide) */
#define ERR_ARGS_NUM		13	/* invalid number of operands */
#define ERR_ARGS_LBL		14	/* unknown label in operand */
#define ERR_ENT_LBL		15	/* external label can't be entry */
#define ERR_LMT_DAN		16	/* number too big or small for operand numbers */
#define ERR_LMT_GUN		17	/* number too big or small for guide numbers */
#define ERR_LBL			18	/* the label already exists */
#define ERR_LBL_DEC		19	/* invalid label name in declaration */
#define ERR_LBL_LEN		20	/* Label name in label declaration is too long */
#define ERR_EXR 		21      /* Extraneous text after end of command*/

/*	report an error according to the error code given to the function.
	print explanation about the error to the standard output with the
	number of the line (the line number won't be present in fatal errors).
	SUCC is not an error.													
*/

void report(int error, int line);

#endif

