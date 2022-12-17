BUILD_DIR=out
BUILD=floppy
VENDOR=SPPH

.PHONY: mbr test test2 clean always


mbr: chainloader/chainloader.asm always
	echo Building bootloader (chainloader.elf)
	nasm -f elf32 chainloader/chainloader.asm -o chainloader.elf
	echo Building kernel image (kernel.a)
	cargo build --release
	mv ../../target/x86-unknown-bare-metal/release/libsapphire.a ./kernel.a
	wsl ld -m elf_i386 -o image.elf chainloader.elf kernel.a
	objcopy -O binary -R .note -R .comment -R .note.gnu.property image.elf image.bin


# Make example boootloader programs

# Example one
test: chainloader/example/sector_test.asm always
	nasm -f bin chainloader/example/sector_test.asm -o chainloader.bin
	dd if=chainloader.bin of=${BUILD_DIR}/${BUILD}.img conv=notrunc

# Example 2
test2: chainloader/example/oem_label.asm always
	nasm -f bin chainloader/example/oem_label.asm -o chainloader.bin
	dd if=chainloader.bin of=${BUILD_DIR}/${BUILD}.img conv=notrunc

# Cleanup
clean:
	rm -rf *.bin
	rm -rf *.elf
	rm -rf *.a
	rm -rf out

# Don't clean cargo by default (for faster builds)
clean-cargo:
	cargo clean

# Always clean and make the bootdisk
always: clean
	mkdir ${BUILD_DIR}
	wsl dd if=/dev/zero of=${BUILD_DIR}/${BUILD}.img bs=512 count=2880
	wsl mkfs.fat -F 12 -n "${VENDOR}" $(BUILD_DIR)/${BUILD}.img
