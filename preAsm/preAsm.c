#include <stdio.h>
#include "error.h"
#include "preAsm.h"

 	int main(int argc,char  *argv[]){
 		FILE *source , *target; /* the source (.as) and target (.am) files */
 		char *nameOfFile= argv[1];
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
 	
 	
 char *addstr(char *str1, char *str2){
	char *newStr; /* the new string */
	char *fstr1 = (str1 == NULL ? ESTR : str1); /* if the string is NULL, replace it with ESTR */
	
	/* allocate space for the new string */
	newStr = (char *)malloc(  (strlen(fstr1) + strlen(str2) + 1) * sizeof(char)  );
	
	/* check the allocation */
	if (newStr != NULL){
		strcpy(newStr, fstr1);
		strcat(newStr, str2);
	}
	
	return newStr;
}

int addMcr(newMacro *head,char *newName,char *newContent){
     newMacro *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = (newMacro *) malloc(sizeof(newMacro)); /*need to check allocation*/
    if(current->next==NULL)
    	return ERR_MEM;
    current->next->name = newName;
    current->next->content = newContent;
    current->next->next = NULL;
    
    	


    return SUCC;
}

char *ismacrodec(struct macro *head, char *str){
	char *name; /* the name of the macro */
	char *macro = MAC; /* pointer to the macro keyword */
	int i = 0;
	
	/* count how many characters in the first field of str are identical to MAC */
	while (str[i] == macro[i] && str[i] != EOS && macro[i] != EOS) i++;
	
	
	/* check if the first field is equal to MAC */
	if (!(i == strlen(macro) && str[i] == BLK)){
		return NULL; /* the line is not a macro declaration */
	}
	
	/*	under the assumption that there is BLK only between every two fields and
		there can't be consecutive BLK, there must be a second field.		*/
	
	/* put the start of the second field in "name" */
	name = str + (++i);
	
	/* find the end of the second field */
	while (str[i] != EOS && str[i] != BLK) i++;
	
	if (str[i] == BLK){ /* more than two fields */
		return NULL;
	}
	
	
	/* the string has two fields and the first is "macro" */
	
	
	/* check if there already is a macro with this name */
	if (getcontent(head, name) != NULL){
		return NULL;
	}
	
	
	/*check if the name is an instruction */
	if (findinst(name) != NULL){
		return NULL;
	}
	
	
	/* check if the name is a register */
	if (isreg(name) != NULL){
		return NULL;
	}
	
	
	/* check if the name is a name of a guide (with or without the prefix) */
	
	/* check if it starts with the prefix */
	if (*name == DOT){
		name++;
	}
	
	/*	if the name doesn't start with DOT, check if name+0 (which is equal to name) is one of the guides's names
		if the name does start with DOT, check if name+1 (from the end of the prefix) is one of the guides's names	*/
	if (findguide(name) != NULL){
		return NULL;
	}
	
	/* it is a valid declaration */
	return name;
}


char *getcontent(struct macro *head, char *nameOfMacro){
	
	newMacro *ptr = head; /* pointer to scan the linked list */
		
	
	/* scan the list untill ptr is NULL or untill a node with the same name is found */
	for (ptr = ptr->next ; ptr != NULL && strcmp(ptr->name, nameOfMacro);ptr = ptr->next);
	
	/* return NULL if the node wasn't found. otherwise, return the content */
	return ptr == NULL ? NULL : ptr->content;
}



void freeList(newMacro *head){
		newMacro *ptr = head; /* pointer to the current list */
		newMacro *temp;
		/* scan the current list and free each node */
		while (ptr != NULL){
		    temp = ptr->next; /* save the next node */
			free(ptr);
			ptr = temp; /* continue to the next node */
		}
		
}

