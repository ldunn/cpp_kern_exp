section .__mb_header
align 4

MAGIC_NUMBER    equ 0x1BADB002
MOD_ALIGN           equ 1 << 0 ; Align loaded modules on page boundaries
MEMINFO         equ 1 << 1 ; Provide memory map
FLAGS           equ MOD_ALIGN | MEMINFO
CHECKSUM        equ -(MAGIC_NUMBER + FLAGS)
dd MAGIC_NUMBER
dd FLAGS
dd CHECKSUM

section .bss
align 16
KERNEL_STACK_SIZE equ 16384
kernel_stack_bottom:
    resb KERNEL_STACK_SIZE
kernel_stack_top:

section .text

global loader
loader:
    extern kmain
    mov esp, kernel_stack_top
    push ebx
    push eax
    call kmain
.loop:
    jmp .loop
