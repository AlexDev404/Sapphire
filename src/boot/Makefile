BUILD_DIR=out
BUILD=floppy
VENDOR=SPPH
KERNEL_BIN=kernel.bin
KERNEL_O=./objects/kernel.a
KERNEL_STUB=./objects/kernel_stub.elf
CHAINLOADER=./objects/bin/chainloader.bin

.PHONY: mbr test test2 clean clean-cargo always

mbr: chainloader/chainloader.asm always
	@echo.
	@echo.
	@echo Building bootloader (chainloader.elf)
	@echo.
	nasm -f bin chainloader/chainloader.asm -o ${CHAINLOADER}
	nasm -f elf32 chainloader/stage2.asm -o ${KERNEL_STUB}
	@echo.
	@echo.
	@echo Building kernel image (kernel.a)
	@echo.
	cargo build --release --lib
	mv ../../target/x86-unknown-bare-metal/release/libsapphire.a ${KERNEL_O}

	@echo Building debug symbols (sapphire.a)
	cargo build --bin sapphire
	mv ../../target/x86-unknown-bare-metal/debug/sapphire ./objects/sapphire.a

	@echo.
	@echo.
	@echo Building disk image (floppy.img)
	@echo.
	wsl ld -T linker.ld -no-PIE -no-PIC -m elf_i386 -o ${KERNEL_BIN} ${KERNEL_STUB} ${KERNEL_O} --oformat binary
	@echo "Works on my machine!"
	wsl cat ${CHAINLOADER} ${KERNEL_BIN} > image.bin
	wsl dd if=image.bin of=${BUILD_DIR}/${BUILD}.img conv=notrunc

kernel:
	@echo Building kernel image (kernel.a)
	cargo build --release
	mv ../../target/x86-unknown-bare-metal/release/libsapphire.a ./kernel.a

	@echo Moving debug symbols (sapphire.a)
	mv ../../target/x86-unknown-bare-metal/debug/sapphire ./objects/sapphire.a

	wsl ld -o ${KERNEL_BIN} -m elf_i386 -Ttext 0x1000 ${KERNEL_STUB} ${KERNEL_O} --oformat binary
	cat chainloader.bin ${KERNEL_BIN} > image.bin
	wsl dd if=image.bin of=${BUILD_DIR}/${BUILD}.img conv=notrunc

# Make example bootloader programs

# Example one
test: chainloader/example/sector_test.asm always
	nasm -f bin chainloader/example/sector_test.asm -o chainloader.bin
	dd if=chainloader.bin of=${BUILD_DIR}/${BUILD}.img conv=notrunc

# Example 2
test2: chainloader/example/oem_label.asm always
	nasm -f bin chainloader/example/oem_label.asm -o chainloader.bin
	dd if=chainloader.bin of=${BUILD_DIR}/${BUILD}.img conv=notrunc


test3: chainloader/tests/graphics.asm always
	nasm -f bin chainloader/tests/graphics.asm -o chainloader.bin
	wsl dd if=chainloader.bin of=${BUILD_DIR}/${BUILD}.img conv=notrunc

# Cleanup
clean:
	rm -rf *.bin
	rm -rf *.elf
	rm -rf *.a
	rm -rf out
	rm -rf objects

# Don't clean cargo by default (for faster builds)
clean-cargo:
	cargo clean

boot: out/floppy.img
	qemu-system-i386 -s -S -fda out/floppy.img

debug: build-gdb
	gdb "boot/image.bin" -ex "target remote :1234"

build-gdb: 

# Always clean and make the bootdisk
always: clean
	mkdir ${BUILD_DIR}
	mkdir objects
	mkdir "objects/bin"
	wsl dd if=/dev/zero of=${BUILD_DIR}/${BUILD}.img bs=512 count=2880
# wsl mkfs.fat -F 12 -n "${VENDOR}" $(BUILD_DIR)/${BUILD}.img
