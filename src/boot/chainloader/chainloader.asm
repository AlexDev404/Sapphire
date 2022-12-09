; 1. Determine which partition to boot from (either by looking
;    for the active partition, or by presenting the user with a selection
;    of installed operating systems to chose from) ❌ [Ignored]
;
; 2. Determine where your kernel image is located on the boot partition
;    (either by interpreting the file system, or by loading the image from
;    a fixed position) **[WILL LOAD FROM FIXED POSITION IN MEMORY]**
;
; 3. Load the kernel image into memory (requires basic disk I/O); **[TODO]**
;
; 4. Enable protected mode; ✅
;
; 5. Preparing the runtime environment for the kernel (e.g. setting up stack space)
;    - Disable interrupts, including NMI (as suggested by Intel Developers Manual). ✅
;    - Enable the A20 Line. ✅
;    - Load the Global Descriptor Table ✅
;       1. with segment descriptors suitable for code, data, ✅
;       2. and stack. ✅




[BITS 16]
; GDT TABLE LOCATION
[ORG 0x7C00]

; SET VRES (hope this works :'D)
mov ah, 0
mov al, 02h
int 10h

jmp main

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


; STRUCT - GDT DESCRIPTION

gdtr:
    Limit dw 0xFF * 8 ; length of GDT (6 Entries * 8 bytes)
    Base dd NULL_SEGMENT ; where the GDT starts


; GDT STARTS HERE
gdt:
        NULL_SEGMENT: ; Access using "mov al, [label + struc.byte]"
            ISTRUC gdt_entry
                AT gdt_entry.limit_low, dw 0
                AT gdt_entry.base_low, dw 0
                AT gdt_entry.base_middle, db 0
                AT gdt_entry.access, db 0
                AT gdt_entry.granularity, db 0
                AT gdt_entry.base_high, db 0
            IEND
        KERNEL:
            CODE_SEGMENT: ; Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0xFFFF
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 0x9B
                    AT gdt_entry.granularity, db 0xF
                    AT gdt_entry.base_high, db 0
                IEND
            DATA_SEGMENT: ; Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0xFFFF
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 0x93
                    AT gdt_entry.granularity, db 0xF
                    AT gdt_entry.base_high, db 0
                IEND
            STACK_SEGMENT: ; Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 0x97
                    AT gdt_entry.granularity, db 0
                    AT gdt_entry.base_high, db 0
                IEND
        USERLAND:
            UCODE_SEGMENT: ; Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0xFFFF
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 0xFF
                    AT gdt_entry.granularity, db 0xF
                    AT gdt_entry.base_high, db 0
                IEND
            UDATA_SEGMENT: ; Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0xFFFF
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 0xF3
                    AT gdt_entry.granularity, db 0xF
                    AT gdt_entry.base_high, db 0
                IEND
            USTACK_SEGMENT: ; Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 0xF7
                    AT gdt_entry.granularity, db 0
                    AT gdt_entry.base_high, db 0
                IEND

; CHAINLOADER
; JUMP TO MAIN

main:

    cli            ; disable interrupts
    lgdt [gdtr]    ; load GDT register with start address of Global Descriptor Table
    ; [PMODE STARTS] ENABLE PROTECTED MODE
    mov eax, cr0
    or al, 1       ; set PE (Protection Enable) bit in CR0 (Control Register 0)
    mov cr0, eax
    ; END ENABLE PROTECTED MODE

    ; Perform far jump to selector 08h (offset into GDT, pointing at a 32bit PM code segment descriptor)
    ; to load CS with proper PM32 descriptor)


    ; INITIALIZE A20 LINE
    in al, 0x92
    test al, 2
    jmp 08h:PModeMain ; Jump to Protected Mode Main
    or al, 2
    and al, 0xFE
    out 0x92, al

    [BITS 32]
    PModeMain:
        ; load DS, ES, FS, GS, SS, ESP
        ; Flush GDT + Initialize it
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

        ; JUMP TO KERNEL
        ; call _start
        ; [TEST] Print Exclamation mark to scren

        mov ebx, [0xb8000]    ; The video address
        mov al, '!'         ; The character to be print
        mov ah, 0x0F        ; The color: white(F) on black(0)
        mov [ebx], ax        ; Put the character into the video memory
        ; HANG IF THE KERNEL DECIDES TO RETURN

    .hang:
        cli
        hlt
        ; If for some cursed reason the CPU decides to exist anyway,
        ; we jump back to hang
        jmp .hang


; Fill up empty space with zeroes
times 510-($-$$) db 0