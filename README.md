# Sapphire

The Sapphire Operating System

![GitHub top language](https://img.shields.io/github/languages/top/alexdev404/sapphire?logo=c&label=)
[![GitHub license](https://img.shields.io/github/license/alexdev404/sapphire)](https://github.com/alexdev404/sapphire/blob/master/LICENSE)
[![GitHub contributors](https://img.shields.io/github/contributors/alexdev404/sapphire)](https://github.com/alexdev404/sapphire/graphs/contributors)
[![GitHub commit activity](https://img.shields.io/github/commit-activity/m/alexdev404/sapphire)](https://github.com/alexdev404/sapphire/commits)

## To Build [WIP]

## Depends on

- C++ compiler (GCC or equivalent) that supports the `-no-PIE` flag
- NASM assembler
- If you're using Windows...
  - To build ensure that NASM is installed by running `winget install nasm` and add it to your PATH variable
  - Also, to get the GCC binaries required to build and use the Makefile, you can install MinGW-w64 or download them from [FPC Pascal binaries](https://www.freepascal.org/down/x86_64/win64-hungary.html).
  - WSL (Windows Subsystem for Linux) is used by the Makefile for some commands

### Main Project

> The latest Makefile will always be the Windows one. So you can `cd` into `/src/boot` and just run `make` and `make boot` without any issues.

```shell
# For Linux environments, install dependencies
sudo apt-get update && sudo apt-get install nasm binutils dosfstools make qemu qemu-utils qemu-system-x86 qemu-system-gui build-essential g++

# Build and run
cd src/boot
make        # Build the kernel
make boot   # Run in QEMU
```

## Development

The Sapphire OS is a freestanding C++ kernel with the following features:
- Custom bootloader with VBE graphics mode initialization
- Text rendering with IBM VGA fonts
- Basic graphics primitives for drawing shapes and UI elements
- Support for multiple color depths

The codebase is organized as follows:
- `src/boot` - Contains bootloader and build system
- `src/graphics` - Graphics rendering code including fonts and text display
- `src/main.cpp` - Kernel entry point

## Base x86 TODO

- [x] Build
- [ ] Locking
- [ ] GFX (flanterm)
- [ ] PMM 
- [ ] Paging
- [ ] Some heap
- [x] GDT
- [x] IDT
- [ ] TSS
- [ ] ACPI
- [ ] HPET
- [ ] LAPIC 
- [ ] MP
- [ ] IOAPIC
- [ ] Initrd support
- [ ] ELF parsing
- [ ] Scheduling
- [ ] End of x86 pain

## Unix-like TODO

- [ ] Syscalls
- [ ] Start libc porting
- [ ] End libc porting

## Porting TODO

- [ ] bash
- [ ] coreutils
- [ ] doom2
- [ ] nasm
- [ ] gcc
- [ ] lua
- [ ] python
- [ ] etc. apps

## Driver TODO

- [ ] IO
- [x] Serial
- [ ] PCI
- [ ] CMOS
- [ ] PS2 Keyboard 
- [ ] Speaker
- [ ] AHCI
- [ ] XHCI/HID/USB
- [ ] NVME
- [ ] Networking
- [ ] Intel HDA
- [ ] End of driver pain

## Filesystem TODO

- [ ] FAT32
- [ ] VFS
- [ ] EXT4
- [ ] TMPFS
- [ ] End of filesystem pain

## Final TODO

- [ ] TBP
