
all: reg_t.o fadd_s.o fsub_s.o fmul_s.o fdiv_s.o fsqrt_s.o fcvt_s_w.o flt_s.o fle_s.o fhalf.o

reg_t.o: reg_t.c
	gcc -Wall -c reg_t.c
fadd_s.o: fadd_s.c
	gcc -Wall -c fadd_s.c
fsub_s.o: fsub_s.c
	gcc -Wall -c fsub_s.c
fmul_s.o: fmul_s.c
	gcc -Wall -c fmul_s.c
fdiv_s.o: fdiv_s.c
	gcc -Wall -c fdiv_s.c
fsqrt_s.o: fsqrt_s.c
	gcc -Wall -c fsqrt_s.c
fcvt_s_w.o: fcvt_s_w.c
	gcc -Wall -c fcvt_s_w.c
flt_s.o: flt_s.c
	gcc -Wall -c flt_s.c
fle_s.o: fle_s.c
	gcc -Wall -c fle_s.c
fhalf.o: fhalf.c
	gcc -Wall -c fhalf.c

clean:
	rm -f *.o
