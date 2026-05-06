; -----------------------------------------------------------------------------
; Stage 1 chainloader (MBR, 512 bytes)
;
; Job:
;   1. Set up segments and a real-mode stack
;   2. Load stage2 + kernel from disk into memory at 0x7E00
;   3. Far-jump to stage2 in real mode
;
; That's it. No GDT, no protected mode, no A20. Stage2 handles all of that.
; -----------------------------------------------------------------------------

[BITS 16]
[ORG 0x7C00]

jmp main
nop

; -----------------------------------------------------------------------------
; FAT12 BIOS Parameter Block (kept for compatibility with FAT12 tooling)
; -----------------------------------------------------------------------------
fat12_oem:                   db 'MSWIN4.1'        ; 8 bytes
fat12_bytes_per_sector:      dw 512
fat12_sectors_per_cluster:   db 1
fat12_reserved_sectors:      dw 1
fat12_fat_count:             db 2
fat12_dir_entries_count:     dw 0x00E0
fat12_total_sectors:         dw 2880              ; 2880 * 512 = 1.44MB
fat12_media_descriptor_type: db 0xF0              ; F0 = 3.5" floppy
fat12_sectors_per_fat:       dw 9
fat12_sectors_per_track:     dw 18
fat12_heads:                 dw 2
fat12_hidden_sectors:        dd 0
fat12_large_sector_count:    dd 0

; Extended Boot Record
ebr_drive_number:            db 0
                             db 0                 ; reserved
ebr_signature:               db 0x29
ebr_volume_id:               db 0x12, 0x34, 0x56, 0x78
ebr_volume_label:            db 'SPPH       '     ; 11 bytes
ebr_system_id:               db 'fat12   '        ; 8 bytes

; -----------------------------------------------------------------------------
; Stage 2 lives right after the boot sector.
; Physical address = STAGE2_SEGMENT * 16 + STAGE2_OFFSET = 0x7E00.
; Stage 2 is linked with its base at 0x7E00 to match.
; -----------------------------------------------------------------------------
%define STAGE2_SEGMENT 0x0000
%define STAGE2_OFFSET  0x7E00

%include "boot/util.asm"

main:
    ; Zero the segment registers (DL still holds the boot drive from BIOS)
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; Real-mode stack just below the chainloader
    mov ss, ax
    mov sp, 0x7C00

    ; Load stage2 + kernel to ES:BX = 0x0000:0x7E00.
    ; DL is still the boot drive — nothing above touched it.
    mov ax, STAGE2_SEGMENT
    mov es, ax
    mov bx, STAGE2_OFFSET
    call disk_read

    ; Hand off to stage2 (still in real mode)
    jmp STAGE2_SEGMENT:STAGE2_OFFSET

; -----------------------------------------------------------------------------
; Boot sector signature
; -----------------------------------------------------------------------------
times 510-($-$$) db 0
dw 0xAA55