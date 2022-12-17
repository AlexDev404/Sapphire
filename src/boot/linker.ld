SECTIONS {
    . = 0x7C00;
    .text : {
        /* *(.text) */
        chainloader.elf(.text)
    }
    . = 0x100000;
    .text : {
        kernel.a(.text)
    }
    
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.rel.eh_frame)
        *(.note.gnu.property)
    }
}