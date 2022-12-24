; 1. Determine which partition to boot from (either by looking
;    for the active partition, or by presenting the user with a selection
;    of installed operating systems to chose from) ❌ [Ignored]
;
; 2. Determine where your kernel image is located on the boot partition
;    (either by interpreting the file system, or by loading the image from
;    a fixed position) **[WILL LOAD FROM FIXED POSITION IN MEMORY]** ✅
;
; 3. Load the kernel image into memory (requires basic disk I/O); ✅
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

%DEFINE KERNEL_CODE 0x1000
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

[BITS 16]
[ORG 0x7C00]

; Initialize the segment registers
xor ax, ax
mov ds, ax
mov es, ax

; JUMP TO THE MAIN LABEL
init: jmp short main

fat12_oem:                    db 'MSWIN4.1'           ; 8 bytes
fat12_bytes_per_sector:       dw 512
fat12_sectors_per_cluster:    db 1
fat12_reserved_sectors:       dw 1
fat12_fat_count:              db 2
fat12_dir_entries_count:      dw 0E0h
fat12_total_sectors:          dw 2880                 ; 2880 * 512 = 1.44MB
fat12_media_descriptor_type:  db 0F0h                 ; F0 = 3.5" floppy disk
fat12_sectors_per_fat:        dw 9                    ; 9 sectors/fat12
fat12_sectors_per_track:      dw 18
fat12_heads:                  dw 2
fat12_hidden_sectors:         dd 0
fat12_large_sector_count:     dd 0

; extended boot record
ebr_drive_number:           db 0                    ; 0x00 floppy, 0x80 hdd, useless
                            db 0                    ; reserved
ebr_signature:              db 29h
ebr_volume_id:              db 12h, 34h, 56h, 78h   ; serial number, value doesn't matter
ebr_volume_label:           db 'SPPH       '        ; 11 bytes, padded with spaces
ebr_system_id:              db 'fat12   '           ; 8 bytes


BIOS_UTIL:
    Print:
        .print:
            lodsb
            or al, al
            je .done
            mov ah, 0x0E
            int 10h
            .repeat:
                jmp .print
            .done:
                ret
    disk_read:
	    ; store all register values
	    pusha
	    push dx

	    ; prepare data for reading the disk
	    ; al = number of sectors to read (1 - 128)
	    ; ch = track/cylinder number
	    ; dh = head number
	    ; cl = sector number
	    mov ah, 0x02
	    mov al, dh
	    mov ch, 0x00
	    mov dh, 0x00
	    mov cl, 0x02
	    int 13h

	    ; in case of read error
	    ; show the message about it
	    jc disk_read_error
    
    	; check if we read expected count of sectors
    	; if not, show the message with error
	    pop dx
	    cmp dh, al
	    jne disk_read_error
    
    	; restore register values and ret
    	popa
	    ret

    disk_read_error:
	    mov si, DISK_READ_ERROR
	    call Print
	    hlt

; CHAINLOADER
; JUMP TO MAIN

main:

    ; Set video mode
    ; Switch out of text mode and into to graphics mode
    ; mov al, 13h ; 320x200 @ 256
    ; mov ah, 00h
    ; int 10h
    
    ; Load the kernel into memory
    pusha
    mov bx, KERNEL_CODE    ; set address to bx
    mov dh, 15
    mov dl, [BOOTDRIVE]
    call disk_read    ; read our binaries and store by offset above
    popa
    lgdt [gdtr]    ; load GDT register with start address of Global Descriptor Table
    ; lidt [idtr]    ; load IDT register with start address of Interrupt Descriptor Table
    ; [PMODE STARTS] ENABLE PROTECTED MODE
    ; mov si, STATMSG
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
    jmp long KERNEL_CODE
    
    hang:
        cli
        hlt
        ; If for some cursed reason the CPU decides to exit anyway,
        ; we jump back to hang
        jmp hang


; Fill up empty space with zeroes to meet 512B
.rodata:
    DISK_READ_ERROR db "DISK READ ERROR", 13, 10, 0 ; Bytes_right, cursor_x, junk_y
    STATMSG db "Loaded GDT", 13, 10, 0 ; Bytes_right, cursor_x, junk_y
    BOOTDRIVE db 0x00
    
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
times 510-($-$$) db 0
dw 0xAA55