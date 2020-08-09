ENTRY(main)
OUTPUT_FORMAT("elf32-i386")
SECTIONS 
{
    .text :
    {
        *(.text)
        *(.text.*)
    }
    .rodata : 
    {
        *(.rodata)
        *(.rodata.*)
    }
    .data : 
    {
        *(.data)
        *(.data.*)
    }
    .bss : 
    {
        *(.bss)
    }
}