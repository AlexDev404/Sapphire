%DEFINE KERNEL_LOAD_OFFSET 0x1000

org 0x7c00

bits 16

xor ax, ax
mov ds, ax

start: jmp main

Print:
 .print:
  lodsb
  or al, al
  je .done

  mov ah, 0x0E
  int 0x10

  .repeat:
   jmp .print

 .done:
  ret

ResetFloppy:
 mov ah, 0x0

 int 0x13
 jc ErrorFloppy
 .done:
  ret

ReadFloppy:

 mov ah, 0x02
 int 0x13

 jc ErrorFloppy

 .done:
  ret

ErrorFloppy:

 mov si, msgErrorFloppy
 call Print

 jmp hang


main: 
 .print:

  mov si, msg
  call Print

 .loadFile:
  mov al, 0xF
  mov ch, 0x0
  mov cl, 0x02
  mov dh, 0x0
  mov dl, 0x0
  mov bx, KERNEL_LOAD_OFFSET

  call ResetFloppy
  call ReadFloppy


 .loadGDT:
   lgdt [gdtr]
 .pM_start:
  cli
  pusha

  mov eax, cr0
  or al, 1
  mov cr0, eax

  popa

  jmp 0x08:ljmp_pM


 bits 32
 ljmp_pM: 
  mov ax, 0x10
  mov ds, ax
  mov ss, ax
  mov fs, ax
  mov es, ax
  mov gs, ax

        mov edi, 0xb8000    ; The video address
        mov bl, '.'         ; The character to be print
        mov dl, bl        ; The color: white(F) on black(0)
        mov dh, 63
        mov word [edi], dx        ; Put the character into the video memory
  ; jmp KERNEL_LOAD_OFFSET

hang:
 jmp $

gdt:
NULL_DESC:
    dd 0            ; null descriptor
    dd 0

CODE_DESC:
    dw 0xFFFF       ; limit low
    dw 0            ; base low
    db 0            ; base middle
    db 10011010b    ; access
    db 11001111b    ; granularity
    db 0            ; base high

DATA_DESC:
    dw 0xFFFF       ; data descriptor
    dw 0            ; limit low
    db 0            ; base low
    db 10010010b    ; access
    db 11001111b    ; granularity
    db 0            ; base high

gdtr:
    Limit dw 24         ; length of GDT
    Base dd NULL_DESC   ; base of GDT

msg db "Loading OS", 13, 10, 0
msgErrorFloppy db "There was an error with the floppy", 13, 10, 0

FILL:
 times 510-($-$$) db 0

BOOTSECTOR:
 dw 0xAA55