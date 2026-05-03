SECTIONS {
    . = 0x7E00;

    .text : {
        *(.text)
    }
    
    .rodata : {
        *(.rodata .rodata.*)
    }

    .data : {
        *(.data .data.*)
    }

    .bss : {
        *(.bss .bss.*)
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
