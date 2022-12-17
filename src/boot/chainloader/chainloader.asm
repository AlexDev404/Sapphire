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
%DEFINE IDT_FLAG_GATE_TASK 0x5
%DEFINE IDT_FLAG_GATE_16BIT_INT 0x6
%DEFINE IDT_FLAG_GATE_16BIT_TRAP 0x7
%DEFINE IDT_FLAG_GATE_32BIT_INT 0x8
%DEFINE IDT_FLAG_GATE_32BIT_TRAP 0x9
%DEFINE IDT_FLAG_RING0 0
%DEFINE IDT_FLAG_RING1 32
%DEFINE IDT_FLAG_RING2 64
%DEFINE IDT_FLAG_RING3 96
%DEFINE IDT_FLAG_PRESENT 0x80
extern _start

[BITS 16]
; [ORG 0x7C00]

; Initialize the segment registers
xor ax, ax
mov ds, ax
mov es, ax

; JUMP TO THE MAIN LABEL
init: jmp main

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
; idtr:
;     ; ILimit dw 0xFF * 8 ; length of GDT (6 Entries * 8 bytes)
;     ; IBase dd INULL_GATE ; where the GDT starts
;     ILimit dw 0
;     IBase dd 0

; IDT STARTS HERE

; idt: ; NULL IDT - People say interrupts aren't needed
;         INULL_GATE:
;             ISTRUC idt_entry
;                 AT idt_entry.base_low, dw 0
;                 AT idt_entry.selector, dw 0 ; Select Kernel Code
;                 AT idt_entry.reserved, db 0
;                 AT idt_entry.gate_flags, db 0
;                 AT idt_entry.base_high, db 0
;             IEND
; GDT STARTS HERE
; OUTLINE
; ========
; Selector 0x00: Null segment (32-bit)
; Selector 0x08: kernel code (32-bit, ring 0)
; Selector 0x10: kernel data (32-bit)
; Selector 0x18: kernel stack (32-bit, ring 0)
; Selector 0x20: user code (32-bit, ring 3)
; Selector 0x28: user data (32-bit)
; Selector 0x30: user stack (64-bit, ring 3)

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

    ; Set video mode
    ; Switch out of text mode and into to graphics mode
    mov al, 13h ; 320x200 @ 256
    mov ah, 00h
    int 10h

    ; lidt [idtr]    ; load IDT register with start address of Interrupt Descriptor Table
    ; [PMODE STARTS] ENABLE PROTECTED MODE
    ; statmsg db "Loaded GDT", 13, 10, 0 ; Bytes_right, cursor_x, junk_y
    ; mov si, statmsg
    ; call Print

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
        lgdt [gdtr]    ; load GDT register with start address of Global Descriptor Table
        mov ax, 0x10 ; Initialize the segment descriptors with the data segment
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

        ; JUMP TO KERNEL
        ; call _start
        ; [TEST] Print Exclamation mark to scren

        ; mov ebx, 0xb8000 ; Text mode video address - Copy the video address to a general purpose register (this register supports color)
        ; mov eax, 0x076907489     ; Copy the character to print to a general purpose register
        ; mov ah, 0x2F     ; Aqua (3) on White (F)
        ; mov [ebx], eax  ; Put the character into the video memory by turning the
                         ; video memory address into a pointer

        ; HANG IF THE KERNEL DECIDES TO RETURN
        ; PIXELS
        ; =======
        ; Pixel FMT: Color
        ; Placing a pixel: The location is the address offset
        
        mov ebx, 0xA0000 ; Graphics mode video address - Copy the video address to a general purpose register
        mov al, 0x0A     ; the color of the pixel - Black (0) on Green (A) - Easy way to get video colors on Windows -> `color /?`
        mov [ebx], al    ; Offset of X, Y of pixel - Put the character into the video memory by turning the
                         ; video memory address into a pointer and adding an x, y offset
        ; FMT: x+y*screen_x
        mov [ebx+((0)+(0)*320)], al ; beginning of screen
        ; mov [ebx+((320/2)+(200/2)*320)], al ; center of screen
        mov [ebx+((320-1)+(200-1)*320)], al ; end of screen (had to subtract one - guessing it has something to do with the screen size)
        
        ; Kernel jump into offset (???)
        jmp long _start
    hang:
        cli
        hlt
        ; If for some cursed reason the CPU decides to exit anyway,
        ; we jump back to hang
        jmp hang


; Fill up empty space with zeroes to meet 512KB
; [EDIT] I disabled this so that I can keep an eye on how much
;        space I have left in the binary
times 510-($-$$) db 0

dw 0xAA55