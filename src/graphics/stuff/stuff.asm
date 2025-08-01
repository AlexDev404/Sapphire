[BITS 32]
[GLOBAL thing]


thing:
	; Push the call frame and create a new one
	push ebp
	mov ebp, esp

	mov eax, [ebp + 8]    ; This is our x-coordinate
	
	mov ebx, 0xFD000000 ; This is our hardcoded framebuffer

	mov eax, 1

	call .loop_for_a_bit

	mov eax, [ebp + 12]   ; This is our y-coordinate


	pop ebp               ; Restore old call frame
	ret                   ; Exit and give control back to handler


	.loop_for_a_bit:
		add ebx, eax
		mov ecx, [ebx]
		mov ebx, 0xFF0000
		add eax, 1
		cmp eax, 60
		jne .loop_for_a_bit
		ret
