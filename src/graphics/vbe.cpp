#include <graphics/vbe.h>
#include <memory/memory.h>

VBE::VBE() {
    vbe_mode_info_t* tmpModeInfo = (vbe_mode_info_t*)VBE_MODE_INFO_ADDRESS;
    gfx_mode = tmpModeInfo;
}

void VBE::PlotPixel(uint32_t x, uint32_t y, uint32_t color) {
    uint8_t bpp = (gfx_mode->bits_per_pixel+1) / 8;
    *(uint32_t*)(gfx_mode->physical_base_pointer + (x*bpp) + (y * gfx_mode->x_resolution * bpp)) = color;
}

void VBE::LineDraw(Point start, Point end, uint32_t color) {
    int16_t deltaX = abs((end.X - start.X));    // Delta X, change in X values, positive absolute value
    int16_t deltaY = -abs((end.Y - start.Y));   // Delta Y, change in Y values, negative absolute value
    int16_t signX = (start.X < end.X) ? 1 : -1; // Sign of X direction, moving right (positive) or left (negative)
    int16_t signY = (start.Y < end.Y) ? 1 : -1; // Sign of Y direction, moving down (positive) or up (negative)
    int16_t error = deltaX + deltaY;
    int16_t errorX2;

    while (1) {
        PlotPixel(start.X, start.Y, color);
        if (start.X == end.X && start.Y == end.Y) break;

        errorX2 = error * 2;
        if (errorX2 >= deltaY) {
            error   += deltaY;
            start.X += signX;
        }
        if (errorX2 <= deltaX) {
            error   += deltaX;
            start.Y += signY;
        }
    }
}

void VBE::TriangleDraw(Point vertex0, Point vertex1, Point vertex2, uint32_t color) {
    LineDraw(vertex0, vertex1, color);
    LineDraw(vertex1, vertex2, color);
    LineDraw(vertex2, vertex0, color);
}
void VBE::TriangleFill(Point p0, Point p1, Point p2, uint32_t color) {
    Point temp;

    // Get center (Centroid) of a triangle
    temp.X = (p0.X + p1.X + p2.X) / 3;
    temp.Y = (p0.Y + p1.Y + p2.Y) / 3;

    // First draw triangles sides (boundaries)
    TriangleDraw(p0, p1, p2, color - 1);

    // Then fill in boundaries 
    FillBounds(temp.X, temp.Y, color, color - 1);

    // Then redraw boundaries to correct color
    TriangleDraw(p0, p1, p2, color);
}

// Fill an area with a solid color
void VBE::FillBounds(uint16_t X, uint16_t Y, uint32_t fill_color, uint32_t boundary_color) {
    // Recursive - may use a lot of stack space
    uint8_t *framebuffer    = (uint8_t *)gfx_mode->physical_base_pointer; 
    uint8_t bytes_per_pixel = (gfx_mode->bits_per_pixel+1) / 8;             // Get # of bytes per pixel, add 1 to fix 15bpp modes
    uint8_t draw = 0;

    framebuffer += (Y * gfx_mode->x_resolution + X) * bytes_per_pixel;

    for (uint8_t temp = 0; temp < bytes_per_pixel; temp++) {
        if ((framebuffer[temp] != (uint8_t)(fill_color >> (temp * 8))) &&
            (framebuffer[temp] != (uint8_t)(boundary_color >> (temp * 8)))) {

            draw = 1;
            break;
        }
    }

    if (draw) {
        for (uint8_t temp = 0; temp < bytes_per_pixel; temp++)
            framebuffer[temp] = (uint8_t)(fill_color >> temp * 8);

        // Check 4 pixels around current pixel
        FillBounds(X + 1, Y, fill_color, boundary_color);
        FillBounds(X - 1, Y, fill_color, boundary_color);
        FillBounds(X, Y + 1, fill_color, boundary_color);
        FillBounds(X, Y - 1, fill_color, boundary_color);
    }
} 

void VBE::ClearScreen(uint32_t color) {
    memset((void*)(gfx_mode->physical_base_pointer), color, gfx_mode->x_resolution * gfx_mode->y_resolution * ((gfx_mode->bits_per_pixel+1) / 8));
}