
typedef struct macro{ /* define list of macros and their data */
    char *name; /* the name of the macro */
    char *content; /* the content of the macro */
    struct macro *next; /* the next node */
}newMacro;

	/*  get a string and check if it is a valid string declaration:
	check if the string is of the following format "macro NAME".
	The NAME is not a label, instruction, guide or register.
	there must be BLK only between every two "fields" and there
	can't be consecutive BLK.
	if it is a valid declaration, return a pointer to the start of the
	name in the string. otherwise, return NULL. */

char *ismacrodec(newMacro *head, char *str);

/*	create a new "newMacro" node with the given name and content,
	return SUCC. if couldn't allocate memory
	for the new node, return ERR_MEM.	 */
int addMcr(newMacro *head,char *newName,char *newContent);


/*	search in the table for the content with a name matching
	and return the content.
	if couldn't find anything, return NULL.		*/
char *getcontent(newMacro *head, char *nameOfMacro);
 
/* free the memory held by the given list(linked list of macro). */
void freeList(newMacro *head);
