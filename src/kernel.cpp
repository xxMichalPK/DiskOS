#include <graphics/vbe.h>

extern "C" __attribute__ ((section("kernel_entry"))) void kMain(void) {
    VBE vbe;
    vbe.ClearScreen(0xFF121212);
    vbe.LineDraw(Point {10, 10}, Point {60, 30}, 0xFF2288CC);
    vbe.TriangleFill(Point {50, 50}, Point {100, 50}, Point {75, 90}, 0xFF22CC77);
    for (;;);
}