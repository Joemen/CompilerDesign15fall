TARGET = parser
OBJECT = parser.tab.c parser.tab.o lex.yy.c alloc.o functions.o semanticAnalysis.o symbolTable.o offset.o gencode.o
OUTPUT = parser.output parser.tab.h
CC = gcc -g
LEX = flex
YACC = bison -v
YACCFLAG = -d
LIBS = -lfl 

parser: parser.tab.o alloc.o functions.o symbolTable.o semanticAnalysis.o offset.o gencode.o
	$(CC) -o $(TARGET) parser.tab.o alloc.o functions.o symbolTable.o semanticAnalysis.o offset.o gencode.o $(LIBS)

parser.tab.o: parser.tab.c lex.yy.c alloc.o functions.c symbolTable.o semanticAnalysis.o offset.o gencode.o
	$(CC) -c parser.tab.c
    
semanticAnalysis.o: semanticAnalysis.c symbolTable.o 
	$(CC) -c semanticAnalysis.c

symbolTable.o: symbolTable.c
	$(CC) -c symbolTable.c

offset.o: offset.c
	$(CC) -c offset.c

gencode.o:  gencode.c
	$(CC) -c gencode.c
 
lex.yy.c: lexer3.l
	$(LEX) lexer3.l

parser.tab.c: parser.y 
	$(YACC) $(YACCFLAG) parser.y

alloc.o: alloc.c
	$(CC) -c alloc.c
	
functions.o: functions.c
	$(CC) -c functions.c

link:
	aarch64-linux-gnu-gcc -static -O0 main.S
run:
	./parser test.c
exec:
	qemu-aarch64-static ./a.out
clean:
	rm -f $(TARGET) $(OBJECT) $(OUTPUT)

