[BITS 16]

%define SECTORS_TO_READ 40

; -----------------------------------------------------------------------------
; disk_read: read SECTORS_TO_READ sectors starting at LBA 1 (sector 2 in CHS)
; into ES:BX. DL must contain the BIOS drive number.
; On failure: prints an error message and halts forever.
; -----------------------------------------------------------------------------
disk_read:
    pusha
    mov ah, 0x02              ; BIOS: read sectors
    mov al, SECTORS_TO_READ   ; count
    mov ch, 0                 ; cylinder 0
    mov cl, 2                 ; sector 2 (1-indexed; sector 1 is the MBR)
    mov dh, 0                 ; head 0
    int 0x13
    jc .fail                  ; carry = error
    cmp al, SECTORS_TO_READ   ; verify count *before* popa restores AL
    jne .fail
    popa
    ret

.fail:
    mov si, .err_msg
.print:
    lodsb
    or al, al
    jz .halt
    mov ah, 0x0E
    int 0x10
    jmp .print
.halt:
    cli
    hlt
    jmp .halt

.err_msg: db "DISK READ ERROR", 13, 10, 0