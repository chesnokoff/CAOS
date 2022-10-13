#include <sys/syscall.h>

.intel_syntax noprefix

.global _start
.text
_start:
  mov rax, SYS_brk
  mov rdi, -1
  syscall

  mov r12, rax  // heap_start
  mov r13, 0    // heap size
  mov r14, 4096 // heap capacity
  
  mov rax, SYS_brk
  mov rdi, r12
  add rdi, 4096
  syscall
  
  mov r15, rax // break pointer += Page_size

  mov rax, SYS_read
  mov rdi, 0
  mov rsi, r12
  mov rdx, r14
  sub rdx, r13
  syscall

  read_loop:
    cmp rax, 0
    jle check_empty
    add r13, rax   //  size += read
    cmp r13, r14
    jl new_read
    
    mov rax, SYS_brk
    mov rdi, r15
    add rdi, 4096
    syscall

    mov r15, rax
    add r14, 4096 // capacity += PAGE_SIZE
    
  new_read:
    mov rax, SYS_read
    mov rdi, 0
    mov rsi, r12
    add rsi, r13 //start += size
    mov rdx, r14
    sub rdx, r13
    syscall

    jmp read_loop

  check_empty:
    cmp r13, 0
    jg check_new_line
    mov BYTE PTR [r12], '\n'
    inc r13
    jmp prepare_to_write

  check_new_line:
    cmpb [r12d + r13d - 1], '\n'
    jz prepare_to_write
    mov rax, SYS_brk
    mov rdi, r15
    inc rdi
    syscall
    mov r15, rax
    mov BYTE PTR [r12 + r13], '\n'
    inc r13
  
  prepare_to_write:
    mov r14, r13
    dec r14    // capacity now is pointer on last char that was not printed
    sub r13, 2 // size -> pos. Now r13 is pos
  
  write_loop:
    cmp r13, 0
    jl last_write
    cmpb [r12 + r13], '\n'
    jnz dec_pos
    jmp write_line

  dec_pos:
    dec r13
    jmp write_loop
  
  write_line:
    mov rax, SYS_write
    mov rdi, 1
    mov rsi, r12
    add rsi, r13
    inc rsi
    mov rdx, r14
    sub rdx, r13
    syscall
    mov r14, r13
    
    jmp dec_pos

  last_write:
    mov rax, SYS_write
    mov rdi, 1
    mov rsi, r12
    mov rdx, r14
    syscall

    mov BYTE PTR [r12], '\n'
    mov rax, SYS_write
    mov rdi, 1
    mov rsi, r12
    mov rdx, 1
    syscall


  mov rdi, 0
  mov rax, SYS_exit
  syscall

