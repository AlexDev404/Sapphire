[BITS 32]
[GLOBAL load_gdt]

; void CDECL load_gdt(GDTDescriptor *gdt, uint16_t code_sel, uint16_t data_sel);
load_gdt:
	push ebp
	mov ebp, esp

	; Load the new GDT
	mov eax, [ebp + 8]
	lgdt [eax]

	; Far return to reload CS with the new code selector
	movzx eax, word [ebp + 12]    ; zero-extend the 16-bit selector
	push eax
	push .reload
	retf

.reload:
	; Now CS is reloaded; reload data segments too
	mov ax, [ebp + 16]            ; data segment selector
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	pop ebp
	ret
