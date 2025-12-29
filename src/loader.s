

section .__mb_header
align 4

MAGIC_NUMBER    equ 0x1BADB002
FLAGS           equ 0x0
CHECKSUM        equ -MAGIC_NUMBER
dd MAGIC_NUMBER
dd FLAGS
dd CHECKSUM

section .bss
align 4
KERNEL_STACK_SIZE equ 4096
kernel_stack:
    resb KERNEL_STACK_SIZE

section .text

global loader
loader:
    extern kmain
    call kmain
.loop:
    jmp .loop
