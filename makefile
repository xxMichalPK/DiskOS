CC_FLAGS = -m32 -Iinclude -ffreestanding -fno-PIC -Wall -O2

BOOT_FILES = opt/bootloader/bootSect.bin \
			opt/bootloader/termu16n.bin

KERNEL_FILES = opt/loader.o \
				opt/kernel.o \
				opt/kernelUtils.o \
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

opt/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo [Compiling $@]
	@gcc $(CC_FLAGS) -c $^ -o $@

run: link
	qemu-system-i386 OS.bin

link: kernel.bin $(BOOT_FILES)
	@cat $(BOOT_FILES) kernel.bin > tmpboot.bin
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

kernel.bin: $(KERNEL_FILES)
	ld -melf_i386 -Tlinker.ld $^ -o $@
	@size=$$(($$(wc -c < kernel.bin)));\
	echo "%define KERNEL_SECTORS" "$$(printf '0x%02X' $$((size / 512)))" > src/bootloader/sizes.inc;

clean:
	rm -rf *.bin *.o *.img opt/
