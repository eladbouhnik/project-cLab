#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "utils.h"
#include "guides.h"
#include "instruction.h"
#include "ioFuncs.h"
#include "error.h"

char *dupl(char *str){
	char *nstr = malloc((sizeoffd(str) + 1) * sizeof(char)); /* allocate space for the new string */
	char *ptr = nstr; /* pointer to set the Characters of the new string */
	
	/* check the allocation */
	isnull(nstr, NULL);
	
	/* copy-paste all the characters of the string */
	while ((*ptr = *str) != EOS && *ptr != BLK ) {ptr++; str++;}
	
	*ptr = EOS; /* sign the end of the string */
	
	return nstr;
}


char *duplw(char *str){
	/* allocate space for the new string (if the string is NULL, allocate memory only for the EOS character) */
	char *nstr = malloc((str == NULL ? 1 : (WORDSIZE + 1)) * sizeof(char)); 
	int i = 0;
	
	/* check the allocation */
	isnull(nstr, NULL);
	
	/* check if the string is not NULL */
	if (str != NULL){
		/* copy-paste all the characters of the string */
		for (; i < WORDSIZE; i++) nstr[i] = str[i];
	}
	
	nstr[i] = EOS; /* sign the end of the string */
	
	return nstr;
}


char *duplstr(char *str){
	char *nstr = malloc((strlen(str) + 1) * sizeof(char)); /* allocate space for the new string */
	char *ptr = nstr; /* pointer to set the charatcres of the new string */
	
	/* check the allocation */
	isnull(nstr, NULL);
	
	/* copy-paste all the characters of the string */
	while ((*ptr++ = *str++) != EOS);
	
	return nstr;
}



int sizeoffd(char *str){
	int i;
	
	/* skip characters until BLK or EOS or COM */
	for (i = 0; str[i] != BLK && str[i] != EOS && str[i] != COM; i++);
	
	return i;
}


char *skipfd(char *str){
	/* look for the end of the field */
	while (*str != EOS && *str != BLK) str++;
	
	/* check if there is another field */
	return *str == BLK ? str + 1 : str; /* if *str is BLK, return pointer to the first character of the second field, otherwise it is pointer to EOS */
}


