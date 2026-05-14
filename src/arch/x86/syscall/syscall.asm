[GLOBAL send_syscall]
; uint32_t send_syscall(uint32_t syscall_number, uint32_t arg1)
send_syscall:
	push ebx
	; Move the syscall number and arguments into the appropriate registers
	mov eax, [esp + 8]        ; syscall_number -> eax
	mov ebx, [esp + 12]       ; arg1 -> ebx
	; Trigger the syscall interrupt (0x80)
	int 0x80
	pop ebx
	ret