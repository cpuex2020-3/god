# 特に意味もなければ呼出規約すら守ってないテストプログラム
wawa:
  bne a0, a1, 8
  addi fp, fp, 3
  slti t5, a0, 8
  jalr zero, ra, 0
uouo:
  xori s0, a0, 2
  addi s1, s0, 3
  or   a0, s0, s1
  addi sp, sp, -16
  sw s0, 8(sp)
  sub s0, zero, s0
  lw s0, 8(sp)
  addi sp, sp, 16
  jal wawa
main:
  addi sp, sp, -16
  sw s0, 8(sp)
  addi s0, sp, 16
  addi a5, zero, 1
  addi a6, zero, 2
  add a5, a5, a6
  addi a0, a5, 0
  lw s0, 8(sp)
  addi sp, sp, 16
  slt s0, a6, a0
  beq s0, t5, 12
  addi s0, s0, -1
  jal uouo
  lui a2, 100
