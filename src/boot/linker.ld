SECTIONS {
    . = 0x1000;
    .text : {
        *(.text)
    }
    
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.rel.eh_frame)
        *(.note.gnu.property)
    }
}