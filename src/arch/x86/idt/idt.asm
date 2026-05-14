[BITS 32]

; =============================================================================
; _load_idt — loads the IDT register (same idea as _load_gdt)
; =============================================================================
[GLOBAL _load_idt]

; void _load_idt(IDTDescriptor *idt)
_load_idt:
	mov eax, [esp+4]
	lidt [eax]
	ret

; =============================================================================
; ISR stubs for CPU exceptions (interrupts 0-31)
;
; Some exceptions push an error code automatically, some don't.
; To keep the stack layout consistent, we push a dummy 0 for the ones that don't.
; =============================================================================

; Macro for ISR that does NOT have an error code (most of them)
%macro ISR_NO_ERR 1
[GLOBAL isr%1]
isr%1:
	push 0          ; dummy error code
	push %1         ; interrupt number
	jmp isr_common
%endmacro

; Syscall handler (interrupt 128)
; also doesn't have an error code
; but we want to pass 128 as the interrupt number
[GLOBAL isr128]
isr128:
	push 0          ; dummy error code
	push 128        ; interrupt number (for syscall)
	jmp isr_common

; Macro for ISR that DOES have an error code (CPU pushes it automatically)
%macro ISR_HAS_ERR 1
[GLOBAL isr%1]
isr%1:
	push %1         ; interrupt number (error code already on stack)
	jmp isr_common
%endmacro

; CPU exceptions 0-31
ISR_NO_ERR  0   ; Division by Zero
ISR_NO_ERR  1   ; Debug
ISR_NO_ERR  2   ; Non-Maskable Interrupt
ISR_NO_ERR  3   ; Breakpoint
ISR_NO_ERR  4   ; Overflow
ISR_NO_ERR  5   ; Bound Range Exceeded
ISR_NO_ERR  6   ; Invalid Opcode
ISR_NO_ERR  7   ; Device Not Available
ISR_HAS_ERR 8   ; Double Fault
ISR_NO_ERR  9   ; Coprocessor Segment Overrun
ISR_HAS_ERR 10  ; Invalid TSS
ISR_HAS_ERR 11  ; Segment Not Present
ISR_HAS_ERR 12  ; Stack-Segment Fault
ISR_HAS_ERR 13  ; General Protection Fault
ISR_HAS_ERR 14  ; Page Fault
ISR_NO_ERR  15  ; Reserved
ISR_NO_ERR  16  ; x87 FP Exception
ISR_HAS_ERR 17  ; Alignment Check
ISR_NO_ERR  18  ; Machine Check
ISR_NO_ERR  19  ; SIMD FP Exception
ISR_NO_ERR  20  ; Virtualization Exception
ISR_HAS_ERR 21  ; Control Protection
ISR_NO_ERR  22  ; Reserved
ISR_NO_ERR  23  ; Reserved
ISR_NO_ERR  24  ; Reserved
ISR_NO_ERR  25  ; Reserved
ISR_NO_ERR  26  ; Reserved
ISR_NO_ERR  27  ; Reserved
ISR_NO_ERR  28  ; Hypervisor Injection
ISR_HAS_ERR 29  ; VMM Communication
ISR_HAS_ERR 30  ; Security Exception
ISR_NO_ERR  31  ; Reserved

; =============================================================================
; IRQ stubs for hardware interrupts (IRQs 0-15 → interrupts 32-47)
; =============================================================================

%macro IRQ 2
[GLOBAL irq%1]
irq%1:
	push 0          ; dummy error code
	push %2         ; interrupt number (32+)
	jmp isr_common
%endmacro

IRQ  0, 32  ; Timer
IRQ  1, 33  ; Keyboard
IRQ  2, 34  ; Cascade (slave PIC)
IRQ  3, 35  ; COM2
IRQ  4, 36  ; COM1
IRQ  5, 37  ; LPT2
IRQ  6, 38  ; Floppy
IRQ  7, 39  ; LPT1 / Spurious
IRQ  8, 40  ; RTC
IRQ  9, 41  ; Free
IRQ 10, 42  ; Free
IRQ 11, 43  ; Free
IRQ 12, 44  ; PS/2 Mouse
IRQ 13, 45  ; FPU
IRQ 14, 46  ; Primary ATA
IRQ 15, 47  ; Secondary ATA

; =============================================================================
; Common handler — saves registers, calls C++ isr_handler, restores, returns
; =============================================================================
[EXTERN isr_handler]

isr_common:
	pusha               ; save all general-purpose registers

	push esp            ; pass pointer to InterruptFrame as argument
	call isr_handler    ; call C++ handler
	add esp, 4          ; clean up argument

	popa                ; restore registers
	add esp, 8          ; clean up interrupt number + error code
	iret                ; return from interrupt
