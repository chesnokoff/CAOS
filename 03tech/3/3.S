.global main

.text
main:
  // Save stack frame and ret address
  stp x29, x30, [sp, -16]! // sp <- sp - 16
  mov x29, sp
  // Allocate local variables
  sub sp, sp, 16
  mov x0, 0 //nullpointer
  mov x1, 40 // size to allocate 10 integers
  bl realloc
  mov x19, x0 // pointer to input string
  mov x20, 0  // char counter
  mov x21, 10 // capacity
  b loop

loop:
  bl getchar_unlocked
  cmp w0, -1
  beq out

push:
  cmp x20, x21
  beq resize
  lsl x8, x20, 2
  str w0, [x19, x8]
  add x20, x20, 1
  b loop

resize:
  mov x22, x0
  mov x0, x19 // x0 - pointer to array
  lsl x21, x21, 1 // capacity *= 2
  lsl x1, x21, 2 // x1 - new size to allocate
  bl realloc
  mov x19, x0
  mov x0, x22
  b push
  
out:
  cmp x20, 0
  beq end
  sub x20, x20, 1
  lsl x8, x20, 2
  ldr w0, [x19, x8]
  bl putchar_unlocked
  b out

end:
  mov x0, x19
  lsl x1, x21, 2
  bl free
  // Free local variables
  mov sp, x29 
  // Restore fp, sp and free the memory
  ldp x29, x30, [sp], 16
  // Exit with return code 0
  mov w0, 0
  ret
