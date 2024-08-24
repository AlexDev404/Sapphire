ENTRY(_start)

SECTIONS
{
    . = 0x100000;
    .text : {
        *(.text.init)
        *(.text .text.*)
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
}

