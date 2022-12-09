[ bits	16 ]

[ org	0x7c00	]

start:          jmp loader

;*************************************************;
;	OEM Parameter block / BIOS Parameter Block
;*************************************************;

TIMES 0Bh-$+start DB 0

bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster: 	DB 1
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 	DB 2
bpbRootEntries: 	DW 224
bpbTotalSectors: 	DW 2880
bpbMedia: 	        DB 0xF0
bpbSectorsPerFAT: 	DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors:       DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused: 	        DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "MOS FLOPPY "
bsFileSystem: 	        DB "FAT12   "

;***************************************
;	Prints a string
;	DS=>SI: 0 terminated string
;***************************************

Print:
			lodsb
			or			al, al
			jz			PrintDone
			mov			ah,	0eh
			int			10h
			jmp			Print
PrintDone:
			ret

;*************************************************
;	Bootloader Entry Point
;*************************************************

loader:
mov ah, 0ch
mov al, 15
mov bh, 0
mov cx, 10
mov dx, 10
int 10h


.Reset:
	mov		ah, 0

	mov		dl, 0
	int		0x13
	jc		.Reset
	mov		ax, 0x1000
	mov		es, ax
	xor		bx, bx

	mov		ah, 0x02
	mov		al, 1
	mov		ch, 1
	mov		cl, 2
	mov		dh, 0
	mov		dl, 0
	int		0x13

	jmp		0x1000:0x0


times 510 - ($-$$) db 0
dw 0xAA55

; End of sector 1, beginning of sector 2 ---------------------------------


; org 0x1000

; cli
; hlt
