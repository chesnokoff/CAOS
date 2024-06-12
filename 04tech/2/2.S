#include <sys/syscall.h>

.intel_syntax noprefix

.global _start
.text
_start:
  mov rax, SYS_read
  mov rdi, 0
  lea rsi, [SPACE]
  mov rdx, OFFSET 4096
  syscall

  read_print_loop:  
    cmp rax, 0
    jle end
    mov rdx, rax
    mov rax, SYS_write
    mov rdi, 1
    lea rsi, [SPACE]
    syscall
    
    cmp rax, 0
    jle end
    mov rax, SYS_read
    mov rdi, 0
    lea rsi, [SPACE]
    mov rdx, OFFSET 4096
    syscall
    
    jmp read_print_loop

  end:

  mov rdi, rax
  mov rax, SYS_exit
  syscall

.data
SPACE: .space 4096
PAGE_SIZE = . - SPACE
