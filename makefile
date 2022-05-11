ASM_FILES = opt/bootloader/bootSect.bin

opt/%.bin: src/%.asm
	@mkdir -p $(@D)
	@echo [Compiling $@]
	@nasm -f bin $< -o $@

run: link
	qemu-system-i386 OS.bin

link: $(ASM_FILES)
	cat $^ > OS.bin

clean:
	rm -rf *.bin opt/