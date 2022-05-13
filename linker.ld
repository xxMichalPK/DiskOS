ENTRY("loader")
OUTPUT_FORMAT("binary")

phys = 0x00003000;

SECTIONS
{
   . = phys;

   .text BLOCK(4K) : ALIGN(4K) {
      *(.kernel_entry)
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