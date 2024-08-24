SECTIONS {
    . = 0x1000;
    .text : {
        *(.text)
    }
    .data : {
        *(.data .data.*)
        }
    .rodata : {
        *(.rodata .rodata.*)
    }
    .bss : {
        *(.bss .bss.*)
    }
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.rel.eh_frame)
        *(.note.gnu.property)
    }
}