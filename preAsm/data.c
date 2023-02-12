#include "preAsm.h"
/* array of the registers' names  */
#define INSTSIZE 16
#define NUM_OF_GUIDES 4
char *regs[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};



/* array of the instructions (instruct is defined in instructs.h) */
instruct insts[] =	{
						{"add",2},
						{"bne",10},
						{"clr",5},
						{"cmp",1},
						{"dec",8},
						{"inc",7},
						{"jmp",9},
						{"jsr",7},
						{"lea",6},
						{"jst",13},
						{"mov",0},
						{"not",4},
						{"prn",12},
						{"red",11},
						{"rts",14},
						{"stop",15},
						{"sub",3}
					
					};
					
					
					
					
	guide guids[] =	{
					{"data"},
					{"entry"},
					{"extern"},
					{"string"}
				};
					
char *isreg(char *str){
	/* binary search on "regs" */
	
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

	instruct *findinst(char *str){
	int low = 0, high = INSTSIZE - 1;  /*the limits of the array */
	instruct *inst = NULL; /* the instruction with the same name */
	int i;
	char lc;  /*the character after the first field in the line (EOS or BLK) */
	
	for (i = 0; str[i] != BLK && str[i] != EOS && str[i] != COM; i++);	/* scan for the first character after the first field */
	
	/* replace the character with EOS */
	lc = str[i];
	str[i] = EOS;
	
/*	binary search on insts */
	while (low <= high && inst == NULL){
	/*	the middle of the searched area */
		int mid = low + (high - low)/2;  /*a form of averag that doesn't go beyong high in the calculation (to prevent runtime errors) */
		int cmp = strcmp(str, insts[mid].name); /* the relation between the strings (<,>,=) */
		
		if (cmp < 0){
			high = mid - 1;
		} else if (0 < cmp){
			low = mid + 1;
		} else {
			inst = insts + mid;
		}
	}
	
	str[i] = lc;/*  return the original character */
	
	return inst;
}


guide *findguide(char *str){
	int low = 0, high = NUM_OF_GUIDES - 1;  /*the limits of the array */
	guide *gd = NULL; /* the guide with the same name */
	int i;
	char lc;  /*the character after the first field in the line (EOS or BLK) */
	
	for (i = 0; str[i] != BLK && str[i] != EOS; i++); /* scan for the first character after the first field */
	
	/*replace the character with EOS */
	lc = str[i];
	str[i] = EOS;
	
	/* binary search on guids */
	while (low <= high && gd == NULL){
	/*	the middle of the searched area */
		int mid = low + (high - low)/2; /* a form of averag that doesn't go beyong high in the calculation (to prevent runtime errors) */
		int cmp = strcmp(str, guids[mid].name);  /*the relation between the strings (<,>,=) */
		
		if (cmp < 0){
			high = mid - 1;
		} else if (0 < cmp){
			low = mid + 1;
		} else {
			gd = guids + mid;
		}
	}
	
	str[i] = lc; /* return the original character */
	
	return gd;
}
