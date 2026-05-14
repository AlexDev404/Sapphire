[BITS 32]
; This file provides inline assembly functions for port I/O operations

; Output a byte to a port
; void outb(uint16_t port, uint8_t value)
[GLOBAL outb]
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
; uint8_t inb(uint16_t port)
[GLOBAL inb]
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
; Output a long to a port
; uint32_t outl(uint16_t port, uint32_t value)
[GLOBAL outl]
outl:
	; Save old stack frame and initialize a new one
	push ebp
	mov ebp, esp

	; Get port number (first parameter) and value (second parameter)
	mov dx, [ebp + 8]    ; port parameter
	mov eax, [ebp + 12]   ; value parameter
	out dx, eax           ; output long to port
	
	; Restore stack frame and return
	pop ebp
	ret
; Input a long from a port
; uint32_t inl(uint16_t port)
[GLOBAL inl]
inl:
	; Save old stack frame and initialize a new one
	push ebp
	mov ebp, esp

	; Get port number (first parameter)
	mov dx, [ebp + 8]    ; port parameter
	in eax, dx            ; input long from port

	; Restore stack frame and return
	pop ebp
	ret
; Input a word from a port
; uint16_t inw(uint16_t port)
[GLOBAL inw]
inw:
	; Save old stack frame and initialize a new one
	push ebp
	mov ebp, esp

	; Get port number (first parameter)
	mov dx, [ebp + 8]    ; port parameter
	in ax, dx            ; input word from port

	; Restore stack frame and return
	pop ebp
	ret
; Output a word to a port
; void outw(uint16_t port, uint16_t value)
[GLOBAL outw]
outw:
	; Save old stack frame and initialize a new one
	push ebp
	mov ebp, esp

	; Get port number (first parameter) and value (second parameter)
	mov dx, [ebp + 8]    ; port parameter
	mov ax, [ebp + 12]   ; value parameter
	out dx, ax           ; output word to port

	; Restore stack frame and return
	pop ebp
	ret
