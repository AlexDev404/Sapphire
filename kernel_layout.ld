ENTRY(_start)

SECTIONS
{
    . = 0x100000;
    .text : {
        *(.text.init)
        *(.text .text.*)
    }
    .rodata : {
        *(.rodata .rodata.*)
    }
    .data : {
        *(.rodata .rodata.*)
        }
    .bss : {
        *(.bss .bss.*)
    }
}

