ENTRY(test)
OUTPUT_FORMAT("binary")
SECTIONS 
{
    . = 0x3000;
    .text :
    {
        *(.text)
    }
}