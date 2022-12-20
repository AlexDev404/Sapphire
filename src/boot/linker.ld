SECTIONS {
    . = 0x7C00;
    .text : {
        /* *(.text) */
        chainloader.elf(.text)
    }
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.rel.eh_frame)
        *(.note.gnu.property)
    }
}