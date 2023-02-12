/*
	macros and functions to read\write from\to the input\output files.
	also, definition of a string and constants for special characters.
*/

#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* header for the macros of the pre-assembler functions	*/

#define MAC "mcr" /* the macro declaration keyword */
#define EMAC "endmcr" /* end of macro declaration keyword */


#define write2str(fp, str1, str2) fprintf(fp, "%s %s", str1, str2) /* write a string to the given file in the format "str1 str2" */


/* adds 2 strings together, free the previous str1 and put the new string in str1 */
#define addfree(str1, str2) {										\
								char *temp = addstr(str1, str2);	\
								free(str1);							\
								str1 = temp;						\
							}
#define MAXLEN 80	/* max line length */

#define EAS ".as"	/* the input file */
#define EAM ".am"	/* the input file after pre-assembler */
#define EOB ".ob"	/* the file after convertion to 32bit format */
#define ENT ".ent"	/* list of labels that are allowed to use in other files */
#define EXT ".ext"	/* list of labels requested to use from other files */

#define NLN '\n'	/* new line */
#define EOS '\0'	/* end of string */
#define BLK ' '		/* blank - separate fields */
#define TAB '\t'	/* tab - separate fields */
#define COM ','		/* coma - separate operands */
#define DOT '.'		/* dot - the prefix of a guide and the field specifier of a struct */
#define HSH '#'		/* hashtag - the start of a number (as operand of instruction) */
#define COL ':'		/* colon - the end of a label */
#define SCO ';'		/* semicolon - the start of a comment line */
#define QUM '\"'	/* quotation marks - the start and end of a string */
#define ESTR ""		/* empty string */


#define REGSIZE 8 /* the number of registers */



/*	read a line from fp.
	replace the last character (EOF or NLN) with EOS and put
	the character where lc points to.
	any TAB will be coverted to BLK and consecutive BLK will
	be deleted leaving only one.
	all the white characters at the end of the line will be deleted.
	return NULL if there isn't enough space to store the line.
	when the string is not needed anymore, use
	the "free" function from stdlib.h on the pointer.	 			*/
char *readline(FILE *fp, char *lc, int *len);


/*	writes a line to the given file.
	if the string doesn't ends with NLN, the funtion
	will add NLN to the end.
	there may be NLN any where in the string and all of the
	string will be written.
	if the string is empty (has only the character EOS), the 
	funtion does nothing.													*/
int writeline(FILE *fp, char *str);


char *isreg(char *str);


	
	typedef struct {
	char *name; /* instruction's name */
	unsigned int opcode; /*the instruction's code */
} instruct;

	instruct *findinst(char *str);

typedef struct {
	char *name; /* guide's name */
} guide;

guide *findguide(char *str);

	typedef struct macro{
    char *name;
    char *content;
    struct macro *next;
}newMacro;

 	int addMcr(newMacro *head,char *newName,char *newContent);
 	char *addstr(char *str1, char *str2);
 	char *getcontent(struct macro *head, char *nameOfMacro);
 	char *ismacrodec(struct macro *head, char *str);
 	void freeList(newMacro *head);


#endif

