.global main

.text
main:
  // Save stack frame and ret address
  stp x29, x30, [sp, -16]! // sp <- sp - 16
  mov x29, sp
  // Allocate local variables
  sub sp, sp, 16

loop:
  bl getchar_unlocked
  cmp w0, -1
  beq end
  cmp w0, '0'
  blt loop
  cmp w0, '9'
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
