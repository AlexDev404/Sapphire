[GLOBAL vbe_block]
[EXTERN vbe_mode_block]

;vbe_block:
;	; Save the call frame and create a new one (Prologue)
;	push ebp
;	mov ebp, esp
;	; Return the pointer to the vbe_mode_block (Epilogue)
;	mov eax, vbe_mode_block
;	mov esp, ebp
;	pop ebp
;	ret

;extern "C" const VbeModeInfo* vbe_block();
vbe_block:
    mov eax, vbe_mode_block
    ret
