#include <graphics/vbe.h>

void main() {
	vbe_mode_info_t* vbe = (vbe_mode_info_t*)0x5000;
	unsigned int* ptr = (unsigned int*)vbe->physical_base_pointer;
	ptr[0] = 0xFFFFFFFF;
	ptr[1] = 0xFFFFFF00;
	ptr[2] = 0xFFFF0000;
	ptr[3] = 0xFF00FF00;
	ptr[4] = 0xFF0000FF;
	for (;;);
}