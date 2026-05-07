[global _read_cr0]
[global _write_cr0]

; extern "C" uint32_t _read_cr0(void)
_read_cr0:
		mov eax, cr0
		ret
; extern "C" void _write_cr0(uint32_t val)
_write_cr0:
		mov eax, [ebp+8]
		mov cr0, eax
		ret
