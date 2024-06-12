.global main

.text
main:
  // Save stack frame and ret address
  stp x29, x30, [sp, -16]! // sp <- sp - 16
  mov x29, sp
  // Allocate local variables
  sub sp, sp, 16

  // Call scanf
  adr x0, INPUT_STR
  mov x1, sp
  add x2, x1, 4
  bl scanf

  // Sum
  ldr w1, [sp]
  ldr w2, [sp, 4]
  add w1, w1, w2

  // Call printf

  // adrp x0, FORMAT_STR
  // add x0, x0, :lo12:FORMAT_STR
  adr x0, FORMAT_STR
  // ldr w1, [x29, -4]
  bl printf

  // Free local variables
  mov sp, x29 
  // Restore fp, sp and free the memory
  ldp x29, x30, [sp], 16
  // Exit with return code 0
  mov w0, 0
  ret

.data
INPUT_STR: .string "%d %d"
FORMAT_STR: .string "%d"