int splitline(char *line, void **command, char * **args, int *lenargs){
	char *ptrl = line; /* pointer to scan the line */
	int endf = 0; /* flag to sign that the end of the line aready have been reached */
	*lenargs = 0; /* reset the counter of the arguments */
	*args = NULL;
	
	/* scan first field of the line */
	while (*ptrl != EOS && *ptrl != BLK){
		if (*ptrl++ == COM){ /* check for illegal comma */
			return  ERR_COM_IL;
		}
	}
	
	/* sign the end of the instruction (the first word) */
	if (*ptrl != EOS){
		*ptrl++ = EOS;
	} else { /* (*ptrl == EOS) */
		endf++; /* change the flag to remember that the end of the line have been reached */
	}
	
	/* check if there is an illegal comma between the instruction and the first argument */
	while (*ptrl == COM || *ptrl == BLK){
		if (*ptrl++ == COM){
			return  ERR_COM_IL;
		}
	}
	
	/* allocating space for the first argument */
	if (*ptrl != EOS){
		*args = (char **) malloc(sizeof(char *));
		
		/* check the allocation */
		if (*args == NULL){
			return ERR_MEM;
		}
	} else { /* (*ptrl == EOS) */
		endf++; /* change the flag to remember that the end of the line has been reached */
	}
	
	/* scanning the line for arguments until the end of the line */
	while (!endf){
		/* allocate space for another argument in args */
		char **temp = (char **) realloc(*args, ++(*lenargs) * sizeof(char *));
				
		/* check the allocation */
		if (temp == NULL){
			return ERR_MEM;
		}
		
		*args = temp;
		
		(*args)[*lenargs - 1] = ptrl; /* set the current last argument in args */
		
		/* check if the current argument is a string */
		if (*ptrl == QUM){
			ptrl++; /* move to the first character of the string */
			
			/* skip all the characters of the string */
			while(*ptrl != QUM && *ptrl != EOS) ptrl++;
			
			/* check if found the end of the string wasn't found */
			if (*ptrl != QUM){
				return ERR_OPD; /* the string wasn't closed properly */
			}
			
			ptrl++; /* check the character after the QUM */
			if (*ptrl != BLK && *ptrl != COM && *ptrl != EOS){
				return ERR_OPD;
			}
		}
		else if(isAdr2(ptrl))
		    while(*ptrl != EOS) ptrl++;
		
		else { /* regular argument - not string*/
			/* skip all the characters of the argument */
			if(isAdr2(ptrl))
			    while(*ptrl != BLK && *ptrl != EOS ){
				    ptrl++;
			    }
		    else
			    while(*ptrl != BLK && *ptrl != COM && *ptrl != EOS ){
			      ptrl++;
			    }
		}
		
		
		/* check the next character after the argument (*ptrl is BLK or COM or EOS) */
		if (*ptrl == EOS){ /* reached end of line */
			endf++;
		} else if (*ptrl == BLK){
			/* sign the end of the argument */
			*ptrl++ = EOS;
			
			/* skip all the blanks */
			while (*ptrl == BLK){
				ptrl++;
			}
			
			/* check the current character (in a normal case it should be a comma or EOS) */
			if (*ptrl == EOS){ /* reached end of line */
				endf++;
			} else if (*ptrl != COM){ /* some other character */
				return ERR_COM_MI;
			}
		}
		
		/* now the character is COM or EOS */
		
		if (*ptrl == COM){
			*ptrl++ = EOS; /* sign end of argument (won't impact anything if the argument already ended) */
		
			/* skip all the blanks */
			while (*ptrl == BLK) {
				ptrl++;
			}
		
			/* in a normal case, *strl should be now the start of a new argument (a character different from COM and EOS) */
			if (*ptrl == COM){ /* check consecutive commas */
				return ERR_COM_MU;
			} else if (*ptrl == EOS){ /* the line ended instead of having a next argument */
				return ERR_EXR;  /*the comma is an extraneous text */
			}
		}
	}
	
	/* search for the instruction (at the end of the first word there is EOS) */
	*command = (void*)findinst(line);
	
	/* check if the instruction wasn't found */
	if (*command == NULL){
		/* check if it is a guide */
		if (*line == DOT && (*command = (void *)findguide(line + 1)) != NULL){
			return GTYP;
		}
		
		/* not an instruction or guide */
		return ERR_OPT;
	}
	
	return ITYP;
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

	
char *isreg(char *str){
	/* binary search on "regs" */
	char *regs[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};
	int low = 0, high = REGSIZE - 1;  /*the limits of the array */
	char *reg = NULL;  /*the register with the same name */
	int i;
	char lc; /* the character after the first field in the line (EOS or BLK) */
	
	 /*scan for the first character after the first field */
	for (i = 0; str[i] != BLK && str[i] != EOS && str[i] != COM; i++);
	
	 /*replace the character with EOS */
	lc = str[i];
	str[i] = EOS;
	
	 /*binary search on regs */
	while (low <= high && reg == NULL){
	/*	 the middle of the searched area */
		int mid = low + (high - low)/2;  /*a form of averag that doesn't go beyong high in the calculation (to prevent runtime errors) */
		int cmp = strcmp(str, regs[mid]);  /*the relation between the strings (<,>,=) */
		
		if (cmp < 0){
			high = mid - 1;
		} else if (0 < cmp){
			low = mid + 1;
		} else {
			reg = regs[mid];
		}
	}
	
	str[i] = lc; /* binary search on insts */
	
	return reg;
}



char *itostr(int num){
	char *numstr = (char *)malloc((WORDSIZE + 1) * sizeof(char)); /* allocate space for the string */
	int absn = (num < 0) ? -num : num; /* get the absolut value of num */
	int carry = (num < 0) ? 1 : 0; /* the number to add to the next bit (1 in case of negative number) */
	int i;
	
	isnull(numstr, NULL); /* check the allocation */
	
	/* set all the characters in the string */
	for (i = 0; i < WORDSIZE; i++){
		/* set the i-th character (from right to left) of the string to '1' or '0' based on the current bit of the number
		   and flip it if the original number was negative.									*/
		gbw(numstr, i) = (num < 0) ? (('0' + absn % BINARYBASE == '1') ? '0' : '1') : ('0' + absn % BINARYBASE);
		
		/* get read of the last significant bit (without being dependent on how the system represent the number) */
		absn /= BINARYBASE;
		
		/* add 1 to the current bit if the carry is 1, if the bit was 1, the carry stays 1, otherwise it stays 0 */
		if (carry == 1){
			if (gbw(numstr, i) == '1'){
				gbw(numstr, i) = '0';
				carry = 1;
			} else { /* gbw(numstr, i) == '0' */
				gbw(numstr, i) = '1';
				carry = 0;
			}
		}
	}
	
	/* sign the end of the string */
	numstr[WORDSIZE] = EOS;
	
	return numstr;
}

int isAdr2(char *str){
	char *ptr = str;
        int count = 0; /* reset count to 0 */
        while (*ptr != EOS && *ptr != OPEN_BRAC){  /* while the character at ptr is not the end of string character or open brackts */
		count++;
        	ptr++; /* move ptr to the next character */
	}
	if(count == 0 || (*ptr == EOS)) /* illegal structure of addressing method 2 */
		return 0;
	ptr++; /* move ptr to the next character */
	count = 0; /* reset count to 0 */
	while (*ptr != EOS && *ptr != COM){ /* while the character at ptr is not the end of string character or comma */
		count++;
		ptr++; /* move ptr to the next character */
	}
		if(count == 0 || (*ptr == EOS)) /* illegal structure of addressing method 2 */
		     return 0;
		 ptr++; /* move ptr to the next character */
		 count = 0;   /* reset count to 0 */
		 
		 while (*ptr != EOS && *ptr != CLOSE_BRAC){
		 	count++;
		   	ptr++; /* move ptr to the next character */
		 }
		 if(count == 0 || (*ptr == EOS )) /* illegal structure of addressing method 2 */
		 	return 0;
		 return 1;
  }

char *islbl(char *line){
	char *lbl; /* the duplicated label */
	char lc; /* the character after the first field in the line (EOS or BLK) */
	int i;
	
	/* check if the first character is alphabet */
	if (!isalpha(*line)) return NULL;
	
	/* count the number of characters in the label, and check that all of them are alpha-numeric */
	for (i = 0; line[i] != COL && line[i] != BLK && line[i] != EOS; i++){
		if (!isalnum(line[i])){ /* check the character */
			return NULL;
		}
	}
	
	/* check that the last character in the field is COL */
	if (line[i] != COL || (line[i + 1] != BLK && line[i + 1] != EOS)){
		return NULL;
	}
	
	 /* save the first character after the first field and replace it with EOS*/
	lc = line[i];
	line[i] = EOS;
	
	/* chek if the label is a name of guide */
	if (findguide(line) != NULL){
		line[i] = lc; /* put the character back */
		return NULL;
	}
	
	/* check if the label is a name of instruction */
	if (findinst(line) != NULL){
		line[i] = lc; /* put the character back */
		return NULL;
	}
	
	/* chek if the label is a name of register */
	if (isreg(line) != NULL){
		line[i] = lc; /* put the character back */
		return NULL;
	}
	
	/* duplicate the label */
	lbl = dupl(line);
	
	line[i] = lc; /* put the character back */
	
	return lbl;
}

int findadr(char *str){
	char *ptr = str; /* pointer to scan the string */
	char lc; /* the last character of the current field */
	char *lcptr; /* pointer to the first charcter after the first field */
	int twoLegalParam = 0;
	char *token; /* for strtok*/
	char *strCopy;
	int twoRegParam = 0;
	/* scan for the first character after the first field */
	while (*ptr == BLK && *ptr != EOS) ptr++;

	lcptr = ptr; /* save the position of the character */
	lc = *ptr; /* save the character */
	
	/* check the addressing */
	if (*ptr == HSH && *(ptr + 1) != EOS){ /* check for number operand */
		char *suff; /* suffix for the convetion test of the number */
		
		/* check if can't convert to integer because of garbage characters */
		strtol(ptr + 1, &suff, DECBASE);
		
		*lcptr = lc; /* return the character from the start */
		
		/* check if there are garbage characters */
		if (*suff != EOS){
			return ERR_OPD;
		}
		
		return AC0;
	} else if (isreg(ptr) != NULL){ /* check for register */
			*lcptr = lc; /* return the character from the start */
			return AC3;
		}	
	 	else { /* label or lable to jump into */
	
			/* check if it is a valid name of label */
			/* label name starts with alphabet character */
			if (!isalpha(*ptr)){
				*lcptr = lc; /* return the character from the start */
				return ERR_OPD;
			}
		
			/* all the charatcres in label name are alphanumeric */
			while (*ptr != EOS && *ptr != OPEN_BRAC){ 
				if (!isalnum(*ptr++)){
					*lcptr = lc; /* return the character from the start */
					return ERR_OPD;
				}
			}
		
			/* check if the string has only alphanumeric characters and the first is alphabet - it is a label */
			if (*ptr == EOS && *ptr != OPEN_BRAC){
				*lcptr = lc; /* return the character from the start */
				return AC1;
			}
	
			ptr++; /* we get into the brackets and check if it is operand of addressing method of jump with parameters */
			if(*ptr != EOS){
				if((strCopy = duplstr(ptr))==NULL){
					free(strCopy);
					return ERR_MEM;
				}
				removeWhiteSpace(strCopy);
				token = strtok(strCopy,",");
				while(token != NULL && twoLegalParam != 2){
					if(twoLegalParam == 1){
						token = strtok(NULL,")");
						 if(token == NULL)
					 		return ERR_MEM;
					}
					while (*token == BLK && *token != EOS) token++;/* scan for the first character after the first field */
		
					if (*token == HSH && *(token + 1) != EOS){ /* check for number operand */
						char *suff; /* suffix for the convetion test of the number */
		
						/* check if can't convert to integer because of garbage characters */
						strtol(token + 1, &suff, DECBASE);
		
						*lcptr = lc; /* return the character from the start */
		
						/* check if there are garbage characters */
						if (*suff != EOS){
							return ERR_OPD;
						}
						twoLegalParam++; /* legal param*/
						continue;
					}
					else
					if (isreg(token) != NULL){
						twoLegalParam++;
						twoRegParam++;
						continue;
					}
					else{
					if (!isalpha(*token)){ /* label name starts with alphabet character */
						*lcptr = lc; /* return the character from the start */
						return ERR_OPD;
		 			} 
		
					/* all the characters in label name are alphanumeric */
						while (*token != EOS){ 
							if (!isalnum(*token++)){
								*lcptr = lc; /* return the character from the start */
								return ERR_OPD;
							}
						}
						twoLegalParam++;
						continue;
				}
			
					*lcptr = lc; /* return the character from the start */
					return ERR_OPD;
		
			 }
				if(twoLegalParam !=2 && token == NULL)
					return ERR_COM_MI;
				else{
					token = strtok(NULL,"\n");
					if(token != NULL){
						while(*token != EOS){
			  			if(!isspace(*token))
				 				return  ERR_ARGS_NUM;
				 			token++;
						}	
					}
					if(twoRegParam == 2)
						return TWO_REG_PARAM;
					return AC2;
				}
			}
			return ERR_OPD;
		}
	}
		
char *convert2(char *inputStr)
 {
     /* WORDSIZE is the number of special base 2 characters needed */
    int len = WORDSIZE,i; 	 /* numer of characters in the new string (without EOS) */ 
    char *convString = (char *) malloc(sizeof(char) * (len + 1)); /* allocating memory for the the converted number */
    isnull(convString, NULL);  /*check the allocataion */
    
    for ( i = 0; i < len; i++) { /* scan the "inputStr" */
        if (inputStr[i] == '1') 
            convString[i] = '/';
        else
         convString[i] = '.';
        
    }
    
    convString[len] = EOS; /* sign the end of the string */
    return convString;
}



void freeall(void *ptr, ...){
	va_list argptr;
	va_start(argptr, ptr);
	
	/* scan the arguments until NULL */
	while (ptr != NULL) {
		free(ptr); /* free the pointer */
		ptr = va_arg(argptr, void *); /* continue to the next pointer */
	}
	
	va_end(argptr);
	return;
}


int fcloseall(FILE *ptr, ...){
	char status = SUCC; /* fclose return EOF if couldn't close the file and zero (which is equal to SUCC) overwise */
	
	va_list argptr;
	va_start(argptr, ptr);
	
	/* scan the arguments until NULL */
	while (ptr != NULL) {
		/* check if all the previous files closed successfully */
		if (status == SUCC){
			status = fclose(ptr); /* return 0 (which is equal to SUCC) in when succeded */
		} else {
			fclose(ptr);
		}
		
		/* continue to the next argument */
		ptr = va_arg(argptr, FILE *);
	}
	
	va_end(argptr);
	
	/* check if succeded to close all the files */
	return status == EOF ? ERR_FCE : SUCC;
}



int powr(int x, int y){
	int ans = 1; /* the resault */
	while (y-- != 0) ans *= x; /* mutiply the "ans" by x, y times */
	return ans;
}

char** getParams(char* str) {
    char** params; 
    char* start;
    char* end; 
    char *copy;
    copy = (char*) malloc(sizeof(char)*(strlen(str)+1)); /* allocate memory for a copy of the string*/
    strcpy(copy,str);
    removeWhiteSpace(copy);
   params = (char**) malloc(sizeof(char*)*2); /* allocate memory for two strings*/
    start = strchr(copy, '('); /* find the opening parenthesis*/
    end = strchr(copy, ')'); /*find the closing parenthesis*/
    if (start && end) {
    		char* comma ;
        start++; /* advance the start pointer past the opening parenthesis*/
        *end = '\0'; /*replace the closing parenthesis with a null character to terminate the string*/
        comma = strchr(start, ','); /* find the comma delimiter*/
        if (comma) {
            *comma = '\0'; /*replace the comma with a null character to terminate the first string*/
            params[0] = (char*) malloc(sizeof(char)*(strlen(start)+1)); /* allocate memory for the first string*/
            strcpy(params[0], start); /* copy the first string to the allocated memory*/
            comma++; /* advance the comma pointer past the null character*/
            params[1] = (char*) malloc(sizeof(char)*(strlen(comma)+1)); /* allocate memory for the second string*/
            strcpy(params[1], comma); /* copy the second string to the allocated memory*/
        }
    }
    return params; /* return the array of strings*/
}


int removeWhiteSpace (char *str){
  	int i,j;
	  for(i = 0;str[i] != '\0';i++)
			if(str[i] == ' ' || str[i] == '\t' || str[i] == '\n'){ /*if the current character is whitespace*/
				for(j = i;str[j] != '\0'; j++)  /*remove current character*/
					str[j] = str[j + 1];
			  i = -1;	/* preparing to check in from the beginning */
		  }
		return 1;
  }
  
void trimEnd(char *str) {
	int len;
    	len = strlen(str);    /* Get the length of the string.*/
	/* starting from the end of the string, remove any whitespace characters */
    	/* until a non-whitespace character is encountered. */
    	while (len > 0 && isspace(str[len - 1])) {
        	len--;
    	}
	/* Replace the first whitespace character (if any) after the end of the
        actual string with the null terminator character.*/
    	str[len] = EOS;
}





