[GLOBAL _start]
[BITS 16]
[EXTERN _rust]
[EXTERN _testvbe]

graphics:
    ; Get VBE information
    mov ah, 4Fh
    mov al, 00h ; Return Super VGA information
    mov di, vbe_info_block ; Pointer to buffer
    int 10h

    ; Get video mode info
    mov ax, 4F01h
    mov cx, 101h ; First mode
    mov di, vbe_mode_block
    int 10h

    ; Set video mode to 101h (640x480), 105h (800x600)
    mov ah, 0
    mov ax, 4F02h
    mov bx, 101h ; Mode 101h
    int 10h

    ; ; Assume first window is valid
    ; mov ax, WORD [es:vbe_mode_block.window_a_segment]
    ; mov es, ax

    ; ; Example of how to change the window
    ; mov ax, 4F05h
    ; xor bx, bx
    ; mov dx, 5 ; Granularity units
    ; int 10h

    ; ; Clear window A
    ; xor di, di
    ; mov al, 0 ; Value to clear with
    ; mov cx, 3 * 1024 * 20 ; Number of bytes to clear
    ; rep stosb


	; mov eax, [vbe_mode_block.framebuffer]
	; mov dword[vbe_current_mode.framebuffer], eax
	jmp _start

error:
	lodsb
	or al, al
	je mode_end
	mov ah, 0x0e
	ERR db "ERROR", 13, 10, 0
	mov al, [ERR]
	int 10h
	jmp error

mode_end:
	cli
	hlt


_start:
	cli                          ; Disable interrupts
	pusha
	mov eax, cr0
	or al, 1                     ; Set PE (Protection Enable) bit in CR0 (Control Register 0)
	mov cr0, eax
	popa
	; END ENABLE PROTECTED MODE - INTERRUPTS INACCESSIBLE

	; Perform far jump to selector 0x8 (offset into GDT, pointing at a 32bit PM code segment descriptor)
	; to load CS with proper PM32 descriptor)
	jmp long 0x8:PModeMain        ; Jump to Protected Mode Main in the code segment

	[BITS 32]
PModeMain:
	; load DS, ES, FS, GS, SS, ESP
	; Flush GDT + Initialize it + load segment registers
	mov eax, 0x10                ; Initialize the segment descriptors with the data segment
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax
	; JUMP TO KERNEL

	; mov ax, 0x0F ; Pixel Color. We chose red
	; @audit-issue PITCH + BPP Reassignment
	mov esi, DWORD [vbe_mode_block.pitch] ; The pitch is at offset 12h in the mode information block
	mov ebx, DWORD [vbe_mode_block.framebuffer]; Our framebuffer
	mov edi, DWORD [vbe_mode_block.bpp]; Our BPP
	; add ebx, 180050; pixel_offset = y * pitch + ( x * ( bpp / 8 )) + framebuffer;
	; mov [ebx], ax

	call _rust
	jmp $


rodata:
	; VBE Variables
	; width: dw 1920
	; height: dw 1080
	; width: dw 1024
	; height: dw 768

vbe_current_mode:             ; Current mode
.height: dw 0
.width: dw 0
.framebuffer: dd 0
.pitch: dw 0
.bpp: db 0
.bytes_per_pixel: dw 0


vbe_query:                    ; Preferred mode
.width: dw 800
.height: dw 600
.bpp: db 32
.offset: dw 0
.t_segment: dw 0              ; "segment" is keyword in fasm
.mode: dw 0

	; PADDING
	times 512 - ($ - $$) db 0

vbe_info_block:               ; 'Sector' 2
.vbe_signature: db 'VBE2'
.vbe_version: dw 0            ; Should be 0300h? BCD value
.oem_pointer: dd 0
.capabilities: dd 0
.video_modes: dd 0
.total_memory: dw 0
.oem_software_rev: dw 0
.oem_vendor_name_pointer: dd 0
.oem_product_name_pointer: dd 0
.oem_product_revision_pointer: dd 0
.reserved: times 222 db 0
.oem_data: times 256 db 0

vbe_mode_block:               ; 'Sector' 3
	;; Mandatory info for all VBE revisions
.mode_attributes: dw 0
.window_a_attributes: db 0
.window_b_attributes: db 0
.window_granularity: dw 0
.window_size: dw 0
.window_a_segment: dw 0
.window_b_segment: dw 0
.window_function_pointer: dd 0
.pitch: dw 0                  ; bytes per scanline

	;; Mandatory info for VBE 1.2 and above
.width: dw 0
.height: dw 0
.x_char: db 0                 ; unused
.y_char: db 0                 ; unused
.planes: db 0
.bpp: db 0                    ; bits - per - pixel
.banks: db 0
.memory_model: db 0
.bank_size: db 0
.image_pages: db 0
.reserved1: db 1

	;; Direct color fields (required for direct / 6 and YUV / 7 memory models)
.red_mask: db 0
.red_field: db 0
.green_mask: db 0
.green_field: db 0
.blue_mask: db 0
.blue_field: db 0
.reserved_mask: db 0
.reserved_field: db 0
.direct_color_mode_info: db 0

	;; Mandatory info for VBE 2.0 and above
.framebuffer: dd 0            ; Physical address for flat memory frame buffer
.reserved2: dd 0
.reserved3: dw 0

.reserved4: times 206 db 0    ; Remainder of mode info block

; Sector padding
times 1536 - ($ - $$) db 0
