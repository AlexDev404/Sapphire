[BITS 32]
[GLOBAL thing]
[EXTERN send_debug]


thing:
	; Push the call frame and create a new one
	push ebp
	mov ebp, esp

	mov eax, [ebp + 8]    ; This is our x-coordinate

	;mov ecx, [0xFD000000] ; This is our hardcoded framebuffer
	mov ecx, [eax]

	mov edx, 1            ; Use EDX for loop counter
	mov ax, 0x0F          ; Pixel Color. We chose red

	mov bx, [def_byte]

	call .loop_for_a_bit

	mov eax, [ebp + 12]   ; This is our y-coordinate


	pop ebp               ; Restore old call frame
	ret                   ; Exit and give control back to handler


	.loop_for_a_bit:
		add ecx, edx
		;add ebx, 180050; pixel_offset = y * pitch + ( x * ( bpp / 8 )) + framebuffer;
		mov [ecx], ax

		; char c
		push bx
		; extern "C" void interrupt_debug_char(char c);  // Interrupt-context safe debug output
		call send_debug
		; mov ecx, [ebx]
		; mov ebx, 0xFF0000
		; add bx, 1
		add edx, 1
		cmp edx, 18
		jne .loop_for_a_bit
		ret

def_byte:
	dw 'TESTESTESTESTFETESTESTESTEST'