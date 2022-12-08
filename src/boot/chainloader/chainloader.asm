[BITS 16]
; CHAINLOADER LOCATION
; JUMP TO OFFSET 0x7C00

[ORG 0x7C00]

; ENABLE PROTECTED MODE
cli            ; disable interrupts
lgdt [gdtr]    ; load GDT register with start address of Global Descriptor Table
mov eax, cr0 
or al, 1       ; set PE (Protection Enable) bit in CR0 (Control Register 0)
mov cr0, eax
; END ENABLE PROTECTED MODE
 
; Perform far jump to selector 08h (offset into GDT, pointing at a 32bit PM code segment descriptor) 
; to load CS with proper PM32 descriptor)


; INITIALIZE A20 LINE
in al, 0x92
test al, 2
jnz 08h:PModeMain ; Jump to Protected Mode Main
or al, 2
and al, 0xFE
out 0x92, al

bits 32
PModeMain:
; load DS, ES, FS, GS, SS, ESP


gdt:
NULL_DESC:
    dd 0            ; null descriptor
    dd 0

CODE_DESC:
    dw 0xFFFF       ; limit low
    dw 0            ; base low
    db 0            ; base middle
    db 10011010b    ; access
    db 11001111b    ; granularity
    db 0            ; base high

DATA_DESC:
    dw 0xFFFF       ; data descriptor
    dw 0            ; limit low
    db 0            ; base low
    db 10010010b    ; access
    db 11001111b    ; granularity
    db 0            ; base high

gdtr:
    Limit dw 24         ; length of GDT
    Base dd NULL_DESC   ; base of GDT

hang:
 jmp $