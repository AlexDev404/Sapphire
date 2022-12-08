.PHONY all mbr 2stage clean

all: chainloader/chainloader.asm 2stage/2stage.asm
	nasm -fbin -o chainloader.bin chainloader/chainloader.asm
	nasm -fbin -o 2stage.bin 2stage/2stage.asm

mbr: chainloader/chainloader.asm
	nasm -fbin -o chainloader.bin chainloader/chainloader.asm

2stage: 2stage/2stage.asm
	nasm -fbin -o 2stage.bin 2stage/2stage.asm

clean:
	rm -rf chainloader.bin
	rm -rf 2stage.bin