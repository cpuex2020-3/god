
# written by God

.text

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


# tri.s : Implement sin, cos.

.data
l.dpi:	# 6.28318548203
.word 0x40c90fdb
l.pi:	# 3.1415927410125732421875
.word	0x40490fdb
l.qpi:	# 0.785398
.word	0x3f490fd8
l.hpi:	# 1.570796
.word	0x3fc90fd8
l.n.720:	# 720.000000
.word	0x44340000
l.twentyfour:	# 24.000000
.word	0x41c00000
l.four:	# 4.000000
.word	0x40800000
l.two:	# 2.000000
.word	0x40000000
l.one:	# 1.000000
.word	0x3f800000
l.n.5040:	# 5040.000000
.word	0x459d8000
l.n.120:	# 120.000000
.word	0x42f00000
l.six:	# 6.000000
.word	0x40c00000

.text
reduction:
la	t6, l.dpi
flw	ft1, 0(t6)
fsgnj.s	ft3, ft1, ft1
la	t6, l.pi
flw	ft0, 0(t6)
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
fdiv.s	ft0, ft0, ft2
j	red_break
red_ret:
ret

kernel_sin:
fmul.s	ft1, fa0, fa0
fmul.s	ft1, ft1, fa0
la	t6, l.six
flw	ft2, 0(t6)
fdiv.s	ft1, ft1, ft2
fsub.s	ft1, fa0, ft1
fmul.s	ft2, fa0, fa0
fmul.s	ft2, ft2, fa0
fmul.s	ft2, ft2, fa0
fmul.s	ft2, ft2, fa0
la	t6, l.n.120
flw	ft3, 0(t6)
fdiv.s	ft2, ft2, ft3
fadd.s	ft1, ft1, ft2
fmul.s	ft2, fa0, fa0
fmul.s	ft2, ft2, fa0
fmul.s	ft2, ft2, fa0
fmul.s	ft2, ft2, fa0
fmul.s	ft2, ft2, fa0
fmul.s	ft2, ft2, fa0
la	t6, l.n.5040
flw	fa0, 0(t6)
fdiv.s	ft2, ft2, fa0
fsub.s	fa0, ft1, ft2
fsgnj.s	fa0, fa0, fa1
ret
kernel_cos:
la	t6, l.one
flw	ft1, 0(t6)
fmul.s	ft2, fa0, fa0
la	a0, l.two
flw	ft3, 0(t6)
fdiv.s	ft2, ft2, ft3
fsub.s	ft1, ft1, ft2
fmul.s	ft2, fa0, fa0
fmul.s	ft2, ft2, fa0
fmul.s	ft2, ft2, fa0
la	t6, l.twentyfour
flw	ft3, 0(t6)
fdiv.s	ft2, ft2, ft3
fadd.s	ft1, ft1, ft2
fmul.s	ft2, fa0, fa0
fmul.s	ft2, ft2, fa0
fmul.s	ft2, ft2, fa0
fmul.s	ft2, ft2, fa0
fmul.s	ft2, ft2, fa0
la	t6, l.n.720
flw	fa0, 0(t6)
fdiv.s	ft2, ft2, fa0
fsub.s	fa0, ft1, ft2
ret

.globl min_caml_sin
min_caml_sin:
fsgnj.s	fa1, fa0, fa0
fsgnjx.s	fa0, fa0, fa0
sw	ra, 8(s0)
addi	s0, s0, 12
jal	reduction
addi	s0, s0, -12
lw	ra, 8(s0)
# pi is set to ft0 in `reduction`
fle.s	t6, ft0, fa0
beq	t6, zero, sin_else1
fsub.s	fa0, fa0, ft0
fsgnjn.s	fa1, fa1, fa1
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
fsgnj.s	fa1, fa0, fa0
sw	ra, 8(s0)
addi	s0, s0, 12
jal	reduction
addi	s0, s0, -12
lw	ra, 8(s0)
# pi is set to ft0 in `reduction`
fle.s	t3, ft0, fa0
beq	t3, zero, cos_else1
fsub.s	fa0, fa0, ft0
fsgnjn.s	fa1, fa1, fa1
cos_else1:
la	t6, l.hpi
flw	ft1, 0(t6)
fle.s	t3, ft1, fa0
beq	t3, zero, cos_else2
fsub.s	fa0, ft0, fa0
fsgnjn.s	fa1, fa1, fa1
cos_else2:
la	t6, l.qpi
flw	ft2, 0(t6)
fle.s	t3, fa0, ft2
beq	t3, zero, cos_else3
j	kernel_cos
cos_else3:
fsub.s	fa0, ft1, fa0
j	kernel_sin

# lib.s : Fix min_caml_create_array

.data
l.one:	# 1.000000
.word	0x3f800000
l.ftoi_cmp: # 8388608.0
.word	0x4b000000
.text
.globl min_caml_create_array
min_caml_create_array:
mv	t5, a0
slli	t4, t5, 2
create_array_loop:
bne	t5, zero, create_array_cont
create_array_exit:
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
add	t0, t0, t4
ret
create_float_array_cont:
addi	t5, t5, -1
slli	t6, t5, 2
add	t6, t6, t0
fsw	fa0, 0(t6)
j	create_float_array_loop

.globl min_caml_sqrt
min_caml_sqrt:
fsqrt.s	fa0, fa0
ret
.globl min_caml_abs_float
min_caml_abs_float:
fsgnjx.s	fa0, fa0, fa0
ret
.globl min_caml_float_of_int
min_caml_float_of_int:
fcvt.s.w	fa0, a0
ret
.globl min_caml_truncate
min_caml_truncate:
.globl min_caml_int_of_float
min_caml_int_of_float:
la	t6, l.ftoi_cmp
flw	ft0, 0(t6)
li	t4, 1258291200
flt.s	t3, fa0, ft0
beq	t3, zero, ftoi_else
fadd.s	fa0, fa0, ft0
fmv.w.s	a0, fa0
sub	a0, a0, t4
ret
ftoi_else:
li	t5, 0
ftoi_cont:
flt.s	t3, fa0, ft0
bne	t3, zero, ftoi_sum
fsub.s	fa0, fa0, ft0
addi	t5, t5, 1
j	ftoi_cont
ftoi_sum:
fadd.s	fa0, fa0, ft0
fmv.w.s	a0, fa0
sub	a0, a0, t4
li	t4, 8388608
ftoi_loop:
bne	t5, zero, ftoi_sum_cont
ret
ftoi_sum_cont:
addi	t5, t5, -1
add	a0, a0, t4
j	ftoi_loop

.globl min_caml_floor
min_caml_floor:
la	t6, l.ftoi_cmp
flw	ft0, 0(t6)
fsgnjx.s	ft1, fa0, fa0
fsgnj.s	ft2, fa0, fa0
flt.s	t3, ft1, ft0
beq	t3, zero, floor_ret
sw	a0, 4(s0)
sw	ra, 8(s0)
addi	s0, s0, 12
jal	min_caml_int_of_float
jal	min_caml_float_of_int
addi	s0, s0, -12
lw	ra, 8(s0)
lw	a0, 4(s0)
flt.s	t3, ft2, fa0
beq	t3, zero, floor_ret
la	t6, l.one
flw	ft2, 0(t6)
fsub.s	fa0, fa0, ft2
floor_ret:
ret
