test_user:
    jmp test_user

global enter_ring3

enter_ring3:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x23
    push rsp
    pushfq
    push 0x1B
    push test_user
    iretq
