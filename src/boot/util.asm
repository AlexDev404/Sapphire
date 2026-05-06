[BITS 16]

; 1.44 MB floppy geometry
%define SECTORS_PER_TRACK 18
%define NUM_HEADS         2
%define TOTAL_SECTORS     2880

; Load up to 512 KB after the boot sector (stays below PM stack at 0x90000).
%define SECTORS_TO_READ   1024

; Number of retries before giving up on a read error
%define MAX_RETRIES       3

; -----------------------------------------------------------------------------
; disk_read: Load SECTORS_TO_READ sectors (starting at LBA 1) into ES:BX.
;   Input:  DL = BIOS drive number, ES:BX = destination buffer
;   Output: Data loaded sequentially from LBA 1
;   Error:  Prints message and halts after MAX_RETRIES failures
;
; Reads one sector at a time for maximum compatibility.
; Per OSDev wiki: "The simplest workaround is to read or write only one sector
; at a time in CHS mode." This avoids all DMA boundary, track boundary, and
; cylinder boundary issues.
; -----------------------------------------------------------------------------
disk_read:
    pusha

    ; --- Normalize ES:BX so BX=0 (fold offset into segment) ---
    ; 0x0000:0x7E00 → 0x07E0:0x0000 (same physical address)
    mov ax, bx
    shr ax, 4
    mov cx, es
    add ax, cx
    mov es, ax
    xor bx, bx

    ; --- Stack frame for loop state ---
    push bp
    mov bp, sp
    ; [bp-2] = drive number (only DL matters)
    ; [bp-4] = sectors remaining
    ; [bp-6] = current LBA
    xor dh, dh
    push dx                       ; [bp-2] drive number
    push word SECTORS_TO_READ     ; [bp-4] sectors remaining
    push word 1                   ; [bp-6] current LBA

.loop:
    cmp word [bp-4], 0
    je .done

    ; === Convert current LBA to CHS ===
    mov ax, [bp-6]                ; AX = LBA
    xor dx, dx
    mov cx, SECTORS_PER_TRACK
    div cx
    ; AX = LBA / SPT  (used for head & cylinder)
    ; DX = LBA % SPT  (0-based sector within track)

    ; CHS sector number (1-based)
    inc dl
    mov [.sector], dl

    ; Head and cylinder from quotient
    xor dx, dx
    mov cx, NUM_HEADS
    div cx
    ; AX = cylinder, DX = head
    mov [.cylinder], al
    mov [.head], dl

    ; === Read one sector with retry logic ===
    mov di, MAX_RETRIES           ; DI = retries left

.retry:
    mov ah, 0x02                  ; BIOS: read sectors
    mov al, 1                     ; read exactly 1 sector
    mov ch, [.cylinder]
    mov cl, [.sector]
    mov dh, [.head]
    mov dl, [bp-2]                ; drive number
    ; ES:BX = destination (BX = 0)
    int 0x13
    jnc .read_ok                  ; success → advance

    ; --- Read failed: reset disk and retry ---
    dec di
    jz .fail                      ; out of retries → halt

    ; Reset disk (AH=0x00, DL=drive)
    xor ah, ah
    mov dl, [bp-2]
    int 0x13
    jmp .retry

.read_ok:
    ; === Advance state ===
    inc word [bp-6]               ; LBA++
    dec word [bp-4]               ; remaining--

    ; Advance buffer: ES += 32 paragraphs (= 512 bytes = 1 sector)
    mov ax, es
    add ax, 32
    mov es, ax
    ; BX stays 0

    jmp .loop

.done:
    mov sp, bp
    pop bp
    popa
    ret

; --- Scratch bytes for CHS values ---
.cylinder: db 0
.head:     db 0
.sector:   db 0

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
