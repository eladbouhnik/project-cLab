/*
	header for the prototypes of the pre-assembler, pass1 and pass2 functions.
	which are responsible for compiling an EAS file to EAM,EOB,ENT and EXT files.
*/


#include "memoryOpr.h"
#include "ioFuncs.h"



/*	take "nameOfFile" and read the file named "nameOfFile" with the extension EAS.
	write to the file named "nameOfFile" with the extension EAM the lines of the
	EAS file.
	any unnecessary spaces and tabs are removed, comment lines are removed,
	and all the macros are expended.
	
	the function does checks for:
		ERR_MEM
		ERR_FOE
		ERR_PRINT
		ERR_FCE
		ERR_LINE_EXR
	
	and return the code of the error in case of error, return
	SUCC otherwise.																*/
int preAsm(char *nameOfFile);


/*	take "nameOfFile" and read the file named "nameOfFile" with the extension EAM.
	create table of the labels found in the file.
	create a list of the operations' encoding and put a pointer to the head
	in "memhead".
	
	the function does checks for:
		ERR_MEM
		ERR_FOE
		ERR_FCE
		ERR_LINE_OLBL
		ERR_OPD
		ERR_ARGS_NUM
		ERR_LBL
		ERR_FULL_MEM
		ERR_LMT_GUN
		and the checks that "splitline" from "supfuncs.h" perform.
	
	and return the code of the error in case of error, return
	SUCC otherwise (the status parameter is there to change the default from SUCC).		*/
int pass1(char *nameOfFile, lblword **headOfLbl, word **memhead, int status);



/*	take "nameOfFile" and read the file named "nameOfFile" with the extension EAM.
	use the labels in headOfLbl to create the operands' encoding.
	add the operands' encoding to the "memhead" list in the right position
	based on the "nameOfFile" file, and print the encoded list to a file named
	"nameOfFile" with the extension of EOB.
	print to a file named "nameOfFile" with the extension ENT a list of labels
	that are declared as "entry" in the EAM file.
	print to a file named "nameOfFile" with the extension EXT a list of labels
	that are declared as "extern" in the EAM file.
	
	the function does checks for:
		ERR_MEM
		ERR_FOE
		ERR_PRINT
		ERR_FCE
		ERR_LINE_OLBL
		ERR_OPD
		ERR_ARGS_NUM
		ERR_ARGS_LBL
		ERR_ARGS_STC
		ERR_ENT_LBL
		ERR_LMT_DAN
		and the checks that "splitline" from "supfuncs.h" perform.
	
	if status is not SUCC or found an error the ENT, EXT and EOB files won't be created.
	
	and return the code of the error in case of error, return
	SUCC otherwise.																*/
int pass2(char *nameOfFile, int status, lblword *headOfLbl, word *head);
