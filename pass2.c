#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ioFuncs.h"
#include "utils.h"
#include "memoryOpr.h"
#include "guides.h"
#include "error.h"
#include "instruction.h"
#include "pass2.h"


int pass2(char *nameOfFile, int status, lblword *headOfLbl, word *head){
	FILE *source, *obj, *ent, *ext; /* pointers to source file and destination files */
	char lc = NLN; /* the last character of the current file */
	char *fullName;
	char plc; /* the previous last character*/
	char *line; /* current line */
	int ln = 0; /* line counter */
	int ic = 0, dc = 0; /* instruction and data counters */
	int i;
	int Two_Par_stat = 0; /* the status of problems with parameters */
	int res; /* returned value from functions that report about more than one error */
	word *wordnode = head; /* pointer to edit the word list */
	lblword *enthead ; /* list for entry labels */
	lblword *exthead ,*extTemp,*extNode; /* list for extern labels */
	enthead = addnextlbl(NULL, NULL, NULL,DEFK); /* create dummy head for the list */
	exthead =(extNode = addnextlbl(NULL, NULL, NULL,DEFK)); /* create dummy head for the list */

	/* check if the lists were created */
	if (enthead == NULL || exthead == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		freeLblList(headOfLbl);
		report(ERR_MEM, ln);
		return ERR_MEM;
	}
	
	/* create the name of the source file */
	if ((fullName = addstr(nameOfFile, EAM)) == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		freeLblList(headOfLbl);
		report(ERR_MEM, ln);
		return ERR_MEM;
	}
	
	/* open the source file */
	if ((source = fopen(fullName,"r")) == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		freeLblList(headOfLbl);
		free(fullName);
		report(ERR_FOE, ln);
		return ERR_FOE;
	}
	
	free(fullName);

	/* scan the file's lines */
	while ((plc = lc) != EOF && (line = readline(source, &lc, NULL)) != NULL){ /* read line from source file */

		char *offset = line; /* to skip the part of the line with label */
		char **args; /* the operands of the current line */
		char **params; /* the parameters of the current line */
		int lenargs; /* number of operands */
		char *lbl; /* string for the label of the current line */
		int lbllen; /* the length of the label declaration */
		int adrtg = ACN; /* the addressing method of the target operand */
		int adrsc = ACN; /* the addressing methods of the source operand */
		int adrParam1 = ACN; /* the addressing method of the first paramter */
		int adrParam2 = ACN; /* the addressing methods of the second parameter */
		instruct *ip; /* instruct pointer */
		int opNum; /* the number of operands required by the current instruction */
		ln++; /* count the current line */


		/* check if the line is empty */
		if (*line == EOS){
			free(line);
			continue;
		}
		
									
		/* check if there is a label at the start of the line */
		lbl = islbl(line);
		lbllen = sizeoffd(line);
		
		if (lbl != NULL){ /* check if the label name is valid */
			offset = skipfd(line); /* skip the label */
			
			/* if the label name is too long, forget it and report */
			if(LBLLENMAX < lbllen){
				/* reported in pass1 */
				status = ERR_LBL_LEN;
				free(lbl);
				lbl = NULL;
			}
			
			/* check if there is only label in the line */
			if (*offset == EOS){
				freeall(line, lbl, NULL);
				/* reported in pass1 */
				status = ERR_LINE_OLBL;
				continue; /* continue to the next line */
			}
			
		} else if (line[lbllen - 1] == COL){ /* check if the label name wasn't valid */
			offset = skipfd(line); /* skip the label */
			
			/* reported in pass1 */
			status = ERR_LINE_OLBL;
			
			/* check if there is only label in the line */
			if (*offset == EOS){
				free(line);
				/* reported in pass1 */
				status = ERR_LINE_OLBL;
				continue; /* continue to the next line */
			}
		}
		

		/* split the line to several strings for further analyze */
		if((res = splitline(offset, (void**)(&ip), &args, &lenargs)) != GTYP && res != ITYP) {/* if res is not GTYP or ITYP there was an error */
			freeall(line, args, NULL);
			/* check which error occurred */
			if (res == ERR_MEM){
				freeLblList(enthead);
				freeLblList(exthead);
				freeMemList(head);
				freeLblList(headOfLbl);
				fclose(source);
				report(ERR_MEM, ln);
				return ERR_MEM;
			} else {
				/* the error is reported in "pass1" */
				status = res;
				continue; /* move to the next line */
			}
		}


		if(res == GTYP){ /* check if the operation is guide */
			guide *gp = (guide *)ip; /* covert ip to pointer to guide */
			
			/* check if the guide is not entry */
			if(gp -> kind != GENT){
				freeall(line, args, NULL);
				continue; /* move to the next line */
			}
			
			/* check the number of operands */
			if(!((gp->opmin) <= lenargs && (lenargs <= (gp->opmax) || (gp->opmax) == GNM))){ 
				freeall(line, args, NULL);
				report(status = ERR_ARGS_NUM, ln);
				continue; /* move to the next line */
			}

			/* scan the operands and add them to the entry list */
			for(i = 0; i < lenargs; i++){
				int kind; /* the kind of the label */
				char address[WORDSIZE]; /* the address of the entries label */
				char *lblw; /* the label's word */
				int j;
				
				/* check if the label exists and the kind is not GEXT (extern)*/
				if((kind = getkindLbl(headOfLbl, args[i])) == NOTFOUND){ /* if the label exists */
					report(status = ERR_ARGS_LBL, ln);
					continue; /* move to the next operand */
				} else if(kind == GEXT) { /* if the kind is extern */
					report(status = ERR_ENT_LBL, ln);
					continue; /* move to the next operand */
				}
				
				/* get the label's word */
				lblw = getcontentLbl(headOfLbl, args[i]);
				
				/* get the label's address */
				
				for (j = 0; j < ADRSTRT; j++){
					address[j] = '0'; /* set to zero the unused characters */
				}
				for (j = 0; j < ADRLEN; j++){
					gbw(address, j) = gbw(lblw, j + ADRSTRT); /* shift the bits of the address to the right */
				}

				/* add the label to the entry list */
				if(addnextlbl(enthead, args[i], address,kind) == NULL){
					freeall(line, args, NULL);
					freeLblList(enthead);
					freeLblList(exthead);
					freeMemList(head);
					freeLblList(headOfLbl);
					fclose(source);
					report(ERR_MEM, ln);
					return ERR_MEM;
				}
			}
			
			freeall(line, args, NULL);
			continue; /* move to the next line */
		}/* end of guide check */

		opcount(opNum,ip); /* count number of operands that ip require */
		if (opNum != lenargs){ /* check number of operands */
			/* error reported in "pass1" */
			status = ERR_ARGS_NUM;
			continue; /* move to the next line */
		}

		/* check the addressing methods */
		i = 0;							 

		switch (lenargs){ /* check the number of operands */
			case TWOARGS: /* there are two arguments - the first is the source and the second is the target */
				adrsc = findadr(args[i++]);
			case ONEARGS: /* there is one arguments - which is the target */
				adrtg = findadr(args[i++]);
				if(findadr(fstArg) == AC2|| findadr(fstArg) == TWO_REG_PARAM){ /* if the oprand is from addressing method 2*/
					params = getParams(fstArg); /* get the parameters */
					adrParam1 = findadr(fstPar); /* save the addressing method of the first parameter */
			 	    	adrParam2 = findadr(secPar); /* save the addressing method of the second parameter */
			 	   	if(adrtg == TWO_REG_PARAM)
			 	    		adrtg = AC2;
				  }
			break;
		} /* adrtg will remain ACN if there are no operands */


		/* check if the syntax of the operands is legal */
		if (adrsc == ERR_OPD || adrtg == ERR_OPD || adrParam1 == ERR_OPD || adrParam2== ERR_OPD){
			freeall(line, args, NULL);
			report(status = ERR_OPD, ln);
			continue; /* move to the next line */
		}

		/* write the addressing method in the operation word */
		if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
			for(i = 0; i < ADSLEN; i++) { 
				gbw(wordnode->data, i + ADSTGST) = getbit((adrtg == ACN ? AC0 : adrtg), i) + '0'; /* write the addressing method of the target */
				gbw(wordnode->data, i + ADSSCST) = getbit((adrsc == ACN ? AC0 : adrsc), i) + '0'; /* write the addressing method of the source */
				
        	if(adrParam1 != ACN && adrParam2 != ACN){ /* if there are parameters */
					/* write the addressing method of the parameters */
						gbw(wordnode->data, i + ADS_PARAM1_ST) = getbit((adrParam1 == ACN ? AC0 : adrParam1), i) + '0'; 
				
						gbw(wordnode->data, i + ADS_PARAM2_ST) = getbit((adrParam2 == ACN ? AC0 : adrParam2), i) + '0';

					}
			}
		}

/* check if the addressing method match the instruction (if there is no operand or parameters, the check does nothing) */
		if ((adrtg != ACN && (ip->optg & powr(BINARYBASE, adrtg)) != powr(BINARYBASE, adrtg)) 
		 || (adrsc != ACN && (ip->opsc & powr(BINARYBASE, adrsc)) != powr(BINARYBASE, adrsc)) ||
		  (adrParam1 != ACN && (ip->opParam1 & powr(BINARYBASE, adrParam1)) != powr(BINARYBASE, adrParam1)) ||
		    (adrParam2 != ACN && (ip->opParam2 & powr(BINARYBASE, adrParam2)) != powr(BINARYBASE, adrParam2))) {
		    
			report(status = ERR_OPD, ln);
			if(adrParam1 != ACN && adrParam2 != ACN)
			    freeall(line, args,params, NULL); /* free params , if there are parameters */
			else
				freeall(line, args, NULL);
			continue; /* move to the next line */
		}
		
		ic++; /* count the operation word */

		/* check if both operands or parameters are register (if there is less then two operands, at least one of them will be ACN) */
		if ((adrtg == AC3 && adrsc == AC3)|| (adrParam1 == AC3 && adrParam2 == AC3)){
			char memword[WORDSIZE + 1]; /* the registers' string word */
			int srcreg; 
			int trgtreg; 
			char *cop; /* the string words that should be added to the memory list */
			char *tok;
			char *copy;
			int kind;
			word *tmp; /* temporary word to check ERR_MEM */
			if(adrtg == AC3 && adrsc == AC3){
				srcreg = atoi(fstArg + 1); /* the number of the source register (the first character of the register is 'r') */
				trgtreg = atoi(secArg + 1); /* the number of the target register (the first character of the register is 'r') */
			}
			else {
				srcreg = atoi(fstPar + 1); /* the number of the first parameter register (the first character of the register is 'r') */
				trgtreg = atoi(secPar + 1); /* the number of the second parameter register (the first character of the register is 'r') */
				copy = malloc(strlen(fstArg) + 1); /* create a copy of the first operand for strtok */
				strcpy(copy,fstArg); /* copy of the first operand */
				tok = strtok(copy, "("); /* return pointer the the first encounter with open Brackets */
				
				/* get the kind of label (if exists) */
				if((kind = getkindLbl(headOfLbl, tok)) == NOTFOUND){
					freeall(line, args, NULL);
					report(status = ERR_ARGS_LBL, ln);
					continue; /* move to the next line */
				}

				cop = getcontentLbl(headOfLbl, tok); /* get the labels address */
				/* check if the label is external */
				if(kind == GEXT){
					char *strIC; /* the place where the reference to the external label is */
						
					opcE(cop); /* set the ARE */
					
					/* cast the position to binary */
					if((strIC = itostr(MEM_STRT + ic)) == NULL){
						freeall(line, args, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
						freeLblList(headOfLbl);
						fclose(source);
						report(ERR_MEM, ln);
						return ERR_MEM;	
					}

						/* add the label and position to the reference list */
					if ((extTemp = addnextlbl(extNode, tok, strIC,kind)) == NULL){ /* check if added to the list */
						freeall(line, args, strIC, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
						freeLblList(headOfLbl);
						fclose(source);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
          extNode = extTemp; /* point to the new node */
					freeall(strIC,copy,NULL);
			}
			else {
				opcR(cop); /* set the ARE */
			}		
					/* add label to the list */
					if (wordnode != NULL){
           /* make sure it is on the operation word (it could be NULL if there was a previous error) */
						if ((tmp = addnext(wordnode, cop)) == NULL){ /* check if added to the list */
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
				      freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
							wordnode = tmp; /* change to the next node */
							ic++; /* count the added node */
					}
			}
			
			
			/* writes the numbers (in binary) of the registers in the new word */
			for(i = 0; i < REGLEN; i++ ) {
				gbw(memword, i + REGTGST) = getbit(trgtreg, i) + '0'; /* write the target register */
				gbw(memword, i + REGSCST) = getbit(srcreg, i) + '0' ; /* write the source register */
			}
			
			opcA(memword); /* set the ARE of the word */
			/* add the word to the memory list */
			if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
				if ((tmp = addnext(wordnode, memword)) == NULL){
					freeall(line, args, NULL);
					freeLblList(enthead);
					freeLblList(exthead);
					freeMemList(head);
					freeLblList(headOfLbl);
					fclose(source);
					report(ERR_MEM, ln);
					return ERR_MEM;
				}
				wordnode = tmp; /* move to the new node */
				ic++; /* count the new word */
			}
			
		} else { /* all other cases that aren't 2 registers as arguments*/

			int op; /* numeric operands (after cast to int) */
			int kind; /* the kind of label operand */
			int reg,twoParams = 0; /* the number of a register operand (for example  1  is the number of the register "r1") */
			word *tmp; /* temporery word for ERR_MEM checks */
			char *cop; /* the string words that should be added to the memory list */
			char *tok; /* for strtok */
			char *copy; /* for strtok */
			/* check the adressing method of the first operand */ 	

			switch(lenargs == TWOARGS ? adrsc : adrtg){ /* if there is one operand it is the target, if there are two it is the source */
				case AC0: /* the operand is a number */
					op = atoi(fstArg + 1); /* cast the number of the operand (the first character is HSH)*/
					/* check if the number is too big or small */
					if (op < MINOPNUM || MAXOPNUM < op){
						freeall(line, args, NULL);
						report(status = ERR_LMT_DAN, ln);
						continue; /* move to the next line */
					}
					
					/* convert the number to binary representation */
					if((cop = itostr(op)) == NULL){
						freeall(line, args, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
						freeLblList(headOfLbl);
						fclose(source);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					movel2(cop); /* move to the left to set the ARE */
					opcA(cop); /* set the ARE */
					
					/* add the operand to the list */
					if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
						if((tmp = addnext(wordnode, cop)) == NULL){
							freeall(line, args, cop, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						wordnode = tmp; /* change to the new node */
						ic++; /* count the added operand word */
					}
					
					free(cop);

					break;
					
				case AC1: /* the operand is a label */
					/* check if label (operand) exists in the table*/
			
					if((kind = getkindLbl(headOfLbl, fstArg)) == NOTFOUND){ 
						freeall(line, args, NULL);
						report(status = ERR_ARGS_LBL, ln);
						continue; /* move to the next line */
					}
					cop = getcontentLbl(headOfLbl, fstArg); /* get content of the label */

					/* check if the label is external */
					if(kind == GEXT){
						char *strIC; /* the place where the reference to the external label is */
						
						opcE(cop); /* set the ARE */
						
						/* cast the position to binary */
						if((strIC = itostr(MEM_STRT + ic)) == NULL){
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						
						/* add the label and position to the reference list */
						if((extTemp = addnextlbl(extNode, fstArg, strIC,kind)) == NULL){
							freeall(line, args, strIC, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						extNode = extTemp; /* point to the new node */
						free(strIC);
					} else {
							opcR(cop); /* set the ARE */
						}

					/* add the operand to the list */
					if (wordnode != NULL) { /* make sure it is on the operation word (it could be NULL if there was a previous error) */
						if((tmp = addnext(wordnode, cop)) == NULL){ /* check if added to list */
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						wordnode = tmp; /* change to the new node */
						ic++; /* count the added operand word */

					}
					free(cop);
					break;

				case AC2: /* the operand is lable with parameters  (should add the address of the label and the parameters */

		      copy = malloc(strlen(fstArg) + 1); /* allocate space for the copy of the first operand */
					strcpy(copy,fstArg); /* copy the first operand */
					tok = strtok(copy, "("); /* return pointer the the first encounter with open Brackets */

					/* get the kind of label (if exists) */
					if((kind = getkindLbl(headOfLbl, tok)) == NOTFOUND){
						freeall(line, args,params, NULL);
						report(status = ERR_ARGS_LBL, ln);
						continue; /* move to the next line */
					}

					cop = getcontentLbl(headOfLbl, tok); /* get the labels address */
					/* check if the label is external */
					if(kind == GEXT){
						char *strIC; /* the place where the reference to the external label is */
						
						opcE(cop); /* set the ARE */
					
						/* cast the position to binary */
						  if((strIC = itostr(MEM_STRT + ic)) == NULL){
							freeall(line, args,params, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;	
						}

							/* add the label and position to the reference list */
						if((extTemp = addnextlbl(extNode, tok, strIC,kind)) == NULL){ /* check if added to the list */
							freeall(line, args, strIC,params, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						extNode = extTemp; /* point to the new node */
						freeall(strIC,copy,NULL);
					}
					else {
						opcR(cop); /* set the ARE */
					}

					/* add label to the list */
					if (wordnode != NULL){
		          /* make sure it is on the operation word (it could be NULL if there was a previous error) */
						if ((tmp = addnext(wordnode, cop)) == NULL){ /* check if added to the list */
							freeall(line, args,params, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
				       freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						wordnode = tmp; /* change to the next node */
						ic++; /* count the added node */
					}

					while(twoParams != TWOARGS && !Two_Par_stat ){ /* as long as we have not reached two parameters and there are no errors */
		      	if(twoParams == 0){
				    	if(adrParam1 == AC0){
									/* convert to string the field's number */
								if ((cop = itostr(atoi(fstPar + 1))) == NULL){
									freeall(line, args,params, NULL);
									freeLblList(enthead);
									freeLblList(exthead);
									freeMemList(head);
									freeLblList(headOfLbl);
									fclose(source);
								  report(ERR_MEM, ln);
									return ERR_MEM;
								}
					
								movel2(cop); /* move to make place for the ARE */
							  opcA(cop); /* set the ARE */
					
										/* add the field's number to the list */
								if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
									if ((tmp = addnext(wordnode, cop)) == NULL){
									  freeall(line, args, cop,params, NULL);
										freeLblList(enthead);
										freeLblList(exthead);
										freeMemList(head);
										freeLblList(headOfLbl);
										fclose(source);
										report(ERR_MEM, ln);
										return ERR_MEM;
									}

									wordnode = tmp; /* move to the new node */
									ic++; /* count the added node */
								}
					
								 twoParams++;
							}
							else 
							 if(adrParam1 == AC1){
								/* get the kind of label (if exists) */
								 if((kind = getkindLbl(headOfLbl, fstPar)) == NOTFOUND){
										freeall(line, args, params, NULL);
										report(status = ERR_ARGS_LBL, ln);
										Two_Par_stat = 1; /* change status of parameters */
										break; /* move to the next line */
									}
									cop = getcontentLbl(headOfLbl, fstPar); /* get the labels address */


									/* check if the label is external */
									if(kind == GEXT){
										char *strIC; /* the place where the reference to the external label is */

										opcE(cop); /* set the ARE */

										/* cast the position to binary */
										if((strIC = itostr(MEM_STRT + ic)) == NULL){
											freeall(line, args,params, NULL);
											freeLblList(enthead);
											freeLblList(exthead);
											freeMemList(head);
											freeLblList(headOfLbl);
											fclose(source);
											report(ERR_MEM, ln);
											return ERR_MEM;	
										}

										/* add the label and position to the reference list */
										if ((extTemp = addnextlbl(extNode, fstPar, strIC,kind)) == NULL){ /* check if added to the list */
											freeall(line, args, strIC,params, NULL);
											freeLblList(enthead);
											freeLblList(exthead);
											freeMemList(head);
											freeLblList(headOfLbl);
											fclose(source);
											report(ERR_MEM, ln);
											return ERR_MEM;
										}
										extNode = extTemp; /* point to the new node */
										free(strIC);
										
									} else {
											opcR(cop); /* set the ARE */
									  }

										/* add label to the list */
										if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
											if ((tmp = addnext(wordnode, cop)) == NULL){ /* check if added to the list */
												freeall(line, args,params, NULL);
												freeLblList(enthead);
												freeLblList(exthead);
												freeMemList(head);
												freeLblList(headOfLbl);
												fclose(source);
												report(ERR_MEM, ln);
												return ERR_MEM;
											}

											wordnode = tmp; /* change to the next node */
											ic++; /* count the added node */
									}

									twoParams++;

								}
								else {
										/*adrParam1 == AC3*/
									cop = (char *)malloc((WORDSIZE + 1) * sizeof(char)); /* allocate new memory for the operand */
									reg = atoi(fstPar + 1); /* get the register's number */

										/* check the memory allocate */
									if (cop == NULL){ 
										freeall(line, args,params, NULL);
										freeLblList(enthead);
										freeLblList(exthead);
										freeMemList(head);
										freeLblList(headOfLbl);
										fclose(source);
										report(ERR_MEM, ln);
										return ERR_MEM;
									}

									cop[WORDSIZE] = EOS; /* sign the end of the string */

										/* write the register's number to the string */
									for(i = 0; i < WORDSIZE; i++ ) cop[i] = '0'; /* utilized cells to '0' */
									for(i = 0; i < REGLEN; i++ ) { /* write the register number to the operand word */
										/* if there is only one operand, it should be written on the target register.
											if there are two operands, it should be written to the source register	*/
										gbw(cop, i + (lenargs == TWOARGS ? REGSCST : REGTGST)) = getbit(reg, i) + '0' ;
									}

									opcA(cop); /* set the ARE of the operand's word */

										/* add the word to the list */
									if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
										if ((tmp = addnext(wordnode, cop)) == NULL){
											freeall(line, args, cop,params, NULL);
											freeLblList(enthead);
											freeLblList(exthead);
											freeMemList(head);
											freeLblList(headOfLbl);
											fclose(source);
											report(ERR_MEM, ln);
											return ERR_MEM;
										}
										wordnode = tmp; /* change to the new node */
										ic++; /* count the new node */
									}

									twoParams++;
							 }
						 }
					   else {
						 if(adrParam2 == AC0){
							/* convert to string the number */
							if ((cop = itostr(atoi(secPar + 1))) == NULL){
								freeall(line, args,params, NULL);
								freeLblList(enthead);
								freeLblList(exthead);
								freeMemList(head);
								freeLblList(headOfLbl);
								fclose(source);
								report(ERR_MEM, ln);
								return ERR_MEM;
							}

							movel2(cop); /* move to make place for the ARE */
							opcA(cop); /* set the ARE */

							/* add the field's number to the list */
							if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
								if ((tmp = addnext(wordnode, cop)) == NULL){
									freeall(line, args, cop,params, NULL);
									freeLblList(enthead);
									freeLblList(exthead);
									freeMemList(head);
									freeLblList(headOfLbl);
									fclose(source);
									report(ERR_MEM, ln);
									return ERR_MEM;
								}

								wordnode = tmp; /* move to the new node */
								ic++; /* count the added node */
							}

							twoParams++;
						}
						else 
						if(adrParam2 == AC1){
								/* get the kind of label (if exists) */
							if((kind = getkindLbl(headOfLbl, secPar)) == NOTFOUND){
								freeall(line, args,params, NULL);
								report(status = ERR_ARGS_LBL, ln);
								Two_Par_stat = 1; /* change status of parameters */
								break; /* move to the next line */
							}
							cop = getcontentLbl(headOfLbl, secPar); /* get the labels address */

							/* check if the label is external */
							if(kind == GEXT){
								char *strIC; /* the place where the reference to the external label is */

								opcE(cop); /* set the ARE */

								/* cast the position to binary */
								if((strIC = itostr(MEM_STRT + ic)) == NULL){
									freeall(line, args,params, NULL);
									freeLblList(enthead);
									freeLblList(exthead);
									freeMemList(head);
									freeLblList(headOfLbl);
									fclose(source);
									report(ERR_MEM, ln);
									return ERR_MEM;	
								}

								/* add the label and position to the reference list */
								if ((extTemp = addnextlbl(extNode, secPar, strIC,kind)) == NULL){ /* check if added to the list */
									freeall(line, args, strIC,params, NULL);
									freeLblList(enthead);
									freeLblList(exthead);
									freeMemList(head);
									freeLblList(headOfLbl);
									fclose(source);
									report(ERR_MEM, ln);
									return ERR_MEM;
								}
								extNode = extTemp;

								free(strIC);
							} else {
								opcR(cop); /* set the ARE */
							}

							/* add label to the list */
							if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
								if ((tmp = addnext(wordnode, cop)) == NULL){ /* check if added to the list */
									freeall(line, args,params, NULL);
									freeLblList(enthead);
									freeLblList(exthead);
									freeMemList(head);
									freeLblList(headOfLbl);
									fclose(source);
									report(ERR_MEM, ln);
									return ERR_MEM;
								}

								wordnode = tmp; /* change to the next node */
								ic++; /* count the added node */
							}


							twoParams++;

						}

						else 
						{ /*adrParam2 == AC3*/
							cop = (char *)malloc((WORDSIZE + 1) * sizeof(char)); /* allocate new memory for the operand */
							reg = atoi(secPar + 1); /* get the register's number */
					
							/* check the memory allocate */
							if (cop == NULL){ 
								freeall(line, args,params, NULL);
								freeLblList(enthead);
								freeLblList(exthead);
								freeMemList(head);
						    freeLblList(headOfLbl);
								fclose(source);
								report(ERR_MEM, ln);
								return ERR_MEM;
							}
					
							cop[WORDSIZE] = EOS; /* sign the end of the string */
					
							/* write the register's number to the string */
							for(i = 0; i < WORDSIZE; i++ ) cop[i] = '0'; /* utilized cells to '0' */
							for(i = 0; i < REGLEN; i++ ) { /* write the register number to the operand word */
								/* if there is only one operand, it should be written on the target register.
									 if there are two operands, it should be written to the source register	*/
								gbw(cop, i + (lenargs == TWOARGS ? REGSCST : REGTGST)) = getbit(reg, i) + '0' ;
							}
					
							opcA(cop); /* set the ARE of the operand's word */
					
							/* add the word to the list */
							if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
								if ((tmp = addnext(wordnode, cop)) == NULL){
									freeall(line, args, cop, params,NULL);
									freeLblList(enthead);
									freeLblList(exthead);
									freeMemList(head);
						      freeLblList(headOfLbl);
									fclose(source);
									report(ERR_MEM, ln);
									return ERR_MEM;
								}
								wordnode = tmp; /* change to the new node */
								ic++; /* count the new node */
							}
				
							twoParams++;
						}
				                
				   }
				  }
				    	free(cop);
		            
							break;
				case AC3: /* the operand is a register */

					cop = (char *)malloc((WORDSIZE + 1) * sizeof(char)); /* allocate new memory for the operand */
					reg = atoi(fstArg + 1); /* get the register's number */
					
					/* check the memory allocate */
					if (cop == NULL){ 
						freeall(line, args, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
		        freeLblList(headOfLbl);
						fclose(source);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					cop[WORDSIZE] = EOS; /* sign the end of the string */
					
					/* write the register's number to the string */
					for(i = 0; i < WORDSIZE; i++ ) cop[i] = '0'; /* utilised cells to '0' */
					for(i = 0; i < REGLEN; i++ ) { /* write the register number to the operand word */
						/* if there is only one operand, it should be written on the target register.
						   if there are two operands, it should be written to the source register	*/
						gbw(cop, i + (lenargs == TWOARGS ? REGSCST : REGTGST)) = getbit(reg, i) + '0' ;
					}
					
					opcA(cop); /* set the ARE of the operand's word */
					
					/* add the word to the list */
					if (wordnode != NULL){ /* make sure it is on the operation word (it could be NULL if there was a previous error) */
						if ((tmp = addnext(wordnode, cop)) == NULL){
							freeall(line, args, cop, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
		          freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						wordnode = tmp; /* change to the new node */
						ic++; /* count the new node */
					}
					
					free(cop);

					break;
					
			} /* end of switch for first operand */

			/* check the addressing method of the second operand */ 

			switch(lenargs == TWOARGS ? adrtg : ACN){ /* if there are two arguments it is the target, otherwise check no one */
				case AC0: /* the operand is a number */
					op = atoi(secArg + 1); /* cast the number of the operand (the first character is HSH) */
					
					/* check if the number is too big or small */
					if (op < MINOPNUM || MAXOPNUM < op){
						freeall(line, args, NULL);
						report(status = ERR_LMT_DAN, ln);
						continue; /*move to the next line*/
					}
										
					/* convert the number to binary representation */
					if((cop = itostr(op)) == NULL){
						freeall(line, args, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
		       				 freeLblList(headOfLbl);
						fclose(source);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					movel2(cop); /* move to the left to set the ARE */
					opcA(cop); /* set the ARE */
					
					/* add the operand to the list */
					if (wordnode != NULL){ /* make sure it is on the operand word (it could be NULL if there was a previous error) */
						if((tmp = addnext(wordnode, cop)) == NULL){
							freeall(line, args, cop, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						wordnode = tmp; /* change to the new node */
						ic++; /* count the added operand word */
					}
					
					free(cop);
					
					break;
					
				case AC1: /* the operand is a label */
					/* check if label (operand) exists in the table */
																		

					if((kind = getkindLbl(headOfLbl, secArg)) == NOTFOUND){ 
						freeall(line, args, NULL);
						report(status = ERR_ARGS_LBL, ln);
						continue; /* move to the next line */
					}
					cop = getcontentLbl(headOfLbl, secArg); /* get content of the label */
					/* check if the label is external */
					if(kind == GEXT){
						char *strIC; /* the place where the reference to the external label is */
						
						opcE(cop); /* set the ARE */
						
						/* cast the position to binary */
					  if((strIC = itostr(MEM_STRT + ic)) == NULL){
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
		          				freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						
						/* add the label and position to the reference list */
						if((extTemp = addnextlbl(extNode, secArg, strIC,kind)) == NULL){
							freeall(line, args, strIC, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
							freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						extNode = extTemp;
						free(strIC);
					} else {
						opcR(cop); /* set the ARE */
					}
					
					/* add the operand to the list */
					if (wordnode != NULL){ /* make sure it is on the operand word (it could be NULL if there was a previous error) */
						if((tmp = addnext(wordnode, cop)) == NULL){ /* check if added to list */
							freeall(line, args, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
		          				freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}
						wordnode = tmp; /* change to the new node */
						ic++; /* count the added operand word */
					}
					free(cop);
					break;
					
				
					
				case AC3:
					cop = (char *)malloc((WORDSIZE + 1) * sizeof(char)); /* allocate new memory for the operand */
					reg = atoi(secArg + 1); /* get the register's number */
					
					/* check the memory allocate */
					if (cop == NULL){
						freeall(line, args, NULL);
						freeLblList(enthead);
						freeLblList(exthead);
						freeMemList(head);
						freeLblList(headOfLbl);
						fclose(source);
						report(ERR_MEM, ln);
						return ERR_MEM;
					}
					
					cop[WORDSIZE] = EOS; /* sign the end of the string */
					
					for(i = 0; i < WORDSIZE; i++ ) cop[i] = '0'; /* utilised cells to '0' */
					for(i = 0; i < REGLEN; i++ ) { /* write the register number to the operand word */
						/* write the register's number on the  target register */
						gbw(cop, i + REGTGST) = getbit(reg, i) + '0' ;
					}
					
					opcA(cop); /* set the ARE of the operand's word */
					
					/* add the word to the list */
					if (wordnode != NULL){ /* make sure it is on the operand word (it could be NULL if there was a previous error) */
						if ((tmp = addnext(wordnode, cop)) == NULL){
							freeall(line, args, cop, NULL);
							freeLblList(enthead);
							freeLblList(exthead);
							freeMemList(head);
		          				freeLblList(headOfLbl);
							fclose(source);
							report(ERR_MEM, ln);
							return ERR_MEM;
						}

						wordnode = tmp; /* change to the new node */
						ic++; /* count the new node */
					}
					
					free(cop);
					
					break;
					
			} /* end of switch for second operand */	

		} /* end of else (the operands' addressings checks)*/

		/* move to the next instruction word */
		if (wordnode != NULL){ /* make sure it is on the operand word (it could be NULL if there was a previous error) */
			wordnode = wordnode->next;
		}
		if(Two_Par_stat == 0) /* if we haven't freed them yet */
		freeall(line, args, NULL);
	} /* end of while (scan the source file) */
	
	freeLblList(headOfLbl);



	/* close the source file */
	if (fclose(source) == EOF){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		report(ERR_FCE, ln);
		return ERR_FCE;
	}
	
	/* check if there was an error - not printing to the output file */
	if(status != SUCC){ 
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		return status;
	}

	/* update the data-counter */
	if (wordnode != NULL){ /* check if there is any data */
		dc++; /* count the current node */
		while((wordnode = wordnode -> next) != NULL) dc++; /* count the nodes until NULL */
	}
	

	/* the name of the object file */
	if ((fullName = addstr(nameOfFile, EOB)) == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		report(ERR_MEM, ln);
		return ERR_MEM;
	}
	
	/* create\open the object file */
	if ((obj = fopen(fullName,"w")) == NULL){
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		free(fullName);
		report(ERR_FOE, ln);
		return ERR_FOE;
	}
	
	free(fullName);
	
	/* print the memory list to the object file */
	if ((res = printMemList(obj, head, ic, dc)) != SUCC){
		fclose(obj);
		freeLblList(enthead);
		freeLblList(exthead);
		freeMemList(head);
		fclose(obj);
		report(res, ln);
		return res;
	}
	
	freeMemList(head);
	
	/* close the object file */
	if (fclose(obj) == EOF){
		freeLblList(enthead);
		freeLblList(exthead);
		report(ERR_FCE, ln);
		return ERR_FCE;
	}
	
	if(countNodes(enthead) > 1){ /* if there is at least one extern statement */
		/* the name of the entry file */
		if ((fullName = addstr(nameOfFile, ENT)) == NULL){
			freeLblList(enthead);
			freeLblList(exthead);
			report(ERR_MEM ,ln);
			return ERR_MEM;
		}

		/* open\create the entry file */
		if ((ent = fopen(fullName,"w")) == NULL){
			freeLblList(enthead);
			freeLblList(exthead);
			free(fullName);
			report(ERR_FOE, ln);
			return ERR_FOE;
		}

		free(fullName);
		/* print the entry list to the entry file (enthead->next  to skip the dummy head) */
		if((res = printLblList(ent, enthead->next)) != SUCC){
			freeLblList(enthead);
			freeLblList(exthead);
			fclose(ent);
			report(res, ln);
			return res;
		}


		/* close the rntry file */
		if (fclose(ent) == EOF){
			freeLblList(exthead);
			report(ERR_FCE, ln);
			return ERR_FCE;
		}
	}
	freeLblList(enthead);

	if(countNodes(exthead) > 1){ /* if there is at least one extern statement */
		/* the name of the extern file */
		if ((fullName = addstr(nameOfFile, EXT)) == NULL){
			freeLblList(exthead);
			report(ERR_MEM ,ln);
			return ERR_MEM;
		}

		/* open\create the extern file */
		if ((ext = fopen(fullName,"w")) == NULL){
			free(fullName);
			freeLblList(exthead);
			report(ERR_FOE, ln);
			return ERR_FOE;
		}

		free(fullName);

		/* print the external list to the extern file (exthead->next  to skip the dummy head) */
		if ((res = printLblList(ext, exthead->next)) != SUCC) {
			freeLblList(exthead);
			fclose(ext);
			report(res, ln);
			return res;
		}


		/* close the extern file */
		if (fclose(ext) == EOF){
			report(ERR_FCE, ln);
			return ERR_FCE;
		} 
	
	}
	freeLblList(exthead);

	return SUCC;
} /* end of pass2 */



