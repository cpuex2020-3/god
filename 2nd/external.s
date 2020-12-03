.text

.globl min_caml_print_newline
min_caml_print_newline:
  li   t1, 10
  txbu t1
  ret

.globl min_caml_print_int
min_caml_print_int:
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

.globl min_caml_create_array
min_caml_create_array:
	addi	sp, sp, -12
	sw	s0, 8(sp)
	mv	s0, sp
	sw	a3, 4(sp)
	sw	a2,	0(sp)
	mv	a3, a0
	mv	a0, t0
	mv	a2, a3
	slli	a2, a2, 2
	add	t0, t0, a2
create_array_loop:
	bne	a3, zero, create_array_cont
create_array_exit:
	lw	a2, 0(sp)
	lw	a3, 4(sp)
	mv	sp, s0
	lw	s0, 8(sp)
	addi	sp, sp, 12
	ret
create_array_cont:
	addi	a3, a3, -1
	slli	t1, a3, 2
	add	a2, t1, a0
	sw	a1, 0(a2)
	j	create_array_loop
