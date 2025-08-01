[BITS 32]
[GLOBAL isr0_handler]
[EXTERN isr0_c]

; Interrupt Service Routine for interrupt 0 (divide by zero)

isr0_handler:
	cli                 ; Disable interrupts
	pusha               ; Save all registers
	call isr0_c         ; Call the C handler
	popa                ; Restore all registers
	sti                 ; Re-enable interrupts
	add esp, 16         ; Clean up the stack (vector number + EFLAGS)
	iret                ; Return from interrupt
