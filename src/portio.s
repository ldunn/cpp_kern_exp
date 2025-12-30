bits 64

global _outb

; send a byte to an I/O port
; rsi - the data byte
; rdi - the I/O port
; [esp]     - return address
_outb:
    mov rax, rsi
    mov rdx, rdi
    out dx, al
    retq

global _inb

; receive a byte from an I/O port
; rdi - the I/O port 
; [esp]     - return address
_inb:
    mov rdx, rdi
    in al, dx
    retq

global _inw

; receive a word from an I/O port
; rdi - the I/O port 
; [esp]     - return address
_inw:
    mov rdx, rdi
    in ax, dx
    retq

global _indw

; receive a double word from an I/O port
; rdi - the I/O port 
; [esp]     - return address
_indw:
    mov rdx, rdi
    in eax, dx
    retq
    