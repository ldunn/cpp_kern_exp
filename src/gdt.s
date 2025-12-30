global init_gdt

; initialize the GDT. Also reloads all segments
; [esp] - return address
; rdi - pointer to GDT descriptor
; rsi - address of the GDT structure to load
init_gdt:
    lgdt [rdi]
reload_segments:
    push 0x08
    lea rax, [rel .reload_CS]
    push rax
    retfq
.reload_CS:
    mov ax, 0x10 ; 0x10 is our new data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
