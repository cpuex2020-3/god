
ALL_O = main.o instruction.o data.o fpu_wrap.o labels.o parser.o execution.o assembler.o assembly.o

god_float: $(ALL_O)
	gcc -Wall -o god_float $(wildcard fpu/*.o) $(ALL_O) -lm

main.o: main.c
	gcc -Wall -c main.c
instruction.o: instruction.c
	gcc -Wall -c instruction.c
data.o: data.c
	gcc -Wall -c data.c
fpu_wrap.o: fpu_wrap.c
	cd fpu; make
	gcc -Wall -c fpu_wrap.c
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
	rm -f *.o god_float
	cd fpu; make clean