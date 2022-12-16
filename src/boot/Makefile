BUILD_DIR=out
BUILD=floppy
VENDOR=SPPH

.PHONY: all mbr 2stage clean always

all: chainloader/chainloader.asm 2stage/2stage.asm always
	# nasm -fbin -o chainloader.bin chainloader/chainloader.asm
	nasm -f bin chainloader/chainloader.asm -o chainloader.bin
	nasm -fbin -o 2stage.bin 2stage/2stage.asm

mbr: chainloader/chainloader.asm always
	nasm -f bin chainloader/chainloader.asm -o chainloader.bin
	echo dd if=chainloader.bin of=${BUILD_DIR}/${BUILD}.img conv=notrunc
	echo "Copy ELF into Binary"
	objcopy -O binary sapphire.elf kernel.bin
	wsl cat chainloader.bin kernel.bin | wsl dd if=/dev/stdin of=${BUILD_DIR}/${BUILD}.img conv=notrunc

2stage: 2stage/2stage.asm always
	nasm -fbin -o 2stage.bin 2stage/2stage.asm

test: chainloader/example/sector_test.asm always
	nasm -f bin chainloader/example/sector_test.asm -o chainloader.bin
	dd if=chainloader.bin of=${BUILD_DIR}/${BUILD}.img conv=notrunc

test2: chainloader/example/oem_label.asm always
	nasm -f bin chainloader/example/oem_label.asm -o chainloader.bin
	dd if=chainloader.bin of=${BUILD_DIR}/${BUILD}.img conv=notrunc

clean:
	rm -rf chainloader.bin
	rm -rf kernel.bin
	rm -rf 2stage.bin
	rm -rf out

always: clean
	mkdir ${BUILD_DIR}
	wsl dd if=/dev/zero of=${BUILD_DIR}/${BUILD}.img bs=512 count=2880
	wsl mkfs.fat -F 12 -n "${VENDOR}" $(BUILD_DIR)/${BUILD}.img
