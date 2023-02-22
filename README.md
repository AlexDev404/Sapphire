# Sapphire

The Sapphire Operating System

## To Build [WIP]

## Depends on

- [Rust](https://rustup.rs)
- GCC that supports the `-no-PIE` flag
- If you're using Windows to build ensure that NASM is installed by running `winget install nasm` and add it to your PATH variable

### Main Project

```shell
sudo apt-get update && sudo apt-get install nasm dosfstools make qemu qemu-utils qemu-system-x86 qemu-system-gui
cd src/boot
make mbr
```
