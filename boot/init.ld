ENTRY(init)
OUTPUT_FORMAT("binary")
SECTIONS 
{
    . = 0x1000;
    .text :
    {
        *(.text)
        FILL(0);
        . = 0x800;
    }
}