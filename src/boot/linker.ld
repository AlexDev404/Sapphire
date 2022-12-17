SECTIONS {
    /* . = 0x7C00 */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.rel.eh_frame)
        *(.note.gnu.property)
    }
}