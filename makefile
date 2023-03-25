flags = -Wall -ansi -pedantic -g

all: assembler

assembler: assembler.o guides.o ioFuncs.o pass1.o pass2.o  preAsm.o utils.o  error.o memoryOpr.o instruction.o macroTable.o globals.o
	gcc $(flags) assembler.o guides.o ioFuncs.o pass1.o pass2.o preAsm.o utils.o  error.o memoryOpr.o  instruction.o macroTable.o globals.o -o assembler
	
assembler.o: assembler.c assembler.h error.h
	gcc $(flags) -c assembler.c -o assembler.o
	
guides.o: guides.c guides.h
	gcc $(flags) -c guides.c -o guides.o
	
ioFuncs.o: ioFuncs.c ioFuncs.h error.h
	gcc $(flags) -c ioFuncs.c -o ioFuncs.o
	
pass1.o: pass1.c  ioFuncs.h memoryOpr.h error.h guides.h utils.h instruction.h
	gcc $(flags) -c pass1.c -o pass1.o
	
pass2.o: pass2.c pass2.h ioFuncs.h memoryOpr.h error.h guides.h utils.h instruction.h
	gcc $(flags) -c pass2.c -o pass2.o
preAsm.o: preAsm.c  preAsm.h macroTable.h ioFuncs.h utils.h  error.h
	gcc $(flags) -c preAsm.c -o preAsm.o
	
utils.o: utils.c ioFuncs.h  utils.h  guides.h error.h instruction.h 
	gcc $(flags) -c utils.c -o utils.o
	
	
error.o: error.c error.h ioFuncs.h memoryOpr.h
	gcc $(flags) -c error.c -o error.o

memoryOpr.o: memoryOpr.c memoryOpr.h error.h ioFuncs.h
	gcc $(flags) -c memoryOpr.c -o memoryOpr.o

macroTable.o:  macroTable.c macroTable.h utils.h instruction.h guides.h
	gcc $(flags) -c macroTable.c -o macroTable.o


instruction.o: instruction.c instruction.h ioFuncs.h
	gcc $(flags) -c instruction.c -o instruction.o

globals.o: globals.c instruction.h guides.h
	gcc $(flags) -c globals.c -o globals.o

clean:
	rm *.o

