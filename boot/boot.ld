ENTRY(boot)
OUTPUT_FORMAT("binary")
SECTIONS 
{
    . = 0x7C00;
    .text :
    {
        *(.text)
    }
    .sig : AT(0x7DFE)
    {
        SHORT(0xAA55);
    }
}