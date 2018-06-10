.intel_syntax noprefix

.section .text

.globl memory_checker
memory_checker:
push rbx
mov rbx, -(offset 2f - offset 1f)
2:
mov rax, [rdi]

pop rbx       # OK - 'true'
xor rax, rax
not rax
ret

1:
pop rbx       # seg fault - 'false'
xor rax, rax
ret
