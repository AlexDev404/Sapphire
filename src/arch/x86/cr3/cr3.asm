[global _read_cr3]
[global _write_cr3]

; extern "C" uint32_t _read_cr3()
_read_cr3:
		mov eax, cr3
		ret
; extern "C" void _write_cr3(uint32_t value)
_write_cr3:
		mov eax, [ebp+8]
		mov cr3, eax
		ret