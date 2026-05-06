BUILD_DIR=out
BUILD=floppy
OBJECTS_DIR=./objects
VENDOR=SPPH
KERNEL_BIN=kernel.bin
KERNEL_DEBUG_TRANSIENT=kernel.debug.transient
KERNEL_STUB=${OBJECTS_DIR}/kernel_stub.elf
CHAINLOADER=${OBJECTS_DIR}/bin/chainloader.bin

# C++ compiler settings
CXX=g++
ASM=nasm
# Removed O2 because it caused issues with debugging and optimization in a kernel context
CXXFLAGS=-ffreestanding -Wall -Wextra -fno-exceptions \
	-fno-rtti -m32 -fno-pic -mno-sse -mno-sse2 -mno-mmx -mno-80387 -g \
	-fno-pie -fno-inline -I./ -I./include

CPP_SOURCES=$(wildcard kernel/*.cpp kernel/**/*.cpp kernel/**/**/*.cpp \
                        arch/x86/**/*.cpp \
                        driver/**/*.cpp driver/**/**/*.cpp)
CPP_OBJECTS=$(patsubst %.cpp,${OBJECTS_DIR}/%.o,$(CPP_SOURCES))

ASM_SOURCES=$(wildcard arch/x86/**/*.asm boot/**/*.asm)
ASM_OBJECTS=$(patsubst %.asm,${OBJECTS_DIR}/%.asm.elf,$(ASM_SOURCES))

.PHONY: all clean boot boot-debug always

# Default target - build C++ kernel
all: kernel

# Main build target for C++ kernel
kernel: chainloader stage2 $(ASM_OBJECTS) $(CPP_OBJECTS)
	@echo Linking kernel image
	@wsl ld -T linker.ld --no-pie -m elf_i386 -o ${KERNEL_BIN} ${KERNEL_STUB} $(ASM_OBJECTS) $(CPP_OBJECTS)
	@echo [SYMBOLS]: Creating debug symbols
	@wsl ld -T linker.debug.ld --no-pie -m elf_i386 -o ${KERNEL_DEBUG_TRANSIENT} ${KERNEL_STUB} $(ASM_OBJECTS) $(CPP_OBJECTS)
	@echo [SYMBOLS]: Stripping symbols
	@wsl objcopy --only-keep-debug ${KERNEL_DEBUG_TRANSIENT} ${KERNEL_BIN}.sym
	@echo [SYMBOLS]: Removing transient debug file
	@wsl rm ${KERNEL_DEBUG_TRANSIENT}
	@echo Creating bootable image
	wsl cat ${CHAINLOADER} ${KERNEL_BIN} > image.bin
	wsl dd if=image.bin of=${BUILD_DIR}/${BUILD}.img conv=notrunc

# Compile stage 1 bootloader
chainloader: boot/stage1.asm always
	@echo Building bootloader
	@$(ASM) -f bin boot/stage1.asm -o ${CHAINLOADER}

# Compile stage 2 bootloader
stage2: boot/stage2.asm always
	@echo Building stage2
	@$(ASM) -f elf32 -F dwarf -g boot/stage2.asm -o ${KERNEL_STUB}
	@echo.
	@echo Building kernel

# Compile C++ source files
${OBJECTS_DIR}/%.o: %.cpp
	@echo [CXX]: Compiling $<
	@wsl mkdir -p $(dir $@)
	@wsl $(CXX) $(CXXFLAGS) -c $< -o $@

# Compile ASM source files
${OBJECTS_DIR}/%.asm.elf: %.asm
	@echo [ASM]: Compiling $<
	@wsl mkdir -p $(dir $@)
	@$(ASM) -f elf32 -F dwarf -g $< -o $@

# Cleanup
clean:
	@echo Cleaning up...
	@echo.
	rm -rf *.bin
	rm -rf *.elf
	rm -rf *.a
	rm -rf out
	rm -rf objects
	rm -rf tools
	@echo.
	@echo Project Structure: Clean.
	@echo.
	@echo.

# Boot with QEMU
boot: out/floppy.img
	qemu-system-i386 -debugcon stdio -s -fda out/floppy.img

boot-monitor: out/floppy.img
	qemu-system-i386 -monitor stdio -s -fda out/floppy.img

boot-debug: out/floppy.img
	qemu-system-i386 -debugcon stdio -s -S -fda out/floppy.img

# Debug support
debug: build-gdb
	gdb -ex "file ${KERNEL_BIN}.sym" -ex "set architecture i386" -ex "set osabi none" -ex "target remote :1234"

build-gdb:
	@echo "Preparing debug session"

# Create directories and setup empty floppy image
always: clean
	@echo Preparing...
	@echo.
	mkdir ${BUILD_DIR}
	mkdir tools
	mkdir objects
	mkdir "objects/bin"
	wsl dd if=/dev/zero of=${BUILD_DIR}/${BUILD}.img bs=512 count=2880
	@echo.
	@echo Project Structure: Ready.
	@echo.
	@echo.
