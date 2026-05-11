[BITS 32]

; =============================================================================
; _load_idt — loads the IDT register (same idea as _load_gdt)
; =============================================================================
[GLOBAL _load_idt]

; void _load_idt(IDTDescriptor *idt)
_load_idt:
	; TODO: Load the IDT
	; 1. Get the pointer from the stack: mov eax, [esp+4]
	; 2. Load it: lidt [eax]
	; 3. Return: ret
	; ... your code here ...

; =============================================================================
; ISR stubs for CPU exceptions (interrupts 0-31)
;
; Some exceptions push an error code automatically, some don't.
; To keep the stack layout consistent, we push a dummy 0 for the ones that don't.
;
; The pattern for each stub:
;   1. push 0          (dummy error code — ONLY for exceptions that don't push one)
;   2. push <int_num>  (so the C++ handler knows which interrupt fired)
;   3. jmp isr_common  (save registers, call C++ handler)
;
; Exceptions that push their OWN error code (do NOT push dummy 0):
;   8 (Double Fault), 10 (Invalid TSS), 11 (Segment Not Present),
;   12 (Stack Fault), 13 (GPF), 14 (Page Fault), 17 (Alignment Check),
;   21 (Control Protection), 29 (VMM Communication), 30 (Security Exception)
; =============================================================================

; Macro for ISR that does NOT have an error code (most of them)
; TODO: Define this macro
;   1. Make the label global: [GLOBAL isr%1]
;   2. Create the label: isr%1:
;   3. Push dummy error code: push 0
;   4. Push the interrupt number: push %1
;   5. Jump to common handler: jmp isr_common
;
; Usage: ISR_NO_ERR 0
;        ISR_NO_ERR 1
;        etc.
%macro ISR_NO_ERR 1
	; ... your code here ...
%endmacro

; Macro for ISR that DOES have an error code (CPU pushes it automatically)
; TODO: Define this macro
;   Same as above but WITHOUT the dummy push (the CPU already pushed the error code)
;   1. [GLOBAL isr%1]
;   2. isr%1:
;   3. push %1          (just the interrupt number — error code is already on stack)
;   4. jmp isr_common
%macro ISR_HAS_ERR 1
	; ... your code here ...
%endmacro

; TODO: Use the macros to create stubs for ISR 0-31
; Exceptions WITHOUT error code: 0-7, 9, 15, 16, 18-20, 22-28, 31
; Exceptions WITH error code:    8, 10, 11, 12, 13, 14, 17, 21, 29, 30
;
; Example:
;   ISR_NO_ERR 0
;   ISR_NO_ERR 1
;   ...
;   ISR_HAS_ERR 8
;   ISR_NO_ERR 9
;   ISR_HAS_ERR 10
;   ...
; ... your code here ...

; =============================================================================
; IRQ stubs for hardware interrupts (IRQs 0-15 → interrupts 32-47)
;
; IRQs never have error codes, so they all push a dummy 0.
; They push the ACTUAL interrupt number (32+), not the IRQ number.
; =============================================================================

; Macro for IRQ stubs
; %1 = IRQ number (0-15), %2 = interrupt number (32-47)
; TODO: Define this macro
;   1. [GLOBAL irq%1]
;   2. irq%1:
;   3. push 0            (dummy error code)
;   4. push %2           (interrupt number = IRQ + 32)
;   5. jmp isr_common
%macro IRQ 2
	; ... your code here ...
%endmacro

; TODO: Use the IRQ macro for IRQs 0-15
; Example:
;   IRQ 0, 32
;   IRQ 1, 33
;   ...
;   IRQ 15, 47
; ... your code here ...

; =============================================================================
; Common handler — saves registers, calls C++ isr_handler, restores, returns
;
; At this point the stack looks like:
;   [esp+0]  interrupt number  (pushed by our stub)
;   [esp+4]  error code        (pushed by CPU or our dummy 0)
;   [esp+8]  EIP               (pushed by CPU)
;   [esp+12] CS                (pushed by CPU)
;   [esp+16] EFLAGS            (pushed by CPU)
; =============================================================================
[EXTERN isr_handler]

isr_common:
	; TODO: Save all general-purpose registers
	; Hint: pusha pushes EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI in one instruction

	; TODO: Pass a pointer to the InterruptFrame to the C++ handler
	; Hint: after pusha, ESP points to the start of the InterruptFrame struct
	;   push esp           ; pass pointer to frame as argument
	;   call isr_handler   ; call C++ handler
	;   add esp, 4         ; clean up the argument we pushed

	; TODO: Restore all general-purpose registers
	; Hint: popa (reverse of pusha)

	; TODO: Clean up the interrupt number and error code from the stack
	; Hint: add esp, 8  (skip over the 2 dwords we pushed)

	; TODO: Return from interrupt
	; Hint: iret (NOT ret — iret restores EIP, CS, and EFLAGS from the stack)

	; ... your code here ...
