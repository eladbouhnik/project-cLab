#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "ioFuncs.h"
#include "memoryOpr.h"
#include "error.h"
#include "guides.h"
#include "utils.h"
#include "instruction.h"

int pass1(char *nameOfFile, lblword **headOfLbl, word **memhead, int status){
	int IC = 0, DC = 0; /* the instruction counter and the data counter */
	int i;
	char *line; /* the current line */
	int ln = 0; /* count lines */
	char lc = NLN; /* the last character in the current line */
	char *fullName;
	char plc; /* the last character of the previous line */ 
	FILE *source; /* the source file */
	lblword *lblTemp, *lblNode; /* temp lblword for "addnextlbl" */
	lblword *ptr; /* a pointer to point the head of the labels linked list */
		
	word *datahead, *memnode = NULL, *datanode = NULL; /* lists for the operations and data memory words */
	*memhead = (memnode = addnext(NULL, NULL)); /* create dummy head */
	datahead = (datanode = addnext(NULL, NULL)); /* create dummy head */
	
	*headOfLbl = (lblNode = addnextlbl(NULL,"head", "null", DEFK)); /* table of labels - creating head*/
	ptr = *headOfLbl; /* point the head of the labels linked list */
	fullName = addstr(nameOfFile, EAM); /* the name of the source file */
	
	/* check allocations */
	if (*headOfLbl == NULL || *memhead == NULL || datahead == NULL || fullName == NULL){
		free(fullName);
		freeLblList(*headOfLbl); 
		freeMemList(*memhead);
		freeMemList(datahead);
		report(ERR_MEM, ln);
		return ERR_MEM;
	}
	
	/* open the source file */
	if ((source = fopen(fullName,"r")) == NULL){
		free(fullName);
		freeLblList(*headOfLbl); 
		freeMemList(*memhead);
		freeMemList(datahead);
		report(ERR_FOE, ln);
		return ERR_FOE;
	}
	
	free(fullName);
	
	/* scan the lines of the source file */
	while ((plc = lc) != EOF && (line = readline(source, &lc, NULL)) != NULL){
		char *lbl; /* string for the label declaration of the current line */
		int lbllen; /* the length of the label declaration */
		char **args; /* the operands of the current line */
		int lenargs; /* the number of operands */
		int typ; /* the type of the operation (GTYP or ITYP) */
		void *opr; /* the operation in the current line (instruction or guide) */
		char *offset = line; /* offset that will point to after the label of the line (is the label exists) */
		
		ln++; /* increase the line number by one */
		
		/* check if the line is empty */
		if (*line == EOS){
			free(line);
			continue; /* continue to the next line */
		}
		
		
		/* check if there is a label at the start of the line */
		lbl = islbl(line);
		lbllen = sizeoffd(line);
		
		if (lbl != NULL){ /* check if the label name is valid */
			offset = skipfd(line); /* skip the label */
			
			/* if the label name is too long, forget it and report */
			if(LBLLENMAX < lbllen){
				report(status = ERR_LBL_LEN, ln);
				free(lbl);
				lbl = NULL;
			}
			
			/* check if there is only label in the line */
			if (*offset == EOS){
				freeall(line, lbl, NULL); 
				report(status = ERR_LINE_OLBL , ln);
				continue; /* continue to the next line */
			}
		} else if (line[lbllen - 1] == COL){ /* check if the label name wasn't valid */
			offset = skipfd(line); /* skip the label */
			
			report(status = ERR_LBL_DEC, ln);
			
			/* check if there is only label in the line */
			if (*offset == EOS){
				free(line);
				report(status = ERR_LINE_OLBL , ln);
				continue; /* continue to the next line */
			}
		}
		
		
		/* split the line to its components */
		if ((typ = splitline(offset, &opr, &args, &lenargs)) != GTYP && typ != ITYP){ /* check for errors */
			freeall(line, args, lbl, NULL);
			
			/* check which error occurred */
			if(typ == ERR_MEM){
				freeLblList(*headOfLbl);
				freeMemList(*memhead);
				freeMemList(datahead);
				fclose(source);
				report(ERR_MEM, ln);
				return ERR_MEM;
			}else{
				report(status = typ, ln);
				continue; /* continue to the next line */
			}
			
			
		}
		

		/* check for allocation guide (for example ".data") */
		if (typ == GTYP && (((guide *) opr)->kind & GALL)){
			guide *op = (guide *) opr; /* cast opr to (guide *) */
			
			/* check the number of operands */
			if (!((op->opmin) <= lenargs && (lenargs <= (op->opmax) || (op->opmax) == GNM))){
				freeall(line, args, lbl, NULL);
				report(status = ERR_ARGS_NUM, ln);
				continue; /* continue to the next line */
			}
			
			/* check if there is a label declaration in the line */
			if (lbl != NULL){
				char *strDC; /* the location in the data list that the current label should point to (after convertion to binary) */
				int ltyp; /* the type of the label*/
				
				/* check if the label already exists */
				if (getcontentLbl(*headOfLbl , lbl) != NULL){
					freeall(line, args, lbl, NULL);
					report(status = ERR_LBL, ln);
					continue; /* continue to the next line */
				}
				
				/* convert the data counter to binary representation */
				if ((strDC = itostr(DC)) == NULL){
					freeall(line, args, lbl, NULL);
					freeLblList(*headOfLbl);
					fclose(source);
					freeMemList(*memhead);
					freeMemList(datahead);
					report(ERR_MEM, ln);
					return ERR_MEM;
				}
				
				/* set the type according the guide's type */
				ltyp = GALL;
				
				/* add to the table */
				if ((lblTemp = addnextlbl( lblNode, lbl, strDC, ltyp)) == NULL){
					freeall(line, args, lbl, strDC, NULL);
					freeLblList(*headOfLbl);
					fclose(source);
					freeMemList(*memhead);
					freeMemList(datahead);
					report(ERR_MEM, ln);
					return ERR_MEM;
				}
		

				freeall(lbl, strDC, NULL); /* free the label and then address */
				lblNode = lblTemp;  /* change to the new node */
			}
			/* check what guide it is- string or data and
			   convert the data to binary format     */
			for (i = 0; i < lenargs; i++){
				word *temp; /* temp word for "addnext" */
				
				/* check if the operand should be an integer */
				if (op->deftype == INTTYP){
					int intop; /* the operand after conversion to int from string */
					char *suf; /* for the strtol function */
					char *biop; /* the operand after conversion to binary */
					
					/* convert the operand to integer */
					intop = (int)strtol(args[i], &suf, DECBASE);
					
					/* check if the number is not too large or small */
					if (intop < MINDTNUM || MAXDTNUM < intop){
						report(status = ERR_LMT_GUN, ln);
						continue; /* continue to the next operand */
					}
					
					/* check if the operand was valid integer (no garbage characters) */
					if (*suf != EOS){
						report(status = ERR_OPD, ln);
						continue; /* continue to the next operand */
					}
					
					/* convert to binary base */
					if ((biop = itostr(intop)) == NULL){
						freeall(line, args, NULL);
						freeLblList(*headOfLbl);
						fclose(source);
						freeMemList(*memhead);
						freeMemList(datahead);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					/* add to the data list */
					if ((temp = addnext(datanode, biop)) == NULL){
						freeall(line, biop, args, NULL);
						freeLblList(*headOfLbl);
						fclose(source);
						freeMemList(*memhead);
						freeMemList(datahead);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					free(biop);
					
					datanode = temp; /* change to the new node */
					DC++;
				} else if (op->deftype == STRTYP){ /* check if the operand should be a string */
					char *ptr; /* pointer to scan the string */
					/* check if the first character is QUM */
				
					if (*args[i] != QUM){
						report(status = ERR_OPD, ln);
						continue;  /* continue to the next line */
					}
					
					/* move ptr to point to the first character of the string */
					ptr = args[i] + 1;
					
					if (*ptr == QUM){ /* check if the string is empty */
						*ptr = EOS; /* replace QUM with EOS to add EOS to the data list */
					}
					
					/* scan the string until the QUM */
					while (*ptr != QUM){
						char *biop; /* the current character after convertion to binary */
						
						/* convert to binary base */
						if ((biop = itostr(*ptr)) == NULL){
							freeall(line, args, NULL);
							freeLblList(*headOfLbl);
							fclose(source);
							freeMemList(*memhead);
							freeMemList(datahead);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						/* add to the data list */
						if ((temp = addnext(datanode, biop)) == NULL){
							freeall(line, biop, args, NULL);
							freeLblList(*headOfLbl);
							fclose(source);
							freeMemList(*memhead);
							freeMemList(datahead);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						free(biop);

						datanode = temp; /* point to the new node */
						DC++;
						
						/*	the string has QUM at the end so when we reach it, we replace it with EOS
							to create binary representation of it, and than replacing it back to QUM
							to end the while.								*/
						if (*ptr == EOS){ /* check if it was the last loop of the while */
							*ptr = QUM; /* change back to QUM */
						} else if (*(ptr + 1) == QUM){ /* check if the next character is QUM */
							*(++ptr) = EOS; /* replace QUM with EOS and continue to it */
						} else {
							ptr++; /* continue to the next character */
						}
					}
				}
			}
		} else if (typ == GTYP) { /* the guide is "extern" or "entry" (because it is not an allocation guide) */
			guide *op = (guide *)opr; /* cast opr to (guide *) */
			
			/* check if there is a label declaration in the line */
			if (lbl != NULL){
				printf("Warning in line %d: Labels before \".entry\" and \".extern\" are ignored.\n", ln);
				free(lbl);
			}
			
			/* check if the guide is entry (the entry guide will be taen care of in "pass2") */
			if (op->kind == GENT){
				freeall(line, args, NULL);
				continue; /* continue to the next line */
			}
			
			/* the guide is ".extern" */
			
			/* check the number of operands*/
			if (!((op->opmin) <= lenargs && (lenargs <= (op->opmax) || (op->opmax) == GNM))){
				freeall(line, args, NULL);
				report(status = ERR_ARGS_NUM, ln);
				continue; /* continue to the next line */
			}
			
			/* convert the operands */
			for (i = 0; i < lenargs; i++){
				char *ptr = args[i]; /* set ptr to the first character of the current argument */
				int allalnum = 1; /* flag to remember if all character in the operand are alpha-numeric */
				
				/* check the syntax of the label */
				
				/* check if the first character is alphabet */
				if (!isalpha(*ptr)){
					report(status = ERR_OPD, ln);
					continue; /* continue to the next operand */
				}
				
				/* check if all the characters are alpha-numeric */
				while (*ptr != EOS){
					if (!isalnum(*ptr++)){ /* check the current character */
						report(status = ERR_OPD, ln);
						allalnum = !allalnum; /* continue to the next operand */
						break; /* there is no reason to check the other characters of the current operand */
					}
				}
				
				if (!allalnum){
					continue; /* continue to the next operand */
				}
				
				/* check if the label already exists in the file */
				if (getcontentLbl(*headOfLbl, args[i]) != NULL){
					report(status = ERR_LBL, ln);
					continue; /* continue to the next operand */
				}
				
				/* add the label to the table with GEXT type */
				if ((lblTemp = addnextlbl(lblNode, args[i], EXTADR, GEXT)) == NULL){
					freeall(line, args, NULL);
					freeLblList(*headOfLbl);
					fclose(source);
					freeMemList(*memhead);
					freeMemList(datahead);
					report(ERR_MEM, ln);
					return ERR_MEM;
				}
				lblNode = lblTemp; /* point to the new node */
			}
		} else { /* instruction */
			instruct *op = (instruct*)opr; /* cast opr to (instruct *) */
			char *opword; /* the memory word of the operation */
			word *temp; /* temporary word for the "addnext" */
			int nop; /* the number of operands that op neads */
			int adr[] = {ACN, ACN}; /* the addressing of the operands given in the line */
			int fcont = SUCC; /* flag to remember to continue to the next line */
			int fadr3 = AC3; /* flag to check if both operands are registers (the addressings are AC3) */
			opcount(nop, op); /* put in nop the number of required operands */
			
			/* check the number of operands */
			if (nop != lenargs){
				freeall(line, args, lbl, NULL);
				report(status = ERR_ARGS_NUM, ln);
				continue; /* continue to the next line */
			}
			
			/* check if there is a label */
			if (lbl != NULL){
				char *adrs; /* the address of the label */
				
				/* check if the label already exists */
				if (getcontentLbl(*headOfLbl, lbl) != NULL){
					freeall(line, args, lbl, NULL);
					report(status = ERR_LBL, ln);
					continue; /* continue to the next line */
				}
				
				/* convert the instruction counter to the binary representation */
				if ((adrs = itostr(IC + MEM_STRT)) == NULL){
					freeall(line, args, lbl, NULL);
					freeLblList(*headOfLbl);
					fclose(source);
					freeMemList(*memhead);
					freeMemList(datahead);
					report(ERR_MEM, ln);
					return ERR_MEM;
				}
											
				movel2(adrs); /* move the binary 2 characters to the left */
				opcA(adrs); /* set the ARE of the address */

				/* add the label to the table */
				if ((lblTemp = addnextlbl(lblNode, lbl, adrs,DEFK)) == NULL){
					freeall(line, adrs, args, lbl, NULL);
					freeLblList(*headOfLbl);
					fclose(source);
					freeMemList(*memhead);
					freeMemList(datahead);
					report(ERR_MEM, ln);
					return ERR_MEM;
				}

				freeall(lbl, adrs, NULL); /* free the label and address*/
				lblNode = lblTemp; /* point to the new node */
				
			}
			
			/* allocate memory for the word */
			opword = (char *)malloc((WORDSIZE + 1) * sizeof(char));
			
			/* check the allocation */
			if (opword == NULL){
				freeall(line, args, NULL);
				freeLblList(*headOfLbl);
				fclose(source);
				freeMemList(*memhead);
				freeMemList(datahead);
				report(ERR_MEM, ln);
				return ERR_MEM;
			}
			strcpy(opword,DEFADR); /* copy the default address("00000000000000") to opword */

			opcA(opword);  /*set the ARE of the operation word */

			/* write the opcode in the operation word */
			for(i = 0; i < OPCLEN; i++ ) { 
				gbw(opword, i + OPCSTRT) = getbit(op->opcode, i) + '0';
				
			}
 
			/* add the operation word to the instructions list */
			if ((temp = addnext(memnode, opword)) == NULL){
				freeall(line, args, opword, NULL);
				freeLblList(*headOfLbl);
				fclose(source);
				freeMemList(*memhead);
				freeMemList(datahead);
				report(ERR_MEM, ln);
				return ERR_MEM;
			}
			
			free(opword);
			
			memnode = temp; /* point to the new node */
			IC++; /* count the added node */
			
			/* check the addressing of the operands */
			for (i = 0; i < lenargs; i++){
				adr[i] = findadr(args[i]); /* check the adderssing of the curent operand */
				if(adr[i] == ERR_OPD){ /* check if operand addresing illegal */
					fcont = ERR_OPD;
				} else if (adr[i] != AC3){ /* check if the operand is not register */
					fadr3 = adr[i];
				}
			}
			
			/* check if the operands do not fit to the addressing syntax of anything */
			if (fcont == ERR_OPD){
				/* error reported in "pass2" */
				status = ERR_OPD;
				freeall(line, args, NULL);
				continue; /* continue to the next line */
			}
			
			/* increase the instruction counter according the addressing methods */
			if (fadr3 == AC3 && lenargs == TWOARGS){ /* if both operands are registers (and there are two operands) - only one word added */
				IC++;
			} else {
				IC += lenargs; /* there is at least one additional word for each operand */
				
				/* check if each operand is from address method 2 */
				for (i = 0; i < lenargs; i++){
					if (adr[i] == AC2)  /* more two words for address method 2 (two parameters) */
						IC+=2;
					if(adr[i] == TWO_REG_PARAM) /* operand of address method two with two registers as parameters*/
						IC++;
				}
			}
		}
		
		freeall(line, args, NULL);
	}
	
	/* close the source file */
	if (fclose(source) == EOF){
		freeLblList(*headOfLbl);
		freeMemList(*memhead);
		freeMemList(datahead);
		report(ERR_FCE, ln);
		return ERR_FCE;
	}
	
	/* check if the number of memory requaired is larger than the allowed memory */
	if (MEM_END < MEM_STRT + IC + DC + 1){
		report(status = ERR_FULL_MEM, ln);
	}
	
	/* scan the table(linked list) of labels of GALL to fix their addresses (currently has only the position in the data list) */
	
	
		/* check if there is a list in this cell */
		while ( ptr != NULL){ /* pointer to scan the current list in the table */
			/* check if the node is a data allocation label */
			if (ptr->kind == GALL){
				int j;
				/* fix the position with respect to the starting place of the memory and the instruction counter */
				int newadr = MEM_STRT + IC + (int)strtol(ptr->data, NULL, BINARYBASE);
				
				opcR(ptr->data); /* set the ARE of the address */
				
				/* write the fixed position to the address */
				for(j = 0; j < ADRLEN; j++) {
					/* write the j-th bit of newadr to the j-th bit of ptr->data */
					gbw(ptr->data, j + ADRSTRT) = getbit(newadr, j) + '0';
				}
			}

			ptr = ptr->next; /* continue to the next node of the current list */
		}
	
	
	/* put the node that after the dummy head of datahead after the end of the memory list */
	memnode->next = datahead->next;
	
	/* delete only the dummy head of datahead */
	datahead->next = NULL;
	freeMemList(datahead);
	
	/* set memnode to the actual head (the node after the dummy head) of the list */
	memnode = (*memhead)->next;
	
	/* free the dummy head */
	(*memhead)->next = NULL;
	freeMemList(*memhead);
	
	/* set the head to the actual head */
	*memhead = memnode;
	
	return status;
}
	
	





