
#include <stdio.h>
#define LBLLENMAX 30 /* the max amount of chars label's name can have */

#define REGSIZE 8 /* the number of registers */
#define BINARYBASE 2 /* the binary base */
#define DECBASE 10 /* the decimal base */
#define ONEARGS 1 /* only one argument in the current line */
#define TWOARGS 2 /* two argument in the current line */
#define TWO_REG_PARAM -2 /* to indicate if we get two registers for parameters in address method 2*/
/* return the bit in position "bit" of the word (from right to left) */
#define gbw(str, bit) ((str)[(WORDSIZE - 1) - (bit)])


/* get a number and the bit that should be accessed, assume the bit is non-negative and that the bit is smaller then the number of bits in the type of num */
#define getbit(num, bit) (((num) & powr(BINARYBASE, (bit))) ? 1 : 0)


/* adds 2 strings together, free the previous str1 and put the new string in str1 */
#define addfree(str1, str2) {										\
								char *temp = addstr(str1, str2);	\
								free(str1);				\
								str1 = temp;				\
							}
							
/* get a string of "WORDSIZE" length and move all the characters to the left to make place for the ARE bits (the characters from position 0 to ARELEN-1 are discarded) */
#define movel2(str) {int i; for (i = ARELEN; i < WORDSIZE; i++) str[i - ARELEN] = str[i];}



extern char *regs[]; /* declare the register array */

 	
/*	duplicate the first field of the given string (pls free the memory with
	the "free" function from "stdlib.h").
	if It couldn't allocate memory for the new string, return NULL.	
*/
char *dupl(char *str);


/*	duplicate a 10-bit word (string).
	is the str is NULL, the function return an empty
	string (that should be freed later).
	free the memory with the "free" function from "stdlib.h".
	if It couldn't allocate memory for the new string, return NULL.	
*/

char *duplw(char *str);


/*	duplicate the  given string (pls free the memory with
	the "free" function from "stdlin.h").
	if It couldn't allocate memory for the new string, return NULL.
*/

char *duplstr(char *str);


/*	skip field in given line.
	assume there is only one BLK between fields and nowhere else.
	return pointer to the start of the next field.
	if there is only one field, return pointer to the end of the string (EOS) 
*/

char *skipfd(char *str);


/*	count chars until COM, BLK or EOS
	return the length of the field.		
*/

int sizeoffd(char *str);


/*		split a line of command
		check the command and put a pointer to it in "command".
		put pointers to the arguments in "args" in the same order as in the line (adds '\0' to the end of each one).
		the function supports strings (the first character of an operand must be QUM for it to be treated as string).
		put the number of arguments in "lenargs".
		In any case, use the "free" function from stdlib.h on "args" when finished using it.
	assume
		there is at least one character in the line.
		the line doesn't start with blank
		the line doesn't contain tabs.
	return
		ERR_MEM			couldn't finish the task du to memory shortage.
		ERR_OPT			unknown command name.
		ERR_COM_IL		an illegal comma has been found.
		ERR_COM_MU		found multiple consecutive commas.
		ERR_COM_MI		There is a missing comma.
		ERR_OPD			a string wasn't properly closed.
		GTYP			finished successfully and found a guide type.
		ITYP			finished successfully and found the instruction type.
	warning
		the function changes the content of the given string.          
*/

int splitline(char *line, void **command, char * **args, int *lenargs);


/*	add two strings together and return the new string.
	return NULL if couldn't allocate memory for the
	new string.
	when finished using the string call the "free"
	function from stdlib.h in order to free the memory.		
*/

char *addstr(char *, char *);



/*	check if the given string is a register.
	search legal register name syntax.
	returns NULL if the name.
	return pointer to string that is equal to the name of the register.
*/

char *isreg(char *str);

/*	get int and returns its binary form (in two's complement) as a string.
	the string has 14 characters and any bit that is after the 14th bit
	will be ignored.
	if It couldn't allocate memory, return NULL.		
*/

char *itostr(int num); 


/*	search for addressing method for given operand
	based on the operand's syntax.
	return the legal addressing method (AC0, AC1, AC2, AC3)
	or ERR_OPD if illegal operand.			
*/
int findadr(char *str);


/* checks whether the operand is in the format of an operand of addressing method 2*/

int isAdr2(char *str);

/*	check if given string is label.
	check for forbidden names as reg, guide or instruction name.
	return null if use forbidden name.
	return ptr to string that is a copy of the label.		
*/

char *islbl (char *line);

/*	return x to the power of y.
	if y is non-positive, return 1.
*/

int powr(int x, int y);

/*	creates a array of strings So that we can mess with the values inside the brackets	*/

char** getParams(char *str);

/*	removing all of the white spaces in the string	*/

int removeWhiteSpace (char *str);

/*	convert string that represent binary number to special base 2.
	assume the string has 14 characters.
	return NULL if can't allocate memory.
	return pointer to the converted string if suceeded.		
*/

char *convert2(char *inputStr);


/*	free all given pointers.
	pointers has to point to memory allocated by calloc\malloc.						
	the last parameters has to be NULL.				
*/

void freeall (void * ptr, ...);


/*	fclose all given FILE pointers.						
	the file has to be opened with fopen.
	even if couldn't close a file, the function 
	will try to close the next FILE pointers.
	return SUCC if succeded to close all
	and ERR_FCE if couldn't. 			
*/

int fcloseall (FILE * ptr, ...);

/*
	Starting from the end
	If the char we are standing on is a space, we erase it.
	When we arrived to the last space in the String
	Well change it to a '\0' so the computer will know its the end of the String 
*/


void trimEnd(char *str);



