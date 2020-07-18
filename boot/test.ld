ENTRY(text)
OUTPUT_FORMAT("binary")
SECTIONS 
{
    .text :
    {
        *(.text)
    }
}