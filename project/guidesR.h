/*
	definition of a guide, and constants related to guides.
*/

#ifndef GUIDES
#define GUIDES




/* for the data allocation guides which has integers has operands (struct and data) */
#define MAXDTNUM 8191  /* the highest signed integer that can be held in 14 bits: 2^9 - 1 */
#define MINDTNUM -8192  /* the lowest signed integer that can be held in 14 bits: -2^13 */


/* constants for the guides */
#define GNUMOP 2 /* the maximum number of operands for guide (when the guide can't have infite number of operands) */
#define GSTCFN 2 /* number of fields in struct */

#define GNM -1 /* no maximum number of operands */

#define GNDEF 0 /* the guide doesn't have a default type for all the operands or the array isn't used*/
#define STRTYP 1 /* the operand is string. */
#define INTTYP 2 /* the operand is integer. */
#define LBLTYP 3 /* the operand is label. */

#define GALL 1  /* the guide is a data (numbers or string) allocation, and also operate as a flag in the guide's kind field */
#define GEXT 2  /* the guide is a declaration about a label from other file */
#define GENT 4  /* the guide is a declaration that other guides can use a label */

#define GTYP -2 /* flag tp identify guide */


/* define guide and make an array of them, sorted alphabetically by their names */
typedef struct {
	char *name; /* guide's name */
	int opmin; /* the minimum number of operands */
	int opmax; /* the maximum number of operands - GNM for no maximum*/
	int kind; /* what kind of guide is it (data allocator or label declaration) */
	int deftype; /* if all the operands have the same type set this to the typeo (STRTYP or INTTYP), overwise it should be GNDEF */
	int optypes[GNUMOP]; /* an array to specify the types of the operands when "deftypes" is GNDEF (and there are two operands at most) */
} guide;


#define NUM_OF_GUIDES 4 /* the number of guides */
extern guide guids[]; /* array of the guides */


/*	function that perform binary search to look for guide and
	in the "guids".
	assume they are sorted alphabetically by names.
	return a pointer to the ones with matching name.
	if couldn't find any, return NULL.								*/
guide *findguide(char *name);

#endif

