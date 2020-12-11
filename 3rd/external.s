
# written by God

# read and print

.text

.globl min_caml_read_int
min_caml_read_int:
  rxbu t1
  or a0, zero, t1
  slli a0, a0, 8
  rxbu t1
  or a0, a0, t1
  slli a0, a0, 8
  rxbu t1
  or a0, a0, t1
  slli a0, a0, 8
  rxbu t1
  or a0, a0, t1
  ret

.globl min_caml_read_float
min_caml_read_float:
  rxbu t1
  or t2, zero, t1
  slli t2, t2, 8
  rxbu t1
  or t2, t2, t1
  slli t2, t2, 8
  rxbu t1
  or t2, t2, t1
  slli t2, t2, 8
  rxbu t1
  or t2, t2, t1
  fmv.s.w fa0, t2
  ret

.globl min_caml_print_newline
min_caml_print_newline:
  addi  t1, zero, 10
  txbu  t1
  ret

.globl min_caml_print_char
min_caml_print_char:
  txbu  a0
  ret

.globl min_caml_print_int_hex
min_caml_print_int_hex:
  addi t1, zero, 48
  txbu t1
  addi t1, zero, 120
  txbu t1
  mv   t1, a0
  addi t2, zero, 28
  addi t3, zero, 10
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
  addi t1, zero, 10
  txbu t1
  ret

.globl min_caml_print_int
min_caml_print_int:
  #negative
  bge  a0, zero, 48
  addi t1, zero, 45
  txbu t1
  lui t1, -512695
  addi t1, t1, -1279
  bge  a0, t1, 24
  lui t1, -488281
  addi t1, t1, -1024
  sub  a0, a0, t1
  addi t1, zero, 50
  txbu t1
  sub  a0, zero, a0
  #positive
  mod10 s2, a0
  div10 a0, a0
  beq   a0, zero, 236
  mod10 s3, a0
  div10 a0, a0
  beq   a0, zero, 208
  mod10 s4, a0
  div10 a0, a0
  beq   a0, zero, 180
  mod10 s5, a0
  div10 a0, a0
  beq   a0, zero, 152
  mod10 s6, a0
  div10 a0, a0
  beq   a0, zero, 124
  mod10 s7, a0
  div10 a0, a0
  beq   a0, zero, 96
  mod10 s8, a0
  div10 a0, a0
  beq   a0, zero, 68
  mod10 s9, a0
  div10 a0, a0
  beq   a0, zero, 40
  mod10 s10, a0
  div10 a0, a0
  beq   a0, zero, 12
  addi  s11, a0, 48
  txbu  s11
  addi  s10, s10, 48
  txbu  s10
  addi  s9, s9, 48
  txbu  s9
  addi  s8, s8, 48
  txbu  s8
  addi  s7, s7, 48
  txbu  s7
  addi  s6, s6, 48
  txbu  s6
  addi  s5, s5, 48
  txbu  s5
  addi  s4, s4, 48
  txbu  s4
  addi  s3, s3, 48
  txbu  s3
  addi  s2, s2, 48
  txbu  s2
  ret

# itof and ftoi

.globl min_caml_float_of_int
min_caml_float_of_int:
	fcvt.s.w	fa0, a0
	ret

.globl min_caml_int_of_float
min_caml_int_of_float:
	fmv.w.s a0, fa0
	srli    t1, a0, 23
	andi    t1, t1, 255  # 0x000000ff
	addi    t1, t1, -126
	bge     t1, zero, 12
	addi    a0, zero, 0
	ret
	slti    t2, a0, 0
	li      t3, 8388607  # 0x007fffff
	and     a0, a0, t3
	addi    t3, t3, 1
	or	    a0, a0, t3
	addi	  t1, t1, -23
	bge	    t1, zero, 16
	sub	    t1, zero, t1
	srl	    a0, a0, t1
  jal     zero, 8
	sll	    a0, a0, t1
	addi	  a0, a0, 1
	srli	  a0, a0, 1
	beq	    t2, zero, 8
	sub	    a0, zero, a0
	ret

