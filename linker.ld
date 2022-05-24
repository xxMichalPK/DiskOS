OUTPUT_FORMAT("binary")

phys = 0x00100000;
virt = 0xC0000000;

SECTIONS
{
   . = phys + virt;

   .text ALIGN(0x1000) : AT(ADDR(.text)-virt) {
      *(.krn_entry)
      *(.text)
   }
   
   .data ALIGN(0x1000) : AT(ADDR(.data)-virt) {
      start_ctors = .;
      KEEP(*( .init_array ));
      KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
      end_ctors = .;

      *(.data)
   }
   
   .rodata ALIGN(0x1000) : AT(ADDR(.rodata)-virt) {
      *(.rodata)
   }

   .bss ALIGN(0x1000) : AT(ADDR(.bss)-virt) {
      *(.bss)
   }

   /DISCARD/ : { *(.fini_array*) *(.comment) }
}