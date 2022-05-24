CC_FLAGS = -m32 -Iinclude -ffreestanding -fno-PIC -Wall -O2

BOOT_FILES = opt/bootloader/bootSect.bin

2ND_STAGE_FILES = opt/bootloader/stage2.bin \
				opt/bootloader/termu16n.bin

3RD_STAGE_FILES = opt/stage3.o

KERNEL_FILES = 	opt/kernel.o
				
LIB_FILES = opt/kernelUtils.o \
			opt/interrupts/idt.o \
			opt/interrupts/exceptions.o \
			opt/disk/ata.o \
			opt/fs/fat32.o \
			opt/memory/pmm.o \
			opt/memory/vmm.o \
			opt/memory/malloc.o \
			opt/graphics/vbe.o

opt/%.bin: src/%.asm
	@mkdir -p $(@D)
	@echo [Compiling $@]
	@nasm -f bin $< -o $@

opt/%.o: src/%.asm
	@mkdir -p $(@D)
	@echo [Compiling $@]
	@nasm -felf32 $< -o $@

opt/interrupts/%.o: src/interrupts/%.cpp
	@mkdir -p $(@D)
	@echo [Compiling $@]
	@gcc $(CC_FLAGS) -mgeneral-regs-only -c $^ -o $@

opt/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo [Compiling $@]
	@gcc $(CC_FLAGS) -c $^ -o $@

run: link
	qemu-system-i386 DiskOS.img

link: kernel.bin stage3.bin $(BOOT_FILES) $(2ND_STAGE_FILES)
	@cp $(BOOT_FILES) ./tmpboot.bin
	@echo [Creating disk image...]
	@dd if=/dev/zero of=DiskOS.img bs=512 count=93750
	@echo [Formatting as FAT32...]
	@mkdosfs -F 32 DiskOS.img
	@echo [Copying bootloader to disk image...]
	@dd if=tmpboot.bin of=DiskOS.img bs=1 count=3 conv=notrunc
	@dd if=tmpboot.bin of=names.bin bs=1 skip=71 count=19
	@dd if=tmpboot.bin of=boot.bin bs=512 skip=90 iflag=skip_bytes
	@dd if=names.bin of=DiskOS.img bs=512 seek=71 oflag=seek_bytes conv=notrunc
	@dd if=boot.bin of=DiskOS.img bs=512 seek=90 oflag=seek_bytes conv=notrunc
	@cp opt/bootloader/stage2.bin ./stage2.bin
	@cp opt/bootloader/termu16n.bin ./termu16n.bin
	@mcopy -i DiskOS.img stage2.bin ::
	@mcopy -i DiskOS.img termu16n.bin ::
	@mcopy -i DiskOS.img stage3.bin ::
	@mcopy -i DiskOS.img kernel.bin ::

kernel.bin: $(KERNEL_FILES) libdisk.a
	ld -melf_i386 -Tlinker.ld $^ -o $@
	@size=$$(($$(wc -c < kernel.bin)));\
	echo "%define KERNEL_SECTORS" "$$(printf '0x%02X' $$((size / 512)))" > src/bootloader/sizes.inc;

stage3.bin: $(3RD_STAGE_FILES) libdisk.a
	ld -melf_i386 -Tstage3.ld $^ -o $@
	@size=$$(($$(wc -c < stage3.bin)));\
	echo "%define STAGE3_SECTORS" "$$(printf '0x%02X' $$((size / 512)))" >> src/bootloader/sizes.inc;

libdisk.a: $(LIB_FILES)
	ar rcs $@ $^

clean:
	rm -rf *.bin *.o *.a *.img opt/
