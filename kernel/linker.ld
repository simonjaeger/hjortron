ENTRY(main)
OUTPUT_FORMAT("binary")
SECTIONS 
{
    . = 0x10000;
    KERNEL_START = .;
    .kernel :
    {
        *(.text.main);
        *(.text);
        *(.text.*);
        *(.rodata);
        *(.rodata.*);
        *(.data);
        *(.data.*);
        *(.bss);
    }
    KERNEL_END = .;
}