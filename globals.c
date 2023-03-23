#include "guides.h"
#include "instruction.h"


guide guids[] =	{
					{"data"	, 1 , GNM , GALL , INTTYP , {GNDEF , GNDEF }},
					{"entry" , 1 , 1 , GENT	 , LBLTYP , {GNDEF , GNDEF }},
					{"extern" , 1 , 1 , GEXT , LBLTYP , {GNDEF , GNDEF}},
					{"string" , 1 , 1 , GALL , STRTYP , {GNDEF , GNDEF }}
					
		 };
				
instruct insts[] = {		
					{"add" ,2, A0+A1+A3 , A1+A3 , AN,AN},
					{"bne"	,10 , AN , A1+A2 , A0+A1+A3 ,A0+A1+A3},
					{"clr"	,5 , AN , A1+A3 , AN,AN},
					{"cmp"	, 1 , A0+A1+A3,A0+A1+A3, AN,AN},
					{"dec"	, 8 , AN,A1+A3	, AN,AN},
					{"inc"	, 7 , AN , A1+A3 , AN , AN },
					{"jmp"	,9, AN , A1+A2 , A0+A1+A3 , A0+A1+A3 }, 
					{"jsr"	,13, AN	, A1+A2	, A0+A1+A3 , A0+A1+A3 }, 
					{"lea"	,6, A1 , A1+A3 , AN,AN},
					{"mov"	, 0 , A0+A1+A3	, A1+A3	, AN,AN},
					{"not"	, 4 , AN , A1+A3 , AN , AN },
					{"prn"	, 12 , AN , A0+A1+A3 , AN,AN},
					{"red"	,11, AN	, A1+A3	, AN , AN },
					{"rts"	,14 , AN , AN , AN , AN},
					{"stop"	,15, AN	, AN , AN , AN },
					{"sub" , 3 , A0+A1+A3 , A1+A3 , AN , AN }
					
		    };
					
					
		/* array of the registers' names */			
char *regs[] = {"r0","r1","r2","r3","r4","r5","r6","r7"};

