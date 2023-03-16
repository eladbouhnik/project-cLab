
typedef struct macro{
    char *name;
    char *content;
    struct macro *next;
}newMacro;

/*	get a string and check if it is a valid string declaration:
		check if the string is of the following format "macro NAME".
		the NAME is not a label, instruction, guide or register.
	there must be BLK only between every two "fields" and there
	can't be consectutive BLK.
	if it is a valid declaration, return a pointer to the start of the
	name in the string. otherwise, return NULL.			*/
char *ismacrodec(newMacro *head, char *str);

 	int addMcr(newMacro *head,char *newName,char *newContent);
 	
 	char *getcontent(newMacro *head, char *nameOfMacro);
 	void freeList(newMacro *head);
