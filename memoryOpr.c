#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memoryOpr.h"
#include "error.h"
#include "ioFuncs.h"

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

char *getcontentLbl(lblword *head, char *nameOflbl){
	
	lblword *ptr = head; /* pointer to scan the linked list */
	char *copy; /* to return a copy of ptr->data */
	
	/* scan the list untill ptr is NULL or untill a node with the same name is found */
	for (ptr = ptr->next ; ptr != NULL && strcmp(ptr->name, nameOflbl);ptr = ptr->next);
		
	if(ptr == NULL) /* return NULL if the node wasn't found.*/
		return NULL;
	copy = malloc((strlen(ptr->data) + 1) * sizeof(char)); /* allocate space for the new string */
	copy = duplstr(ptr->data); /* copy ptr->data to the new string */
	copy[strlen(ptr->data)-1] = EOS; /* put '\0' at the end of the string */
	return copy; 	 /* return the content */

}


int getkindLbl(lblword *headOfLbl, char *nameOfLbl){
	
	lblword *ptr= headOfLbl; /* pointer to scan the linked list */
	
	/* scan the list until ptr is NULL or untill a node with the same name is found */
	for ( ptr = ptr->next ; ptr != NULL && strcmp(ptr->name, nameOfLbl) ; ptr = ptr->next);
	
	/* return NOTFOUND if the node wasn't found. otherwise, return the kind */
	return ptr == NULL ? NOTFOUND : ptr->kind;
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
	while(head != NULL ) {
	
		tmp = head -> next;
		freeall(head->name,head->data,  NULL);
		free(head);

		head = tmp; /* continue to the next node */
	}
	
	return;
}




int printMemList(FILE *fp, word *head, int num1, int num2){
	int memc = MEM_STRT; /* memory counter */
	char adrStr[LEN_ADR]; /* the string of the address*/
	
	/* write to the file coded numbers */
	fprintf(fp,"\t\t%d\t%d",num1,num2);
	
	/* scan the list */
	while (head != NULL) {
		char *adrs, *data2; /* the data needed to be printed */
		sprintf(adrStr,"%d",memc++);
		
		if ((adrs = addstr("0",adrStr)) == NULL){ /* save the current adrs */
			return ERR_MEM;
		}

		/* convert the data of the current node to special base 2 */
		if ((data2 = convert2(head->data)) == NULL){
			free(adrs);
			return ERR_MEM;
		}
		/* print to the given file */
		writeline(fp, ESTR);
		if (write2str(fp, adrs, data2) < 0){
			freeall(adrs, data2, NULL);
			return ERR_PRINT;
		}
		
		freeall(adrs, NULL);
		
		head = head->next; /* continue to the next node */
	}
	
	return SUCC;
}


int printLblList(FILE *fp, lblword * head){
	int firstline = 1; /* flag to note that we are on the first node */
	
	/* scan the list */

	while (head != NULL) {
      		char adr[LEN_ADR]; /* an array to save the address of the lable */
     		adr[LEN_ADR - 1] = EOS; /* put NULL at the end of the array */
     
		sprintf(adr,"%d",binary_to_decimal(head->data)); /* the data needed to be printed */
			   
		/* if it's not the first line, separate the new line from the previous one */
		if (firstline){
			firstline = !firstline; /* turn off the flag */
		} else {
			writeline(fp, ESTR); /* separate the lines */
		}
		
		/* print the data(name and address) to the given file */
		if(write2str(fp, head->name, adr) < 0){
			return ERR_PRINT;
		}

		head = head->next; /* continue to the next node */
	}
	
	return SUCC;
}

int binary_to_decimal(char *binary) {
	int len = strlen(binary),i;	/* get the length of the binary string */
    	int decimal = 0;
    	int base = 1;
    	for (i = len - 1; i >= 0; i--) { /* iterate through the binary string from right to left */
		if (binary[i] == '1') { /* if the current bit is 1, add the corresponding base value to the decimal value */
            		decimal += base;
        	}
        	base *= 2; /* update the base value for the next iteration */
   	 }
   	 return decimal;
}


int countNodes(lblword *head) {
	int count = 0; /* initialize a variable to store the count of nodes in the list */
	lblword *current; /*create a pointer to traverse the linked list, and initialize it to the head of the list */
        current = head;
   
   	while(current != NULL) { /* while the current node is not NULL */
      		count++;
      		current = current->next; /* move the current pointer to the next node in the list */
   	}
   
   	return count;  /* return the final count of nodes in the linked list */
}



