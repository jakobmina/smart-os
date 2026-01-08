; Multiboot 1 Header (Standard)
section .multiboot
align 4
    dd 0x1BADB002           ; magic
    dd 0x00000003           ; flags (align + meminfo)
    dd -(0x1BADB002 + 0x00000003) ; checksum

section .text
bits 32
global _start
extern kernel_main

_start:
    ; Minimal Serial Init (COM1) for debugging
    mov dx, 0x3f9 ; Port 1
    mov al, 0
    out dx, al ; Disable all interrupts
    
    mov dx, 0x3fb ; Line Control
    mov al, 0x80
    out dx, al ; Enable DLAB
    
    mov dx, 0x3f8 ; Port 0
    mov al, 3
    out dx, al ; Set divisor to 3 (38400 baud)
    
    mov dx, 0x3f9 ; Port 1
    mov al, 0
    out dx, al
    
    mov dx, 0x3fb ; Line Control
    mov al, 3
    out dx, al ; 8 bits, no parity, one stop bit

    ; Set up a basic stack
    mov esp, stack_top

    ; Call the C kernel (passing Multiboot magic and info)
    push ebx
    push eax
    call kernel_main

.hlt:
    hlt
    jmp .hlt

section .bss
align 16
stack_bottom:
    resb 8192
stack_top:
