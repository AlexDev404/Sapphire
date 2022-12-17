SECTIONS {
    . = 0x7C00;
    /* .text : {
        *(.text)
    } */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.rel.eh_frame)
        *(.note.gnu.property)
    }
}