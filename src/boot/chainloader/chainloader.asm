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


; GDT TABLE LOCATION

%DEFINE KERNEL_CODE 0x7E00
[BITS 16]
[ORG 0x7C00]

xor ax, ax
mov ds, ax


; JUMP TO THE MAIN LABEL
start: jmp main

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
                    AT gdt_entry.access, db 0x9A
                    AT gdt_entry.granularity, db 0xCF
                    AT gdt_entry.base_high, db 0
                IEND
            DATA_SEGMENT: ; Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0xFFFF
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 0x92
                    AT gdt_entry.granularity, db 0xCF
                    AT gdt_entry.base_high, db 0
                IEND
            STACK_SEGMENT: ; Access using "mov al, [label + struc.byte]"
                ISTRUC gdt_entry
                    AT gdt_entry.limit_low, dw 0
                    AT gdt_entry.base_low, dw 0
                    AT gdt_entry.base_middle, db 0
                    AT gdt_entry.access, db 0x97
                    AT gdt_entry.granularity, db 0xCF
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

    lgdt [gdtr]    ; load GDT register with start address of Global Descriptor Table
    ; [PMODE STARTS] ENABLE PROTECTED MODE
    statmsg db "Loaded GDT", 13, 10, 0 ; Bytes_right, cursor_x, junk_y
    mov si, statmsg
    call Print

    ; INITIALIZE A20 LINE
    .initA20:
        in al, 0x92
        test al, 2
        jmp .startPM ; Initialize Protected Mode right after enabling A20 line
        or al, 2
        and al, 0xFE
        out 0x92, al
    .startPM:
        cli            ; disable interrupts
        pusha
        mov eax, cr0
        or al, 1       ; set PE (Protection Enable) bit in CR0 (Control Register 0)
        mov cr0, eax
        popa
        ; END ENABLE PROTECTED MODE - INTERRUPTS INACCESSIBLE

        ; Perform far jump to selector 08h (offset into GDT, pointing at a 32bit PM code segment descriptor)
        ; to load CS with proper PM32 descriptor)

        jmp 08h:PModeMain ; Jump to Protected Mode Main

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

        mov eax, 0xb8000 ; Copy the video address to a general purpose register
        mov al, 'x'      ; Copy the character to print to a general purpose register
        mov ah, 0x3F     ; Set the color and copy to a general purpose register
        mov [eax], ax    ; Put the character into the video memory by turning the
                         ; video memory address into a pointer
        ; HANG IF THE KERNEL DECIDES TO RETURN

    hang:
        cli
        hlt
        ; If for some cursed reason the CPU decides to exit anyway,
        ; we jump back to hang
        jmp hang


; Fill up empty space with zeroes to meet 512KB
; [EDIT] I disabled this so that I can keep an eye on how much
;        space I have left in the binary
; times 510-($-$$) db 0

dw 0xAA55