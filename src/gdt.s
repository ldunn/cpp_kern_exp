global init_gdt

gdtr dw 0 ; limit (GDT size)
     dd 0 ; base address

; initialize the GDT. Also reloads all segments
; [esp] - return address
; [esp + 4] - size of GDT structure
; [esp + 8] - address of the GDT structure to load
init_gdt:
    mov ax, [esp + 4]
    mov [gdtr], ax
    mov eax, [esp + 8]
    mov [gdtr + 2], eax
    lgdt [gdtr]
reload_segments:
    jmp 0x08:.reload_CS ; 0x08 is our new code segment
.reload_CS:
    mov ax, 0x10 ; 0x10 is our new data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
