.global main

.text
main:
  // Save stack frame and ret address
  stp x29, x30, [sp, -16]! // sp <- sp - 16
  mov x29, sp
  // Allocate local variables
  sub sp, sp, 16
  add x1, sp, 4

loop:
  // mov x0, sp
  bl getchar_unlocked
  //bl putchar_unlocked
  mov x1, x0
  //adr x0, FORMAT_STR
  //bl printf
  cmp w0, -1
  beq end
  cmp w1, 48
  blt loop
  cmp w1, 57
  bgt loop
  bl putchar_unlocked
  b loop

end:
  // Free local variables
  mov sp, x29 
  // Restore fp, sp and free the memory
  ldp x29, x30, [sp], 16
  // Exit with return code 0
  mov w0, 0
  ret

.data
FORMAT_STR: .string "%d"
