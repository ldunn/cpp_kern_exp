global disable_interrupts

disable_interrupts:
    cli
    retq

global enable_interrupts

enable_interrupts:
    sti
    retq

global init_idt

; initialize the IDT. Also enables interrupts
; [esp]  - return address
; rdi - pointer to IDT descriptor
init_idt:
    lidt [rdi]
    retq

%macro isr_err_stub 1
isr_stub_%+%1:
    cli
    ;pushad
    push rax
    push rcx
    push rdx
    push rbx
    push rsp
    push rbp
    push rsi
    push rdi

    mov eax, %1
    push rax

    cld
    call exception_handler
    add esp, 8
    ;popad

    pop rdi
    pop rsi
    pop rbp
    pop rsp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    add esp, 8
    iretq
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    cli
    push 0xdead ; Push dummy error code
    ;pushad
    push rax
    push rcx
    push rdx
    push rbx
    push rsp
    push rbp
    push rsi
    push rdi

    mov eax, %1
    push rax
    cld
    call exception_handler
    add esp, 8
    ;popad

    pop rdi
    pop rsi
    pop rbp
    pop rsp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    add esp, 8
    iretq
%endmacro

%macro irq_stub 1
irq_stub_%+%1:
    cli
    push 0xdead ; Push dummy error code 
    ;pushad

    push rax
    push rcx
    push rdx
    push rbx
    push rsp
    push rbp
    push rsi
    push rdi
    
    mov eax, %1
    push rax
    cld
    call irq_handler
    add esp, 8
    ;popad
    
    pop rdi
    pop rsi
    pop rbp
    pop rsp
    pop rbx
    pop rdx
    pop rcx
    pop rax

    add esp, 8
    sti
    iretq
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
    dq isr_stub_%+i ; use DQ instead if targeting 64-bit
%assign i i+1 
%endrep

%assign i 32
%rep 16
    dq irq_stub_%+i
%assign i i+1
%endrep
