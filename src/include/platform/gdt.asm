[BITS 32]
[GLOBAL g_GDTLoad]

g_GDTLoad:
	push ebp 		; Save old call frame
	mov ebp, esp	; Initialize new call frame

	mov eax, [ebp + 8]		; Get GDT descriptor pointer
	lgdt [eax]				; Load GDT
	
	mov ax, [ebp + 16]		; Get data segment selector
	mov ds, ax				; Set data segment
	mov es, ax				; Set extra segment
	mov fs, ax				; Set FS segment
	mov gs, ax				; Set GS segment
	mov ss, ax				; Set stack segment
	
	push dword [ebp + 12]	; Push code segment selector
	push dword .reload		; Push return address
	retf					; Far return to reload CS
	
.reload:
	pop ebp					; Restore old call frame
	ret						; Return to caller