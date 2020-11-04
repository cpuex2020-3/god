god_fib: main.o instruction.o data.o labels.o parser.o execution.o assembler.o assembly.o
	gcc -Wall main.o instruction.o data.o labels.o parser.o execution.o assembler.o assembly.o -o god_fib

main.o: main.c
	gcc -Wall -c main.c
instruction.o: instruction.c
	gcc -Wall -c instruction.c
data.o: data.c
	gcc -Wall -c data.c
labels.o: labels.c
	gcc -Wall -c labels.c
parser.o: parser.c
	gcc -Wall -c parser.c
execution.o: execution.c
	gcc -Wall -c execution.c
assembler.o: assembler.c
	gcc -Wall -c assembler.c
assembly.o: assembly.c
	gcc -Wall -c assembly.c

clean:
	rm -f *.o god_fib
