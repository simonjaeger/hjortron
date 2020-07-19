ENTRY(test)
OUTPUT_FORMAT("binary")
SECTIONS 
{
    . = 0x10000;
    .text :
    {
        *(.text)
    }
}