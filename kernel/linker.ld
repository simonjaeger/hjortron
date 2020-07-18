ENTRY(main)
OUTPUT_FORMAT("binary")
SECTIONS 
{
    . = 0x10000;
    .kernel :
    {
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.data)
        *(.data.*)
        *(.bss)
    }
}