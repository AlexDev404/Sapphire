[GLOBAL _start]
[BITS 16]
[EXTERN _rust]

_start:
   cli            ; Disable interrupts
   pusha
   mov eax, cr0
   or al, 1       ; Set PE (Protection Enable) bit in CR0 (Control Register 0)
   mov cr0, eax
   popa
   ; END ENABLE PROTECTED MODE - INTERRUPTS INACCESSIBLE

   ; Perform far jump to selector 0x8 (offset into GDT, pointing at a 32bit PM code segment descriptor)
   ; to load CS with proper PM32 descriptor)
   jmp long 0x8:PModeMain ; Jump to Protected Mode Main in the code segment

   [BITS 32]
   PModeMain:
      ; load DS, ES, FS, GS, SS, ESP
      ; Flush GDT + Initialize it + load segment registers
      mov eax, 0x10 ; Initialize the segment descriptors with the data segment
      mov ds, eax
      mov es, eax
      mov fs, eax
      mov gs, eax
      mov ss, eax
      ; JUMP TO KERNEL
      call _rust
      jmp $
