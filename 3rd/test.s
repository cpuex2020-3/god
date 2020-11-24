.data
x:
.word 0x400fffff, 0x0ac0330f

.text
min_caml_start:
la      a0, x
flw     fa0, 0(a0)
fadd.s  fa1, fa0, fa0
ret
