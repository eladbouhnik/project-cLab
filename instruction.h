/*
	definition of an instruction, and macros related to instructions.
*/

#ifndef INSTRUCT
#define INSTRUCT


#include "memoryOpr.h"


#define MAXOP 2 /* the maximum number of operands */

/* for the instructions that allow numbers as operands */
#define MAXOPNUM 2047 /* the highest signed integer that can be held in 12 bits: 2^11 - 1 */
#define MINOPNUM -2048  /* the lowest signed integer that can be held in 12 bits: -2^11 */

/*	count the number of operands the instruction ip can get and put the number in counter.
	ip is a pointer to the instruct */
#define opcount(counter, ip)	{									\
									counter = 0;			\
									if (ip->opsc != AN) counter++;	\
									if (ip->optg != AN) counter++;	\
								}

#define ITYP -1 /* flag to identify instruction */

#define ADSNUM 4 /* the number of addressing methods */

/* flags to sign what kind of addressing each operand in each instruction can be */
#define AN   0 /* no operand */
#define A0   1 /* can have a number as operand */
#define A1   2 /* can have a label as operand */
#define A2   4 /* can have a operand of addressing method 2 */
#define A3   8 /* can have a register as operand */
#define ACN -1 /* the addressing wasn't set */
#define AC0  0 /* the code of the A0 or there is no operand */
#define AC1  1 /* the code of the A1 */
#define AC2  2 /* the code of the A2 */
#define AC3  3 /* the code of the A3 */


#define INSTSIZE 16

/* define instructions and make an array of them, sorted alphabetically by their names */
typedef struct {
	char *name; /* instruction's name */
	unsigned int opcode : OPCLEN; /* the instruction's code */
	unsigned int opsc : ADSNUM; /* addressing options for the source operand (each bit signal a specific addressing that is available) */
	unsigned int optg : ADSNUM; /* addressing options for the target operand (each bit signal a specific addressing that is available) */
	unsigned int opParam1 : ADSNUM; /* addressing options for the first parameter of the operand (each bit signal a specific addressing that is available) */
	unsigned int opParam2 : ADSNUM; /* addressing options for the second parameter of the operand (each bit signal a specific addressing that is available) */
} instruct;



/*	function that perform binary search to look for instruction and
	in the "insts".
	assume they are sorted alphabetically by names.
	return a pointer to the ones with matching names.
	if couldn't find any, return NULL				*/

instruct *findinst(char *str);

extern instruct insts[];
#endif




