[global _read_cr3]
[global _write_cr3]

; uint32_t _read_cr3(void)
; extern "C"
_read_cr3:
		mov eax, cr3
		ret
; void _write_cr3(uint32_t value)
; extern "C"
_write_cr3:
		mov eax, [esp+4]
		mov cr3, eax
		ret