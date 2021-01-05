SECTIONS
{
        . = 0;
        _start = .;
        .head : {
                KEEP(*(.head))
        }
        . = ALIGN(0x80);
        .text : { *(.text) *(.text.*) *(.sfpr) *(.eh_frame) *(.rodata) *(.rodata.*) }
        . = ALIGN(0x80);
        .data : { *(.data) *(.data.*) *(.got) *(.toc) }
        . = ALIGN(0x80);
        __bss_start = .;
        .bss : {
                *(.dynsbss)
                *(.sbss)
                *(.scommon)
                *(.dynbss)
                *(.bss)
                *(.common)
                *(.bss.*)
        }
        . = ALIGN(0x80);
        __bss_end = .;
        . = . + 0x4000;
        __stack_top = .;

        /DISCARD/ :
        {
                *(.note.gnu.build-id)
        }
}
