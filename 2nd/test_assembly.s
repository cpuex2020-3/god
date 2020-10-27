halt
bne   x10, x11, 8
addi  x8, x8, 3
slti  x30, x10, 8
jalr  x0, x1, 0
xori  x8, x10, 2
addi  x9, x8, 3
or    x10, x8, x9
addi  x2, x2, -16
sw    x8, 8(x2)
sub   x8, x0, x8
lw    x8, 8(x2)
addi  x2, x2, 16
jal   x1, -48
addi  x2, x2, -16
sw    x8, 8(x2)
addi  x8, x2, 16
addi  x15, x0, 1
addi  x16, x0, 2
add   x15, x15, x16
addi  x10, x15, 0
lw    x8, 8(x2)
addi  x2, x2, 16
slt   x8, x16, x10
beq   x8, x30, 12
addi  x8, x8, -1
jal   x1, -84
lui   x12, 100
halt
