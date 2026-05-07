[global _read_cr0]
[global _write_cr0]

; uint32_t _read_cr0(void)
; extern "C"
_read_cr0:
		mov eax, cr0
		ret
; void _write_cr0(uint32_t val)
; extern "C"
_write_cr0:
		mov eax, [esp+4]
		mov cr0, eax
		ret
