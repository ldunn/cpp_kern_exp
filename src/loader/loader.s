section .__mb_header
align 4

MAGIC_NUMBER    equ 0x1BADB002
MOD_ALIGN       equ 1 << 0 ; Align loaded modules on page boundaries
MEMINFO         equ 1 << 1 ; Provide memory map
FLAGS           equ MOD_ALIGN | MEMINFO
CHECKSUM        equ -(MAGIC_NUMBER + FLAGS)
dd MAGIC_NUMBER
dd FLAGS
dd CHECKSUM

section .bss
align 16
STACK_SIZE equ 4096
bootstrap_stack:
    resb STACK_SIZE
bootstrap_stack_top:

section .text

GDT:
    .Null: equ $ - GDT
        dq 0
    .Code: equ $ - GDT
        .Code.limit_lo: dw 0xffff
        .Code.base_lo: dw 0
        .Code.base_mid: db 0
        .Code.access: db PRESENT | NOT_SYS | EXEC | RW
        .Code.flags: db GRAN_4K | LONG_MODE | 0xF   ; Flags & Limit (high, bits 16-19)
        .Code.base_hi: db 0
    .Data: equ $ - GDT
        .Data.limit_lo: dw 0xffff
        .Data.base_lo: dw 0
        .Data.base_mid: db 0
        .Data.access: db PRESENT | NOT_SYS | RW
        .Data.Flags: db GRAN_4K | SZ_32 | 0xF       ; Flags & Limit (high, bits 16-19)
        .Data.base_hi: db 0
    .Pointer:
        dw $ - GDT - 1
        dq GDT

global loader
loader:
    cli

    mov esp, bootstrap_stack_top

    ; Store the multiboot info  for the kernel (pushing 0s because we will pop these in 64-bit mode)
    ; and then push it again for the loader
    push 0
    push ebx
    push 0
    push eax

    push ebx
    push eax

    extern loader_main
    call loader_main
    
    add esp, 8

    ; edx:eax contains the entry point of the kernel
    push edx
    push eax

; Initialize paging

    extern bootstrap_PML4T ; Defined in paging.c
    mov edi, bootstrap_PML4T
    mov cr3, edi

CR4_PAE_ENABLE equ 1 << 5

    mov eax, cr4
    or eax, CR4_PAE_ENABLE
    mov cr4, eax

EFER_MSR equ 0xC0000080
EFER_LM_ENABLE equ 1 << 8

    mov ecx, EFER_MSR
    rdmsr
    or eax, EFER_LM_ENABLE
    wrmsr

CR0_PM_ENABLE equ 1 << 0
CR0_PG_ENABLE equ 1 << 31

    mov eax, cr0
    or eax, CR0_PG_ENABLE | CR0_PM_ENABLE   ; ensuring that PM is set will allow for jumping
                                            ; from real mode to compatibility mode directly
    mov cr0, eax

; Access bits
PRESENT        equ 1 << 7
NOT_SYS        equ 1 << 4
EXEC           equ 1 << 3
DC             equ 1 << 2
RW             equ 1 << 1
ACCESSED       equ 1 << 0

; Flags bits
GRAN_4K       equ 1 << 7
SZ_32         equ 1 << 6
LONG_MODE     equ 1 << 5

    lgdt [GDT.Pointer]
    jmp GDT.Code:Realm64

bits 64

Realm64:
    mov ax, GDT.Data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    pop rax
    pop rdi
    pop rsi
    ;hlt
    jmp rax

.loop:
    jmp .loop
