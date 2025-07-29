[GLOBAL _start]
[BITS 16]
[EXTERN kmain]  ; Changed from _rust to kmain

; %DEFINE MODE 103h  ; Test mode with 256 colors (palette-based)
%DEFINE MODE 115h  ; 16.8M colors

graphics: ; Using the Real Mode interface
    ; Get VBE information
    mov ax, 4F00h
    mov di, vbe_info_block ; Pointer to buffer
    int 10h

    ; Get video mode info for the one we want
    mov ax, 4F01h
    ; mov cx, 101h ; First mode
    mov cx, MODE ; First mode
    mov di, vbe_mode_block
    int 10h

    ; Set video mode to 101h (640x480)
    mov ah, 0
    mov ax, 4F02h
    mov bx, MODE ; Mode 101h
    int 10h
	
	; mov dx, 0xE9 ; Debug port
	; mov al, 41h  ; Letter 'A'
	; out dx, al   ; Send letter 'A' to debug port 0xE9

	jmp _start

_start:
	cli                          ; Disable interrupts
	pusha
	mov eax, cr0
	or al, 1                     ; Set PE (Protection Enable) bit in CR0 (Control Register 0)
	mov cr0, eax
	popa
	; Now that Protected Mode is enabled, we safely access the GDT
	; load DS, ES, FS, GS, SS, ESP
	; Flush GDT + Initialize it + load segment registers
    ; Initialize the data segment
    mov eax, 0x10          ; Data segment selector (0x10 for the second segment)
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax

    ; Initialize the stack segment
    mov eax, 0x18          ; Stack segment selector (0x18 for the third segment)
    mov ss, eax            ; Set the stack segment register

    ; Initialize the stack pointer (set to a safer location with more space)
    ; Place stack at 0x00090000 (576KB) with 32KB of stack space growing downward
    ; This is safely above the kernel (loaded at 0x1000) and VBE structures
    mov esp, 0x00090000    ; Set the stack pointer to a better location

	; END ENABLE PROTECTED MODE - INTERRUPTS INACCESSIBLE

	; Perform far jump to selector 0x8 (offset into GDT, pointing at a 32bit PM code segment descriptor)
	; to load CS with proper PM32 descriptor)
	jmp long 0x8:PModeMain        ; Jump to Protected Mode Main in the code segment

[BITS 32]
PModeMain:
	; JUMP TO KERNEL
	; pixel_offset = y * pitch + ( x * ( bpp / 8 )) + framebuffer;

	; Set up the stack
	sub esp, 4   ; Align the stack before pushing the argument
	; Load the address of `vbe_mode_block` into EAX
	; We pass this address and then convert it into a pointer later
    lea eax, [vbe_mode_block]
    
    ; Push the address onto the stack
	; We use the C calling convention which grabs whatever's on the stack as the arguments
    ; Since this uses the C calling convention we call push the arguments in reverse order
    ; and the first argument is at the top of the stack
    push eax
	
	; Call the C++ kernel entry point (`kmain`) with vbe_mode_block as argument
    call kmain  ; Entrypoint for the kernel

    ; Infinite loop after returning from the kernel
    jmp $

; PADDING
;times 512 - ($ - $$) db 0

rodata:

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
