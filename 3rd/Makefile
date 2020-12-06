CFLAGS = -lm -Wall

god_float: main.o instruction.o data.o fpu.o labels.o parser.o execution.o assembler.o assembly.o
	gcc -o god_float main.o instruction.o data.o fpu.o labels.o parser.o execution.o assembler.o assembly.o $(CFLAGS)

main.o: main.c
	gcc $(CFLAGS) -c main.c
instruction.o: instruction.c
	gcc $(CFLAGS) -c instruction.c
data.o: data.c
	gcc $(CFLAGS) -c data.c
fpu.o: fpu.c
	gcc $(CFLAGS) -c fpu.c
labels.o: labels.c
	gcc $(CFLAGS) -c labels.c
parser.o: parser.c
	gcc $(CFLAGS) -c parser.c
execution.o: execution.c
	gcc $(CFLAGS) -c execution.c
assembler.o: assembler.c
	gcc $(CFLAGS) -c assembler.c
assembly.o: assembly.c
	gcc $(CFLAGS) -c assembly.c

clean:
	rm -f *.o god_float
