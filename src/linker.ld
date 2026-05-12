OUTPUT_FORMAT("binary")

KERNEL_VMA = 0xC0000000;

SECTIONS {
    /* Stage 2 bootloader — runs before paging, needs physical addresses.
       VMA == LMA here (no paging yet). */
    . = 0x7E00;
    .boot : {
        *kernel_stub.elf(.text .data .rodata .bss)
        *(.boot .boot.*)
    }

    /* Kernel code — linked at higher-half virtual addresses.
       The binary is still loaded at the same physical spot by the bootloader,
       but the linker resolves all symbol addresses to 0xC0000000+. */

    /* advance VMA into the higher half */
    . += KERNEL_VMA;

    .text : AT(ADDR(.text) - KERNEL_VMA) {
        EXCLUDE_FILE(*kernel_stub.elf) *(.text)
    }

    .rodata : AT(ADDR(.rodata) - KERNEL_VMA) {
        EXCLUDE_FILE(*kernel_stub.elf) *(.rodata .rodata.*)
    }

    .data : AT(ADDR(.data) - KERNEL_VMA) {
        EXCLUDE_FILE(*kernel_stub.elf) *(.data .data.*)
    }

    .bss : AT(ADDR(.bss) - KERNEL_VMA) {
        EXCLUDE_FILE(*kernel_stub.elf) *(.bss .bss.*)
    }

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.rel.eh_frame)
        *(.note.gnu.property)
    }
}
