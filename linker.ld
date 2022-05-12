ENTRY("loader")
OUTPUT_FORMAT("binary")

SECTIONS
{
    .text 0x10000 :
    {
        *(kernel_entry); 
        *(.text*);
    }
    .rodata :
    {
        *(.rodata*);
    }
    .bss :
    {
        *(.bss*);
    }
}