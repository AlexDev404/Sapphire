extern _start

[BITS 16]
; Initialize the segment registers
xor ax, ax
mov ds, ax
mov es, ax

; JUMP TO THE MAIN LABEL
init: jmp short main

BIOS_UTIL:
    Print:
        .print:
            lodsb
            or al, al
            je .done
            mov ah, 0x0E
            int 0x10
            .repeat:
                jmp .print
            .done:
                ret

; STRUCT - ONE ENTRY OF THE GDT TABLE
STRUC gdt_entry
	.limit_low:   resw 1
	.base_low:    resw 1
	.base_middle: resb 1
	.access:      resb 1
	.granularity: resb 1
	.base_high:   resb 1
	.size:
ENDSTRUC

STRUC idt_entry
    .base_low:    resw 1 ; base_Low 0...15
    .selector:    resw 1
    .reserved:    resb 1
    .gate_flags:  resw 1
    .base_high:   resw 1 ; base_high 16...31
ENDSTRUC


; STRUCT - GDT DESCRIPTION

gdtr:
    GLimit dw (gdt_end - gdt) + 1 ; length of GDT (end - start + 1)
    GBase dd GNULL_SEGMENT ; where the GDT starts

 gdt:
        GNULL_SEGMENT: ; 0x0 - Access using "mov al, [label + struc.byte]"
            ISTRUC gdt_entry
                AT gdt_entry.limit_low, dw 0
                AT gdt_entry.base_low, dw 0
                AT gdt_entry.base_middle, db 0
                AT gdt_entry.access, db 0
                AT gdt_entry.granularity, db 0
                AT gdt_entry.base_high, db 0
            IEND
        KERNEL:
            CODE_SEGMENT: ; 0x8 - Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0xFFFF
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 10011100b
                    AT gdt_entry.granularity, db 11001111b
                    AT gdt_entry.base_high, db 0
                IEND
            DATA_SEGMENT: ; 0x10 - Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0xFFFF
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 10010010b
                    AT gdt_entry.granularity, db 11001111b
                    AT gdt_entry.base_high, db 0
                IEND
            STACK_SEGMENT: ; 0x18 - Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 10011110b
                    AT gdt_entry.granularity, db 11001111b
                    AT gdt_entry.base_high, db 0
                IEND
        USERLAND:
            UCODE_SEGMENT: ; 0x20 - Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0xFFFF
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 11101100b
                    AT gdt_entry.granularity, db 11001111b
                    AT gdt_entry.base_high, db 0
                IEND
            UDATA_SEGMENT: ; 0x28 - Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0xFFFF
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 11100010b
                    AT gdt_entry.granularity, db 11001111b
                    AT gdt_entry.base_high, db 0
                IEND
            USTACK_SEGMENT: ; 0x30 - Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 11111110b
                    AT gdt_entry.granularity, db 11001111b
                    AT gdt_entry.base_high, db 0
                IEND
gdt_end:
; CHAINLOADER
; JUMP TO MAIN

main:

    lgdt [gdtr]    ; load GDT register with start address of Global Descriptor Table

    ; INITIALIZE A20 LINE
    .initA20:
        in al, 0x92
        test al, 2
        jmp .startPM ; Initialize Protected Mode right after enabling A20 line
        or al, 2
        and al, 0xFE
        out 0x92, al
    .startPM:
    

        cli            ; Disable interrupts
        pusha
        mov eax, cr0
        or al, 1       ; Set PE (Protection Enable) bit in CR0 (Control Register 0)
        mov cr0, eax
        popa

        ; END ENABLE PROTECTED MODE - INTERRUPTS INACCESSIBLE

        ; Perform far jump to selector 0x8 (offset into GDT, pointing at a 32bit PM code segment descriptor)
        ; to load CS with proper PM32 descriptor)
        
        jmp long 0x8:PModeMain ; Jump to Protected Mode Main in the code segment

    [BITS 32]
    PModeMain:
        ; load DS, ES, FS, GS, SS, ESP
        ; Flush GDT + Initialize it + load segment registers
        mov eax, 0x10 ; Initialize the segment descriptors with the data segment
        mov ds, eax
        mov es, eax
        mov fs, eax
        mov gs, eax
        mov ss, eax
        ; JUMP TO KERNEL
        ; Kernel jump into offset (???)
        jmp long _start
    hang:
        cli
        hlt
        ; If for some cursed reason the CPU decides to exit anyway,
        ; we jump back to hang
        jmp hang

dw 0xAA55