ENTRY(boot)
OUTPUT_FORMAT("binary")
SECTIONS 
{
    . = 0x7C00;
    .boot :
    {
        *(.boot)
    }
    .signature : AT(0x7DFE)
    {
        SHORT(0xAA55);
    }
}