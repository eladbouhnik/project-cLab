

#include <stdio.h>
#include "assemblerR.h"
#include "error.h"
#include "memoryListR.h"


int main(int argc, char *argv[]){
	/* scan the argv array and compile each file  */
	
FILE *p ,*p1;
    p=fopen("ps.obj","w");
    p1=fopen("ps.lbl","w");
	while (0 < --argc){
		int status = SUCC; /* flag to remember if "pass1" found errors in the code */
		lblword *headOfLbl=NULL; /* table for the labels */
		word *memhead = NULL; /* take the list of nodes representing the binary encoding of the operations (from "pass1") and pass it to "pass2" */
		
		printf("-------------       Compiling \"%s\"       -------------\n", argv[argc]);
		
		/* pre-compile the file and check for "fatal" errors */
		if ((status = preAsm(argv[argc])) == ERR_MEM || status == ERR_FOE || status == ERR_PRINT || status == ERR_FCE){
			return status;
		}
		
		/* perform the first pass on the pre-compiled file and check for fatal errors */
		if ((status = pass1R(argv[argc], &headOfLbl, &memhead, status)) == ERR_MEM || status == ERR_FOE || status == ERR_PRINT || status == ERR_FCE){
			return status;
		}
		
								printMemList(p,memhead,0,0);
								printLblList(p1,headOfLbl->next);

		/* perform the second pass on the pre-compiled file and check for fatal errors */
		/*if ((status = pass2(argv[argc], status, headOfLbl, memhead)) == ERR_MEM || status == ERR_FOE || status == ERR_PRINT || status == ERR_FCE){
			return status;
		}*/
		
		if (status == SUCC){
			printf("------------- Compiled \"%s\" successfully -------------\n\n", argv[argc]);
		} else {
			printf("-------------   Failed to compile \"%s\"   -------------\n\n", argv[argc]);
		}
	}

	return SUCC;
}


