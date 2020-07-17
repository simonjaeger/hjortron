ENTRY(boot16)
OUTPUT_FORMAT("binary")
SECTIONS 
{
    . = 0x7C00;
    .boot16 :
    {
        *(.boot16)
    }

    .sig : AT(0x7DFE)
    {
        SHORT(0xAA55);
    }

    . = 0x7E00;
    .boot32 : AT(0x7E00)
    {
        *(.boot32)
    }

    . = 0x10000;
    .kernel : AT(MAX(0x8000, 0x7C00 + SECTOR_SIZE))
    {
        .start = . ;
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.data)
        *(.data.*)
        *(.bss)
        .end = . ;
    }

    sector_size = SECTOR_SIZE;
    kernel_size = (SIZEOF(.kernel) + SECTOR_SIZE - 1) / SECTOR_SIZE;
}