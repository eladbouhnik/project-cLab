#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memoryListR.h"
#include "error.h"
#include "ioR.h"


word *addnext(word *curr, char *data){
	word *newword; /* the new node that will be added to the list */
	word *tmp = NULL; /* to remember the previous "next" node of curr */
	char *newcon; /* the duplicated content of the new node */
	
	newword = (word *)malloc(sizeof(word));
	isnull(newword, NULL); /* check the allocation */
	
	/* duplicate the given data - to prevent aliasing */
	if((newcon = duplw(data)) == NULL){
		free(newword);
		return NULL;
	}
	
	newword->data = newcon;
	
	/* if "curr" is not null, put the new node after it */
	if (curr != NULL){
		tmp = curr->next;
		curr->next = newword;
	}
	
	/* put the previous next after the new node */
	newword->next = tmp;
	
	return newword;
}


lblword *addnextlbl(lblword *curr, char *name, char *data, int newKind){
	lblword *newword; /* the new node that will be added to the list */
	lblword *tmp = NULL; /* to remember the previous "next" node of curr */
	char * newcon = NULL; /* the duplicated content ,name of the new node */
	char *newname = NULL;
	newword = (lblword *)malloc(sizeof(lblword));
	isnull(newword, NULL); /* check the allocation */
	
	/* duplicate the given data - to prevent aliasing */
	newname = dupl(name == NULL ? ESTR : name); /* if the name is NULL, create an empty string */
	if(newname == NULL || (newcon = duplw(data)) == NULL){
		freeall(newword, newname, newcon);
		return NULL;
	}
	
	newword->name = newname;
	newword->data = newcon;
	newword->kind = newKind;
	
	/* if "curr" is not null, put the new node after it */
	if (curr != NULL){
		tmp = curr->next;
		curr->next = newword;
	}
	
	/* put the previous next after the new node */
	newword->next = tmp;
	
	return newword;
}

char *getcontentLbl(struct lblword *head, char *nameOflbl){
	
	lblword *ptr = head; /* pointer to scan the linked list */
		
	
	/* scan the list untill ptr is NULL or untill a node with the same name is found */
	for (ptr = ptr->next ; ptr != NULL && strcmp(ptr->name, nameOflbl);ptr = ptr->next);
	
	/* return NULL if the node wasn't found. otherwise, return the content */
	return ptr == NULL ? NULL : ptr->data;
}

void freeMemList(word * head){
	word * tmp; /* node to remember the next node */
	
	/* scan the list and free each node and its field */
	while(head != NULL) {
		tmp = head -> next;
		
		free(head -> data);
		free(head);
		
		head = tmp; /* continue to the next node */
	}
	
	return;
}


void freeLblList(lblword * head){
	lblword * tmp; /* node to remember the next node */
	
	/* scan the list and free each node and its fields */
	while(head != NULL) {
		tmp = head -> next;
		
		freeall(head -> name, head->data, NULL);
		free(head);
		
		head = tmp; /* continue to the next node */
	}
	
	return;
}


int printMemList(FILE *fp, word *head, int num1, int num2){
	int memc = MEM_STRT; /* memmory counter */
	char adrStr[4]; /* the string of the address*/
	

	
	
	/* write to the file coded numbers */
  	fprintf(fp,"\t\t%d\t%d",num1,num2);
	
	
	/* scan the list */
	while (head != NULL) {
		char *adrs2, *data2; /* the data needed to be printed */
		
		sprintf(adrStr,"%d",memc++);
		
		/* convert to special base 2 */
		if ((adrs2 = addstr("0",adrStr)) == NULL){

			return ERR_MEM;
		}
		
	
		
		/* convert the data of the current node to special base 2 */
		if ((data2 = convert2(head->data)) == NULL){
			free(adrs2);
			return ERR_MEM;
		}
		
		/* print to the given file */
		writeline(fp, ESTR);
		if (write2str(fp, adrs2, data2) < 0){
			freeall(adrs2, data2, NULL);
			return ERR_PRINT;
		}
		
		freeall(adrs2, NULL);
		
		head = head->next; /* continue to the next node */
	}
	
	return SUCC;
}


int printLblList(FILE *fp, lblword * head){
	int firstline = 1; /* flag to note that we are on the first node */
	
	/* scan the list */
		

	while (head != NULL) {
		char *code2 = convert2(head->data); /* the data needed to be printed */
		
		isnull(code2, ERR_MEM); /* check convert2 */
		
		/* if it's not the first line, seperate the new line from the previous one */
		if (firstline){
			firstline = !firstline; /* turn off the flag */
		}
		else {
			writeline(fp, ESTR); /* seperate the lines */
		}
		
		/* print the numbers to the given file */
		if(write2str(fp, head->name, code2) < 0){
			free(code2);
			return ERR_PRINT;
		}
		free(code2);
		
		head = head->next; /* continue to the next node */
	}
	
	return SUCC;
}



