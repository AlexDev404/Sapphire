global _start
[BITS 32]
[EXTERN _rust]

_start:
  call _rust
  jmp $