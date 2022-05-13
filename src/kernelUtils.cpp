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
    bgColor = 0xFF121212;

    vbe->ClearScreen(bgColor);

    /* For testing graphics 
    vbe->LineDraw(Point {10, 10}, Point {190, 30}, 0xFF2288CC);
    vbe->TriangleFill(Point {50, 50}, Point {190, 50}, Point {99, 178}, 0xFF22CC77);
    vbe->RectangleFill(Point {290, 90}, Point {400, 440}, 0xFFDD2288);
    vbe->CircleDraw(Point {600, 390}, 100, 0xFFFFFFFF);
    vbe->CircleFill(Point {600, 390}, 98, 0xFF000000);
    vbe->CircleFill(Point {600, 390}, 36, 0xFFFFFFFF);
    Point poly_array[6] = { 520, 70, 560, 60, 570, 90, 550, 130, 540, 190, 510, 80};
    vbe->PolygonFill(poly_array, 6, 0xFFFF2222);
    */
}