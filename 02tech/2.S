.global solve
.text

solve:
  mov x9, 0
  sxtw x1, w1
  sxtw x2, w2
  sxtw x3, w3

loop:
  mov x10, 0
  madd x10, x0, x9, x1  //Ax + B
  madd x10, x10, x9, x2 //(Ax + B)x + C
  madd x10, x10, x9, x3 //((Ax + B)x + C)x + D
  cmp x10, 0
  beq end
  add x9, x9, 1
  b loop

end:
  mov x0, x9
  ret
