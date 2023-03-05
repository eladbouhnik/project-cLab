
#include "error.h"
#include "preAsm.h"
#include "supfuncsR.h"
#include "macroTable.h"
#include "ioR.h"

 	int preAsm(char *path){/*need to change- to get more then on files in assembler.c*/
 		FILE *source , *target; /* the source (.as) and target (.am) files */
 		char *nameOfFile= path;
 		char *newString;
 		char *line;
 		int len; /* the length of the line */
		int ln = 0; /*line number*/
		char lc = '\n'; /* the last character in the line */
		char plc; /* the last character of the previous line */
		int status = SUCC; /* flag to remember if found error */
		
		newMacro *head = NULL; /*build the first node in the linked list*/
		head = (newMacro *) malloc(sizeof(newMacro));
		if(head==NULL){
		    report(ERR_MEM, ln);
	        return ERR_MEM;
		}
		head->name = "null";
		head->content = "null";
		
		if ((newString = addstr(nameOfFile, ".as")) == NULL){ /* adds the extension of the input file */
			report(ERR_MEM, ln);
			return ERR_MEM;
		}
 		if((source = fopen(newString,"r"))== NULL){ /* open the input file on read mode */
 			free(newString);
			report(ERR_FOE, ln);
			return ERR_FOE;
		}
		free(newString);
 	
 		/* adds the extension of the output file */
	if ((newString = addstr(nameOfFile, ".am")) == NULL){
		fclose(source);
		report(ERR_MEM, ln);
		return ERR_MEM;
	}
	
	if ((target = fopen(newString,"w")) == NULL){
		free(newString);
		fclose(source);
		report(ERR_FOE, ln);
		return ERR_FOE;
	}
	free(newString);
 	
 	while ((plc = lc) != EOF && (line = readline(source, &lc, &len)) != NULL){
		char *name; /* the name of the declared macro */
		int endflag = 0; /* flag to remember if "endmacro" was found */
		char *content = NULL; /* the content of a macro */
		ln++;

		/* check if the line is a comment */
		if (*line == SCO){
			free(line);
			continue;
		}
		
		/* check if the line is too long (more than MAXLEN) */
		if (MAXLEN < len){
			free(line);
			report(status = ERR_LINE_EXR, ln);
			continue;
		}
		
		/* check if it is not a macro declaration */
		if ((name = ismacrodec(head, line)) == NULL){
			/* check if there is a macro with the content of the line as its name */
			if ((content = getcontent(head, line)) != NULL){ 
				if(writeline(target, content) == ERR_PRINT){ /* couldn't print to the file */
					fclose(source);
					fclose(target);
					freeList(head);
					report(ERR_PRINT, ln);
					return ERR_PRINT;
				}
			} else { /* not a macro or a refference to a macro */
				if(writeline(target, line) == ERR_PRINT){/*couldn't print to the file*/
					fclose(source);
					fclose(target);
					freeList(head);
					report(ERR_PRINT, ln);
					return ERR_PRINT;
				}
			}
			
			free(line);
			continue;
		}
		
		
		/* according the assignment, it is guaranteed that there is a line of "endmacro" */
		/* reading lines and storing them until "endmacro" is found */
		while (!endflag){
			char *macroline = readline(source, &lc, &len);
			
			if (macroline == NULL){
				fclose(source);
				fclose(target);
				freeList(head);
				free(line);
				free(content);
				report(ERR_MEM, ln);
				return ERR_MEM;
			}
			
			/* check if the line is a comment */
			if (*macroline == SCO){
				free(macroline);
				continue;
			}
			
			/* check if the line is too long (more than MAXLEN) */
			if (MAXLEN < len){
				free(macroline);
				report(status = ERR_LINE_EXR, ln);
				continue;
			}
			
			/*	if this really is "endmacro", those are the only characters in the line because
				the white characters before and after it are deleted.							 */
			if (!strcmp(macroline, EMAC)){ /* strcmp return 0 only when the strings are equal */
				free(macroline);
				endflag = !endflag; /* found endmcr */
			} else {
				/* "addfree" is a macro from " preAsm.h" */
				/* adds the new line to the macro's content */
				if (content != NULL){
					addfree(content, "\n");
				}
				addfree(content, macroline);
				free(macroline);
			}
		}
		
		
		/* finished reading all the content of the new macro */
		
		/* adding the macro to the linked list */
		if (addMcr(head, name, content) == ERR_MEM){  /*check if there was a memory problem */
			fclose(source);
			fclose(target);
			freeList(head);
			report(ERR_MEM, ln);
			return ERR_MEM;
		}
			   
	}
	
	freeList(head);
	
	fclose(source);
	fclose(target);

	return status;
}
 	
 	
 










