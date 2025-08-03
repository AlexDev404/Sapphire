[BITS 32]
[GLOBAL isr0_handler]
[EXTERN isr0_c]

[GLOBAL spurious_interrupt_handler]
[EXTERN spurious_interrupt_c]

[GLOBAL isr13_handler]
[EXTERN isr13_c]

[GLOBAL timer_interrupt_handler]
[EXTERN timer_interrupt_c]


; Interrupt Service Routine for interrupt 0 (divide by zero)

isr0_handler:
	cli                 ; Disable interrupts
	pusha               ; Save all registers
	call isr0_c         ; Call the C handler
	popa                ; Restore all registers
	sti                 ; Re-enable interrupts
	add esp, 16         ; Clean up the stack (vector number + EFLAGS)
	iret                ; Return from interrupt

; ISR 13 (General Protection Fault)
isr13_handler:
	cli
	; Error code has already been pushed
	push 13 ; Interrupt code number
	call isr13_c
	popa
	sti
	add esp, 16
	iret

; Spurious interrupt handler (vector 0xFF)
spurious_interrupt_handler:
	cli                 ; Disable interrupts
	pusha               ; Save all registers
	call spurious_interrupt_c ; Call the C handler
	popa                ; Restore all registers
	sti                 ; Re-enable interrupts
	iret                ; Return from interrupt (no stack cleanup needed for spurious)

; Timer interrupt handler (vector 0x20)
timer_interrupt_handler:
	cli                 ; Disable interrupts
	pusha               ; Save all registers
	call timer_interrupt_c ; Call the C handler
	popa                ; Restore all registers
	sti                 ; Re-enable interrupts
	iret                ; Return from interrupt
