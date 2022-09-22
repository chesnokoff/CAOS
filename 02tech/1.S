.global sum
.text

sum:
  mov x9, x2        // create copy of array pointer
  mov x10, 0        // x10 - i counter

loop:
  cmp x10, x1       // compare counter x10 with x1 - N
  bge end           // jump to end if counter >= N
  ldr x11, [x9, 0]  // load X_i to x11
  add x0, x0, x11   // x0 += X_i
  add x9, x9, 4    // next element in X
  add x10, x10, 1   // ++counter x4
  b loop            // jump to begin of loop

end:
  ret               // return x0
  
