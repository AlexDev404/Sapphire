[global _read_cr4]
[global _write_cr4]

; uint32_t _read_cr4(void)
; extern "C"
_read_cr4:
		mov eax, cr4
		ret
; void _write_cr4(uint32_t value)
; extern "C"
_write_cr4:
		mov eax, [esp+4]
		mov cr4, eax
		ret