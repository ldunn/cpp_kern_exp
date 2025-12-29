global _outb

; send a byte to an I/O port
; [esp + 8] - the data byte
; [esp + 4] - the I/O port
; [esp]     - return address
_outb:
    mov al, [esp + 8]
    mov dx, [esp + 4]
    out dx, al
    ret

global _inb

; receive a byte from an I/O port
; [esp + 4] - the I/O port 
; [esp]     - return address
_inb:
    mov dx, [esp + 4]
    in al, dx
    ret

global _inw

; receive a word from an I/O port
; [esp + 4] - the I/O port 
; [esp]     - return address
_inw:
    mov dx, [esp + 4]
    in ax, dx
    ret

global _indw

; receive a double word from an I/O port
; [esp + 4] - the I/O port 
; [esp]     - return address
_indw:
    mov dx, [esp + 4]
    in eax, dx
    ret
    