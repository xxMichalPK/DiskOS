#include <kernelUtils.h>

Point Cursor;
uint32_t fgColor;
uint32_t bgColor;
VBE tmpVBE = VBE();
void initKernel() {
    tmpVBE = VBE();
    vbe = &tmpVBE;
    Cursor = { 0, 0 };
    fgColor = 0xFFFFFFFF;
    bgColor = 0xFF000000;
    vbe->ClearScreen(bgColor);
}