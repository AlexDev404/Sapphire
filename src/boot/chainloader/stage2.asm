[GLOBAL _start]
[BITS 16]
[EXTERN _rust]
[EXTERN _testvbe]
; vbe_set_mode:
; Sets a VESA mode
; In\	AX = Width
; In\	BX = Height
; In\	CL = Bits per pixel
; Out\	FLAGS = Carry clear on success
; Out\	Width, height, bpp, physical buffer, all set in vbe_screen structure
 
vbe_set_mode:
	push es					; some VESA BIOSes destroy ES, or so I read
	mov ax, 0x4F00				; get VBE BIOS info
	mov di, vbe_info_block
	int 0x10
	pop es
 
	cmp ax, 0x4F				; BIOS doesn't support VBE?
	jne error
 
	mov ax, word [vbe_info_block.video_modes]
	mov [vbe_query.offset], ax
	mov ax, word [vbe_info_block.video_modes+2]
	mov [vbe_query.t_segment], ax
 
	mov fs, ax
	mov si, [vbe_query.offset]
 
.find_mode:
	mov dx, [fs:si]
	add si, 2
	mov [vbe_query.offset], si
	mov [vbe_query.mode], dx
 
	cmp [vbe_query.mode], word 0xFFFF			; end of list?
	je mode_end
 
	push es
	mov ax, 0x4F01				; get VBE mode info
	mov cx, [vbe_query.mode]
	mov di, vbe_mode_block
	int 0x10
	pop es
 
	cmp ax, 0x4F
	jne error
 
	mov ax, [vbe_query.width]
	cmp ax, [vbe_mode_block.width]
	jne .next_mode
 
	mov ax, [vbe_query.height]
	cmp ax, [vbe_mode_block.height]
	jne .next_mode
 
	mov al, [vbe_query.bpp]
	cmp al, [vbe_mode_block.bpp]
	jne .next_mode
 
	; If we make it here, we've found the correct mode!
	; We're moving the current mode to the screen resource
	mov ax, [vbe_query.width]
	mov word[vbe_current_mode.width], ax
	
	
	mov ax, [vbe_query.height]
	mov word[vbe_current_mode.height], ax
	
	mov eax, [vbe_mode_block.framebuffer]
	mov dword[vbe_current_mode.framebuffer], eax
	
	mov ax, [vbe_mode_block.pitch]
	mov word[vbe_current_mode.pitch], ax
	
	mov eax, 0
	mov al, [vbe_query.bpp]
	mov byte[vbe_current_mode.bpp], al
	
	
	shr eax, 3
	mov dword[vbe_current_mode.bytes_per_pixel], eax
 
	mov ax, [vbe_query.width]
	shr ax, 3
	dec ax
	mov word[vbe_mode_block.x_char], ax
 
	mov ax, [vbe_query.height]
	shr ax, 4
	dec ax
	mov word[vbe_mode_block.y_char], ax
 
	; Set the mode
	push es
	mov ax, 0x4F02
	mov bx, [vbe_query.mode]
	or bx, 0x4000			; enable LFB
	mov di, 0			; not sure if some BIOSes need this... anyway it doesn't hurt
	int 0x10
	pop es
 
	cmp ax, 0x4F
	jne error
 
 ; Go to start
	jmp _start
 
.next_mode:
	mov ax, [vbe_query.t_segment]
	mov fs, ax
	mov si, [vbe_query.offset]
	jmp .find_mode
 
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
      mov ebx, vbe_current_mode.framebuffer
      call _testvbe
      jmp $


[BITS 16]

.rodata:

; VBE Variables
; width: dw 1920
; height: dw 1080
; width: dw 1024
; height: dw 768

vbe_current_mode: ; Current mode
   .height: dw 0
   .width: dw 0
   .framebuffer: dd 0
   .pitch: dw 0
   .bpp: db 0
   .bytes_per_pixel: dw 0

vbe_query: ; Preferred mode
   .width: dw 640
   .height: dw 480
   .bpp: db 32
   .offset: dw 0
   .t_segment: dw 0	; "segment" is keyword in fasm
   .mode: dw 0


vbe_info_block:		; 'Sector' 2
	.vbe_signature: db 'VBE2'
	.vbe_version: dw 0          ; Should be 0300h? BCD value
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

vbe_mode_block:	; 'Sector' 3
    ;; Mandatory info for all VBE revisions
	.mode_attributes: dw 0
	.window_a_attributes: db 0
	.window_b_attributes: db 0
	.window_granularity: dw 0
	.window_size: dw 0
	.window_a_segment: dw 0
	.window_b_segment: dw 0
	.window_function_pointer: dd 0
	.pitch: dw 0 ; bytes per scanline

    ;; Mandatory info for VBE 1.2 and above
	.width: dw 0
	.height: dw 0
	.x_char: db 0 ; unused
	.y_char: db 0 ; unused
	.planes: db 0
	.bpp: db 0 ; bits-per-pixel
	.banks: db 0
	.memory_model: db 0
	.bank_size: db 0
	.image_pages: db 0
	.reserved1: db 1

    ;; Direct color fields (required for direct/6 and YUV/7 memory models)
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
	.framebuffer: dd 0     ; Physical address for flat memory frame buffer
	.reserved2: dd 0
	.reserved3: dw 0

   .reserved4: times 206 db 0      ; Remainder of mode info block