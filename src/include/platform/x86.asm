[BITS 32]
[GLOBAL outportb]
[GLOBAL inportb]
; This file provides inline assembly functions for port I/O operations

; Output a byte to a port
; void outportb(unsigned short port, unsigned char value)
outportb:
	; Save old stack frame and initialize a new one
	push ebp
	mov ebp, esp

	; Get port number (first parameter) and value (second parameter)
	mov dx, [ebp + 8]    ; port parameter
	mov al, [ebp + 12]   ; value parameter
	out dx, al           ; output byte to port
	
	; Restore stack frame and return
	pop ebp
	ret

; Input a byte from a port
; unsigned char inportb(unsigned short port)
inportb:
	; Save old stack frame and initialize a new one
	push ebp
	mov ebp, esp
	
	; Get port number (first parameter)
	mov dx, [ebp + 8]    ; port parameter
	in al, dx            ; input byte from port
	
	; Return value is already in AL
	; Restore stack frame and return
	pop ebp
	ret