.globl min_caml_floor
min_caml_floor:
  fmv      ft0, fa0
  sw	     a0, 4(s0)
	sw	     ra, 8(s0)
  addi	   s0, s0, 12
	jal	min_caml_int_of_float
  fcvt.s.w fa0, a0
	addi	   s0, s0, -12
  lw	     ra, 8(s0)
	lw	     a0, 4(s0)
  flt.s	   t1, ft0, fa0
  fcvt.s.w ft0, t1
  fsub.s	 fa0, fa0, ft0
  ret

.globl min_caml_truncate
min_caml_truncate:
  fmv      ft1, fa0
  fabs     fa0, fa0
  sw	     ra, 4(s0)
  addi	   s0, s0, 8
  jal	min_caml_floor
  jal	min_caml_int_of_float
  addi	   s0, s0, -8
  lw	     ra, 4(s0)
  fmv.s.w  fa0, zero
  flt.s	   t1, ft1, fa0
  beq      t1, zero, 8
  sub   	 a0, zero, a0
  fmv      fa0, ft1
  ret


# written by Joe Hattori

# tri.s
	.data
l.dpi:	# 6.28318548203
	.word 0x40c90fdb
l.pi:	# 3.1415927410125732421875
	.word	0x40490fdb
l.qpi:	# 0.785398
	.word	0x3f490fd8
l.hpi:	# 1.570796
	.word	0x3fc90fd8
l.four:	# 4.000000
	.word	0x40800000
l.two:	# 2.000000
	.word	0x40000000
l.one:	# 1.000000
	.word	0x3f800000
l.sin_3:	# -0.16666668
	.word	0xbe2aaaac
l.sin_5:	# 0.008332824
	.word	0x3c088666
l.sin_7:	# -0.00019587841
	.word	0xb94d64b6
l.cos_2:	# -0.5
	.word	0xbf000000
l.cos_4:	# 0.04166368
	.word	0x3d2aa789
l.cos_6:	# -0.0013695068
	.word	0xbab38106
l.atan_kernel.3:	# -0.3333333
	.word	0xbeaaaaaa
l.atan_kernel.5:	# 0.2
	.word	0x3e4ccccd
l.atan_kernel.7:	# -0.142857142
	.word	0xbe124925
l.atan_kernel.9:	# 0.111111104
	.word	0x3de38e38
l.atan_kernel.11:	# -0.08976446
	.word	0xbdb7d66e
l.atan_kernel.13:	#  0.060035485
	.word	0x3d75e7c5
l.atan_cmp.1:	# 0.4375
	.word	0x3ee00000
l.atan_cmp.2:	# 2.4375
	.word	0x401c0000

	.text
reduction:
	la	t6, l.dpi
	flw	ft1, 0(t6)
	fsgnj.s	ft3, ft1, ft1
	la	t6, l.two
	flw	ft2, 0(t6)
red_cont:
	fle.s	t3, ft1, fa0
	beq	t3, zero, red_break
	fmul.s	ft1, ft1, ft2
	j	red_cont
red_break:
	fle.s	t3, ft3, fa0
	beq	t3, zero, red_ret
	fle.s	t3, ft1, fa0
	beq	t3, zero, red_else
	fsub.s	fa0, fa0, ft1
red_else:
	fdiv.s	ft1, ft1, ft2
	j	red_break
red_ret:
	ret

kernel_sin:
	fsgnj.s	ft0, fa0, fa0
	fmul.s	ft1, fa0, fa0
	la	t2, l.sin_3
	fmul.s	ft0, ft0, ft1
	flw	ft2, 0(t2)
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	la	t2, l.sin_5
	fmul.s	ft0, ft0, ft1
	flw	ft2, 0(t2)
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	la	t2, l.sin_7
	fmul.s	ft0, ft0, ft1
	flw	ft2, 0(t2)
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	fsgnj.s	fa0, fa0, ft4
	ret
