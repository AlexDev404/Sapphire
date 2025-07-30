[GLOBAL disk_read]
[GLOBAL hang]

%DEFINE SECTORS_TO_READ 40

; Utility file
		
hang:
	cli
	hlt
	; If for some cursed reason the CPU decides to exit anyway,
	; we jump back to hang
	jmp hang


Print:
	.print:
		lodsb
		or al, al
		je .done
		mov ah, 0x0E
		int 10h
		.repeat:
			jmp .print
		.done:
			ret
disk_read:
	; store all register values
	pusha
	; prepare data for reading the disk
	; al = number of sectors to read (1 - 128)
	; ch = track/cylinder number
	; dh = head number
	; cl = sector number
	mov dh, SECTORS_TO_READ
	mov ah, 0x02
	mov al, dh
	mov ch, 0x00
	mov dh, 0x00
	mov cl, 0x02
	int 13h
	; in case of read error
	; show the message about it
	jc disk_read_error

	; check if we read expected count of sectors
	; if not, show the message with error
	popa
	cmp dh, al
	jne disk_read_error

	; Return out of the function
	ret
	disk_read_error:
		mov si, DISK_READ_ERROR
		call Print
		jmp hang
