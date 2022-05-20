OUTPUT_FORMAT("binary")

phys = 0xC0000000;

SECTIONS
{
   . = phys;

   .text BLOCK(4K) : ALIGN(4K) {
      *(.krn_entry)
      *(.text)
   }
   
   .rodata BLOCK(4K) : ALIGN(4K) {
      *(.rodata)
   }
   
   .data BLOCK(4K) : ALIGN(4K) {
      *(.data)
   }
   
   .bss BLOCK(4K) : ALIGN(4K) {
      *(.bss)
   }
   
   end = .;
}