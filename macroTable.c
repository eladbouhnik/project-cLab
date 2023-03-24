#include "preAsm.h"
#include "macroTable.h"
#include "guides.h"
#include "instruction.h"
#include "utils.h"
#include "error.h"
#include "ioFuncs.h"

int addMcr(newMacro *head,char *name,char *content){
	char *cpyName, *cpyContent;
	newMacro *current = head;
	while (current->next != NULL) { /*advanced to the last node */
        	current = current->next;
  	}
    
	current->next = (newMacro *) malloc(sizeof(newMacro));  /* the new node that will be added to the list */
	if(current->next == NULL) /*check the allocation*/
    		return ERR_MEM;
    
    	/* duplicate the given name and content - to prevent aliasing */
	if ((cpyName = duplstr(name)) == NULL || (cpyContent = duplstr(content)) == NULL){
		free(cpyName);
		free(cpyContent);
		return ERR_MEM;
	}
	
	current->next->name = cpyName;
	current->next->content = cpyContent;
	current->next->next = NULL;
    
	return SUCC;
}

char *getcontent(newMacro *head, char *nameOfMacro){
	
	newMacro *ptr = head; /* pointer to scan the linked list */
		
	
	/* scan the list untill ptr is NULL or untill a node with the same name is found */
	for (ptr = ptr->next ; ptr != NULL && strcmp(ptr->name, nameOfMacro);ptr = ptr->next);
	
	/* return NULL if the node wasn't found. otherwise, return the content */
	return ptr == NULL ? NULL : ptr->content;
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
