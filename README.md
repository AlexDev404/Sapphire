# Sapphire

The Sapphire Operating System

## To Build [WIP]

## Depends on

- [Rust](https://rustup.rs)
- GCC that supports the `-no-PIE` flag
- If you're using Windows...
  - To build ensure that NASM is installed by running `winget install nasm` and add it to your PATH variable
  - Also, to get the GCC binaries required to build and use the Makefile, weirdly enough they are available [in the FPC Pascal binaries](https://www.freepascal.org/down/x86_64/win64-hungary.html). Just download and install those as well (doesn't matter in which order).

### Main Project

```shell
sudo apt-get update && sudo apt-get install nasm binutils dosfstools make qemu qemu-utils qemu-system-x86 qemu-system-gui build-essential
cd src/boot
make mbr
make boot
```
