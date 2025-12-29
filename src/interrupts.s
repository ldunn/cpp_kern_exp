global disable_interrupts

disable_interrupts:
    cli
    ret

global enable_interrupts

enable_interrupts:
    sti
    ret

idtr dw 0 ; limit (IDT size)
     dd 0 ; base address

global init_idt

; initialize the IDT. Also enables interrupts
; [esp]  - return address
; [esp + 4] - size of IDT structure
; [esp + 8] - address of the IDT structure to load
init_idt:
    mov ax, [esp + 4]
    mov [idtr], ax
    mov eax, [esp + 8]
    mov [idtr + 2], eax
    lidt [idtr]
    ret

%macro isr_err_stub 1
isr_stub_%+%1:
    cli
    pushad
    mov eax, %1
    push eax
    cld
    call exception_handler
    add esp, 4
    popad
    add esp, 4
    iret
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    cli
    push 0xdead ; Push dummy error code
    pushad
    mov eax, %1
    push eax
    cld
    call exception_handler
    add esp, 4
    popad
    add esp, 4
    iret
%endmacro

%macro irq_stub 1
irq_stub_%+%1:
    cli
    push 0xdead ; Push dummy error code 
    pushad
    mov eax, %1
    push eax
    cld
    call irq_handler
    add esp, 4
    popad
    add esp, 4
    sti
    iret
%endmacro

extern exception_handler

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

extern irq_handler
; IRQS
irq_stub 32
irq_stub 33
irq_stub 34
irq_stub 35
irq_stub 36
irq_stub 37
irq_stub 38
irq_stub 39
irq_stub 40
irq_stub 41
irq_stub 42
irq_stub 43
irq_stub 44
irq_stub 45
irq_stub 46
irq_stub 47

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    32
    dd isr_stub_%+i ; use DQ instead if targeting 64-bit
%assign i i+1 
%endrep

%assign i 32
%rep 16
    dd irq_stub_%+i
%assign i i+1
%endrep
