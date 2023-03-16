flags = -Wall -ansi -pedantic -g

all: assembler

assembler: assemblerR.o guidesR.o ioR.o pass1R.o pass2R.o  preAsm.o supfuncsR.o  error.o memoryListR.o instructR.o macroTable.o globalsR.o
	gcc $(flags) assemblerR.o guidesR.o ioR.o pass1R.o pass2R.o preAsm.o supfuncsR.o  error.o memoryListR.o  instructR.o macroTable.o globalsR.o -o assembler
	
assemblerR.o: assemblerR.c assemblerR.h error.h
	gcc $(flags) -c assemblerR.c -o assemblerR.o
	
guidesR.o: guidesR.c guidesR.h
	gcc $(flags) -c guidesR.c -o guidesR.o
	
ioR.o: ioR.c ioR.h error.h
	gcc $(flags) -c ioR.c -o ioR.o
	
pass1R.o: pass1R.c  ioR.h memoryListR.h error.h guidesR.h supfuncsR.h instructR.h
	gcc $(flags) -c pass1R.c -o pass1R.o
	
pass2R.o: pass2R.c pass2R.h ioR.h memoryListR.h error.h guidesR.h supfuncsR.h instructR.h
	gcc $(flags) -c pass2R.c -o pass2R.o
preAsm.o: preAsm.c  preAsm.h macroTable.h ioR.h supfuncsR.h  error.h
	gcc $(flags) -c preAsm.c -o preAsm.o
	
supfuncsR.o: supfuncsR.c ioR.h  supfuncsR.h  guidesR.h error.h instructR.h 
	gcc $(flags) -c supfuncsR.c -o supfuncsR.o
	
	
error.o: error.c error.h ioR.h memoryListR.h
	gcc $(flags) -c error.c -o error.o

memoryListR.o: memoryListR.c memoryListR.h error.h ioR.h
	gcc $(flags) -c memoryListR.c -o memoryListR.o

macroTable.o:  macroTable.c macroTable.h supfuncsR.h instructR.h guidesR.h
	gcc $(flags) -c macroTable.c -o macroTable.o


instructR.o: instructR.c instructR.h ioR.h
	gcc $(flags) -c instructR.c -o instructR.o

globalsR.o: globalsR.c instructR.h guidesR.h
	gcc $(flags) -c globalsR.c -o globalsR.o

clean:
	rm *.o

