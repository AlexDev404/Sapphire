; -----------------------------------------------------------------------------
; Stage 2 bootloader
;
; Loaded by stage1 at physical 0x7E00 (right after the boot sector).
; We get here via `jmp 0x0000:0x7E00`, so CS = 0, IP = 0x7E00.
;
; Job:
;   1. Set up VBE video mode and capture mode info
;   2. Enable A20
;   3. Load a bootstrap GDT (null + code + data)
;   4. Switch to 32-bit protected mode
;   5. Call kmain(vbe_mode_block)
;
; Once kmain runs, it can install its own real GDT/IDT and forget this one.
; -----------------------------------------------------------------------------

; NOTE: This file has no [ORG] directive. The linker script must place this
; section's base at 0x7E00 (or wherever the chainloader loads it). All label
; references are resolved at link time.

[BITS 16]

[GLOBAL vbe_mode_block]
[EXTERN kmain]

%define VBE_MODE  0x115        ; 800x600 24bpp
%define VBE_LFB   0x4000       ; "use linear framebuffer" bit
%define CODE_SEL  0x08
%define DATA_SEL  0x10
%define PM_STACK  0x90000      ; 32-bit stack top (576 KB)

; -----------------------------------------------------------------------------
; Entry point — must be the very first byte at 0x7E00.
; -----------------------------------------------------------------------------
_entry:
	; Stage1 left CS=0, but DS/ES are whatever it had.
	; Zero them so absolute label addresses (which include 0x7E00) work as-is.
	xor ax, ax
	mov ds, ax
	mov es, ax

	; Real-mode stack just below us, like stage1 used.
	mov ss, ax
	mov sp, 0x7C00

	; --- VBE: get controller info ---
	mov ax, 0x4F00
	mov di, vbe_info_block
	int 0x10

	; --- VBE: get mode info ---
	mov ax, 0x4F01
	mov cx, VBE_MODE
	mov di, vbe_mode_block
	int 0x10

	; --- VBE: set mode (with linear framebuffer) ---
	mov ax, 0x4F02
	mov bx, VBE_MODE | VBE_LFB
	int 0x10

	; --- A20 via fast gate (port 0x92) ---
	in al, 0x92
	test al, 2
	jnz .a20_done           ; already on, leave it alone
	or al, 2                ; set A20 enable bit
	and al, 0xFE            ; make sure system-reset bit is clear
	out 0x92, al
.a20_done:

	; --- Switch to protected mode ---
	cli                     ; no interrupts until kernel sets up an IDT
	lgdt [gdtr]

	mov eax, cr0
	or eax, 1               ; PE bit
	mov cr0, eax

	; Far jump to flush CS into the new code segment and enter 32-bit code.
	jmp CODE_SEL:pmode_entry


[BITS 32]
; -----------------------------------------------------------------------------
; First 32-bit instruction. CS now references the bootstrap code descriptor.
; -----------------------------------------------------------------------------
pmode_entry:
	; Reload data segments with the bootstrap data descriptor
	mov ax, DATA_SEL
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, PM_STACK

	; Call kmain(&vbe_mode_block) using cdecl
	; push dword vbe_mode_block
	call kmain
	;add esp, 4

	; If kmain ever returns, hang
.hang:
	cli
	hlt
	jmp .hang


; -----------------------------------------------------------------------------
; Bootstrap GDT — minimal flat-model setup. The kernel will replace this
; with its own GDT (with TSS, userland descriptors, etc.) once it's running.
; -----------------------------------------------------------------------------
align 8
gdt:
	; 0x00 — null
	dq 0
	; 0x08 — code: base=0, limit=4GB, ring 0, exec/read, 32-bit
	dw 0xFFFF, 0x0000
	db 0x00, 10011010b, 11001111b, 0x00
	; 0x10 — data: base=0, limit=4GB, ring 0, read/write, 32-bit
	dw 0xFFFF, 0x0000
	db 0x00, 10010010b, 11001111b, 0x00
gdt_end:

gdtr:
	dw gdt_end - gdt - 1
	dd gdt


; -----------------------------------------------------------------------------
; VBE buffers (filled in by BIOS during real-mode setup above)
; -----------------------------------------------------------------------------
vbe_info_block:
	.vbe_signature:                db 'VBE2'
	.vbe_version:                  dw 0
	.oem_pointer:                  dd 0
	.capabilities:                 dd 0
	.video_modes:                  dd 0
	.total_memory:                 dw 0
	.oem_software_rev:             dw 0
	.oem_vendor_name_pointer:      dd 0
	.oem_product_name_pointer:     dd 0
	.oem_product_revision_pointer: dd 0
	.reserved:                     times 222 db 0
	.oem_data:                     times 256 db 0

vbe_mode_block:
	.mode_attributes:        dw 0
	.window_a_attributes:    db 0
	.window_b_attributes:    db 0
	.window_granularity:     dw 0
	.window_size:            dw 0
	.window_a_segment:       dw 0
	.window_b_segment:       dw 0
	.window_function_pointer:dd 0
	.pitch:                  dw 0
	.width:                  dw 0
	.height:                 dw 0
	.x_char:                 db 0
	.y_char:                 db 0
	.planes:                 db 0
	.bpp:                    db 0
	.banks:                  db 0
	.memory_model:           db 0
	.bank_size:              db 0
	.image_pages:            db 0
	.reserved1:              db 1
	.red_mask:               db 0
	.red_field:              db 0
	.green_mask:             db 0
	.green_field:            db 0
	.blue_mask:              db 0
	.blue_field:             db 0
	.reserved_mask:          db 0
	.reserved_field:         db 0
	.direct_color_mode_info: db 0
	.framebuffer:            dd 0
	.reserved2:              dd 0
	.reserved3:              dw 0
	.reserved4:              times 206 db 0