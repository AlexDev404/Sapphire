KERNEL_VMA = 0xC0000000;

SECTIONS {
    . = 0x7E00;
    .boot : {
        *kernel_stub.elf(.text .data .rodata .bss)
        *(.boot .boot.*)
    }

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

    /* Include debug sections */
    .debug_info       0 : { *(.debug_info) }
    .debug_abbrev     0 : { *(.debug_abbrev) }
    .debug_loc        0 : { *(.debug_loc) }
    .debug_aranges    0 : { *(.debug_aranges) }
    .debug_ranges     0 : { *(.debug_ranges) }
    .debug_line       0 : { *(.debug_line) }
    .debug_str        0 : { *(.debug_str) }
    .debug_frame      0 : { *(.debug_frame) }

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.rel.eh_frame)
        *(.note.gnu.property)
    }
}
