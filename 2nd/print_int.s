#jal min_caml_print_int

min_caml_start:
li   a0, 123

li   t1, 48
txbu t1
li   t1, 120
txbu t1
mv   t1, a0
li   t2, 28
li   t3, 10
srl  a0, a0, t2
andi a0, a0, 15
blt  a0, t3, 8
addi a0, a0, 7
addi a0, a0, 48
txbu a0
beq  t2, zero, 16
addi t2, t2, -4
mv   a0, t1
jal  zero, -36
li   t1, 10
txbu t1

ret