kernel_cos:
	fmul.s	ft1, fa0, fa0
	fsgnj.s	ft0, ft1, ft1
	la	t6, l.one
	flw	fa0, 0(t6)
	la	t6, l.cos_2
	flw	ft2, 0(t6)
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	la	t6, l.cos_4
	flw	ft2, 0(t6)
	fmul.s	ft0, ft0, ft1
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	la	t6, l.cos_6
	flw	ft2, 0(t6)
	fmul.s	ft0, ft0, ft1
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	fsgnj.s	fa0, fa0, ft4
	ret

	.globl min_caml_sin
min_caml_sin:
	fsgnj.s	ft4, fa0, fa0
	fsgnjx.s	fa0, fa0, fa0
	sw	ra, 8(s0)
	addi	s0, s0, 12
	jal	reduction
	addi	s0, s0, -12
	lw	ra, 8(s0)
	la	t6, l.pi
	flw	ft0, 0(t6)
	fle.s	t6, ft0, fa0
	beq	t6, zero, sin_else1
	fsub.s	fa0, fa0, ft0
	fsgnjn.s	ft4, ft4, ft4
sin_else1:
	la	t6, l.hpi
	flw	ft1, 0(t6)
	fle.s	t3, ft1, fa0
	beq	t3, zero, sin_else2
	fsub.s	fa0, ft0, fa0
sin_else2:
	la t6, l.qpi
	flw	ft2, 0(t6)
	fle.s	t3, fa0, ft2
	beq	t3, zero, sin_else3
	j	kernel_sin
sin_else3:
	fsub.s	fa0, ft1, fa0
	j kernel_cos

	.globl min_caml_cos
min_caml_cos:
	fsgnjx.s	fa0, fa0, fa0
	fsgnj.s	ft4, fa0, fa0
	sw	ra, 8(s0)
	addi	s0, s0, 12
	jal	reduction
	addi	s0, s0, -12
	lw	ra, 8(s0)
	la	t6, l.pi
	flw	ft0, 0(t6)
	fle.s	t3, ft0, fa0
	beq	t3, zero, cos_else1
	fsub.s	fa0, fa0, ft0
	fsgnjn.s	ft4, ft4, ft4
cos_else1:
	la	t6, l.hpi
	flw	ft1, 0(t6)
	fle.s	t3, ft1, fa0
	beq	t3, zero, cos_else2
	fsub.s	fa0, ft0, fa0
	fsgnjn.s	ft4, ft4, ft4
cos_else2:
	la	t6, l.qpi
	flw	ft2, 0(t6)
	fle.s	t3, fa0, ft2
	beq	t3, zero, cos_else3
	j	kernel_cos
cos_else3:
	fsub.s	fa0, ft1, fa0
	j	kernel_sin

kernel_atan:
	fsgnj.s	ft0, fa0, fa0
	fmul.s	ft1, fa0, fa0
	la	t2, l.atan_kernel.3
	fmul.s	ft0, ft0, ft1
	flw	ft2, 0(t2)
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	la	t2, l.atan_kernel.5
	fmul.s	ft0, ft0, ft1
	flw	ft2, 0(t2)
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	la	t2, l.atan_kernel.7
	fmul.s	ft0, ft0, ft1
	flw	ft2, 0(t2)
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	la	t2, l.atan_kernel.9
	fmul.s	ft0, ft0, ft1
	flw	ft2, 0(t2)
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	la	t2, l.atan_kernel.11
	fmul.s	ft0, ft0, ft1
	flw	ft2, 0(t2)
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	la	t2, l.atan_kernel.13
	fmul.s	ft0, ft0, ft1
	flw	ft2, 0(t2)
	fmul.s	ft2, ft2, ft0
	fadd.s	fa0, fa0, ft2
	ret

	.globl min_caml_aton
