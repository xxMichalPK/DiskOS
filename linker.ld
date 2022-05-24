OUTPUT_FORMAT("binary")

phys = 0xC0000000;

SECTIONS
{
   . = phys;

   .text ALIGN(4K) : {
      *(.krn_entry)
      *(.text)
   }
   
   .rodata ALIGN(4K) : {
      *(.rodata)
   }
   
   .data ALIGN(4K) : {
      *(.data)
   }
   
   .bss ALIGN(4K) : {
      *(.bss)
   }
   
   end = .;
}