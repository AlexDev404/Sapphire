[BITS 32]
[GLOBAL outb]
[GLOBAL inb]
; This file provides inline assembly functions for port I/O operations

; Output a byte to a port
; void outb(unsigned short port, unsigned char value)
outb:
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
; unsigned char inb(unsigned short port)
inb:
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