min_caml_atan:
	fsgnj.s	ft4, fa0, fa0
	fsgnjx.s	ft3, fa0, fa0
	la	t2, l.atan_cmp.1
	flw	ft1, 0(t2)
	flt.s	t2, ft3, ft1
	beq	t2, zero, atan_else.1
	j	kernel_atan
atan_else.1:
	la	t2, l.atan_cmp.2
	flw	ft1, 0(t2)
	flt.s	t2, ft3, ft1
	beq	t2, zero, atan_else.2
	la	t3, l.one
	flw	ft2, 0(t3)
	fsub.s	fa0, ft3, ft2
	fadd.s	ft2, ft3, ft2
	fdiv.s	fa0, fa0, ft2
	sw	ra, 4(s0)
	addi	s0, s0, 8
	jal	kernel_atan
	addi	s0, s0, -8
	lw	ra, 4(s0)
	la	t2, l.qpi
	flw	ft1, 0(t2)
	fadd.s	fa0, fa0, ft1
	fsgnj.s	fa0, fa0, ft4
	ret
atan_else.2:
	la	t3, l.one
	flw	ft2, 0(t3)
	fdiv.s	fa0, ft2, ft3
	sw	ra, 4(s0)
	addi	s0, s0, 8
	jal	kernel_atan
	addi	s0, s0, -8
	lw	ra, 4(s0)
	la	t2, l.hpi
	flw	ft1, 0(t2)
	fsub.s	fa0, ft1, fa0
	fsgnj.s	fa0, fa0, ft4
	ret

# lib.s
	.data
l.one:	# 1.000000
	.word	0x3f800000
l.zero:	# 0.0
	.word	0x00000000
l.ten:	# 10.0
	.word	0x41200000
l.point5:	# 0.5
	.word	0x3f000000
	.text
	.globl min_caml_create_array
min_caml_create_array:
	mv	t5, a0
	slli	t4, t5, 2
create_array_loop:
	bne	t5, zero, create_array_cont
create_array_exit:
	mv	a0, t0
	add	t0, t0, t4
	ret
create_array_cont:
	addi	t5, t5, -1
	slli	t6, t5, 2
	add	t6, t6, t0
	sw	a1, 0(t6)
	j	create_array_loop

	.globl min_caml_create_float_array
min_caml_create_float_array:
	mv	t5, a0
	slli	t4, t5, 2
create_float_array_loop:
	bne	t5, zero, create_float_array_cont
create_float_array_exit:
	mv	a0, t0
	add	t0, t0, t4
	ret
create_float_array_cont:
	addi	t5, t5, -1
	slli	t6, t5, 2
	add	t6, t6, t0
	fsw	fa0, 0(t6)
	j	create_float_array_loop

	.globl min_caml_fsqr
min_caml_fsqr:
	fmul.s	fa0, fa0, fa0
	ret
	.globl min_caml_sqrt
min_caml_sqrt:
	fsqrt.s	fa0, fa0
	ret
	.globl min_caml_fabs
min_caml_fabs:
	fsgnjx.s	fa0, fa0, fa0
	ret
	.globl min_caml_fless
min_caml_fless:
	flt.s	a0, fa0, fa1
	ret
	.globl min_caml_fhalf
min_caml_fhalf:
	la	t2, l.point5
	flw	ft0, 0(t2)
	fmul.s	fa0, fa0, ft0
	ret

	.globl min_caml_fiszero
min_caml_fiszero:
	la	t2, l.zero
	flw	ft0, 0(t2)
	feq.s	a0, fa0, ft0
	ret

	.globl min_caml_fispos
min_caml_fispos:
	la	t2, l.zero
	flw	ft0, 0(t2)
	flt.s	a0, ft0, fa0
	ret

	.globl min_caml_fisneg
min_caml_fisneg:
	la	t2, l.zero
	flw	ft0, 0(t2)
	flt.s	a0, fa0, ft0
	ret

	.globl min_caml_fneg
min_caml_fneg:
	fsgnjn.s	fa0, fa0, fa0
	ret
