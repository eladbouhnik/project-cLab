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

char *getcontentLbl(lblword *head, char *nameOflbl){
	
	lblword *ptr = head; /* pointer to scan the linked list */
		char *copy;
	
	/* scan the list untill ptr is NULL or untill a node with the same name is found */
	for (ptr = ptr->next ; ptr != NULL && strcmp(ptr->name, nameOflbl);ptr = ptr->next);
	
	/* return NULL if the node wasn't found. otherwise, return the content */

	if(ptr==NULL)
	return NULL;
	copy = malloc((strlen(ptr->data) + 1) * sizeof(char)); /* allocate space for the new string */
	copy = duplstr(ptr->data);
	copy[strlen(ptr->data)-1] = EOS;
	return copy;
}


int getkindLbl(lblword *headOfLbl, char *nameOfLbl){
	
	lblword *ptr= headOfLbl; /* pointer to scan the linked list */
	
	/* scan the list untill ptr is NULL or untill a node with the same name is found */
	for (	ptr = ptr->next	;	ptr != NULL && strcmp(ptr->name, nameOfLbl)	;	ptr = ptr->next);
	
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
     char code[4];
     code[3]='\0';
     
		sprintf(code,"%d",binary_to_decimal(head->data)); /* the data needed to be printed */
			   
		/* if it's not the first line, seperate the new line from the previous one */
		if (firstline){
			firstline = !firstline; /* turn off the flag */
		} else {
			writeline(fp, ESTR); /* seperate the lines */
		}
		
		/* print the numbers to the given file */
		if(write2str(fp, head->name, code) < 0){
			return ERR_PRINT;
		}

		head = head->next; /* continue to the next node */
	}
	
	return SUCC;
}

int binary_to_decimal(char *binary) {
    int len = strlen(binary),i;
    int decimal = 0;
    int base = 1;
    for (i = len - 1; i >= 0; i--) {
        if (binary[i] == '1') {
            decimal += base;
        }
        base *= 2;
    }
    return decimal;
}

void setLastNodeToNull(lblword *head) {
    lblword *currentNode;
     currentNode = head;

    /* Traverse the list until the last node is reached*/
    while (currentNode->next != NULL) {
        currentNode = currentNode->next;
    }

    /*Set the next field of the last node to NULL*/
    currentNode->next = NULL;
}

int countNodes(lblword *head) {
   int count = 0;
   lblword *current;
   current = head;
   
   while(current != NULL) {
      count++;
      current = current->next;
   }
   
   return count;
}

