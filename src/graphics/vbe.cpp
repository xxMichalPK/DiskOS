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


void VBE::RectangleDraw(Point top_left, Point bottom_right, uint32_t color)
{
    Point temp = bottom_right;
    // Draw 4 lines, 2 horizontal parallel sides, 2 vertical parallel sides
    // Top of rectangle
    temp.Y = top_left.Y;
    LineDraw(top_left, temp, color);
    // Right side
    LineDraw(temp, bottom_right, color);
    // Bottom
    temp.X = top_left.X;
    temp.Y = bottom_right.Y;
    LineDraw(bottom_right, temp, color);
    // Left side
    LineDraw(temp, top_left, color);
}
void VBE::RectangleFill(Point top_left, Point bottom_right, uint32_t color)
{
   // Brute force method 
   for (uint16_t y = top_left.Y; y < bottom_right.Y; y++)
       for (uint16_t x = top_left.X; x < bottom_right.X; x++)
           PlotPixel(x, y, color);
}


void VBE::CircleDraw(Point center, uint16_t radius, uint32_t color)
{
    int16_t x = 0;
    int16_t y = radius;
    int16_t p = 1 - radius;
    // Draw initial 8 octant points
    PlotPixel(center.X + x, center.Y + y, color);
    PlotPixel(center.X - x, center.Y + y, color);
    PlotPixel(center.X + x, center.Y - y, color);
    PlotPixel(center.X - x, center.Y - y, color);
    PlotPixel(center.X + y, center.Y + x, color);
    PlotPixel(center.X - y, center.Y + x, color);
    PlotPixel(center.X + y, center.Y - x, color);
    PlotPixel(center.X - y, center.Y - x, color);

    while (x < y) {
        x++;
        if (p < 0) p += 2*x + 1;
        else {
            y--;
            p += 2*(x-y) + 1;
        }
        // Draw next set of 8 octant points
        PlotPixel(center.X + x, center.Y + y, color);
        PlotPixel(center.X - x, center.Y + y, color);
        PlotPixel(center.X + x, center.Y - y, color);
        PlotPixel(center.X - x, center.Y - y, color);
        PlotPixel(center.X + y, center.Y + x, color);
        PlotPixel(center.X - y, center.Y + x, color);
        PlotPixel(center.X + y, center.Y - x, color);
        PlotPixel(center.X - y, center.Y - x, color);
    }
}
void VBE::CircleFill(Point center, uint16_t radius, uint32_t color)
{
    for (int16_t y = -radius; y <= radius; y++)
        for (int16_t x = -radius; x <= radius; x++)
            if (x*x + y*y < radius*radius - radius)
                PlotPixel(center.X + x, center.Y + y, color);
}


void VBE::EllipseDraw(Point center, uint16_t radiusX, uint16_t radiusY, uint32_t color)
{
    int32_t rx2 = radiusX * radiusX;
    int32_t ry2 = radiusY * radiusY;
    int32_t twoRx2 = 2*rx2;
    int32_t twoRy2 = 2*ry2;
    int32_t p;
    int32_t x = 0;
    int32_t y = radiusY;
    int32_t px = 0;
    int32_t py = twoRx2 * y;

    // Draw initial 4 quadrant points
    PlotPixel(center.X + x, center.Y + y, color);
    PlotPixel(center.X - x, center.Y + y, color);
    PlotPixel(center.X + x, center.Y - y, color);
    PlotPixel(center.X - x, center.Y - y, color);

    // Region 1
    p = ROUND(ry2 - (rx2 * radiusY) + (0.25 * rx2));
    while (px < py) {
        x++;
        px += twoRy2;
        if (p < 0) p += ry2 + px;
        else {
            y--;
            py -= twoRx2;
            p += ry2 + px - py;
        }

        // Draw 4 quadrant points
        PlotPixel(center.X + x, center.Y + y, color);
        PlotPixel(center.X - x, center.Y + y, color);
        PlotPixel(center.X + x, center.Y - y, color);
        PlotPixel(center.X - x, center.Y - y, color);
    }

    // Region 2
    p = ROUND(ry2 * (x + 0.5)*(x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2*ry2);
    while (y > 0) {
        y--;
        py -= twoRx2; if (p > 0) p += rx2 - py;
        else {
            x++;
            px += twoRy2;
            p += rx2 - py + px;
        }

        // Draw 4 quadrant points
        PlotPixel(center.X + x, center.Y + y, color);
        PlotPixel(center.X - x, center.Y + y, color);
        PlotPixel(center.X + x, center.Y - y, color);
        PlotPixel(center.X - x, center.Y - y, color);
    }
}
void VBE::EllipseFill(Point center, uint16_t radiusX, uint16_t radiusY, uint32_t color)
{
    // First draw boundaries a different color
    EllipseDraw(center, radiusX, radiusY, color - 1);

    // Then fill in the boundaries
    FillBounds(center.X, center.Y, color, color - 1);

    // Then redraw boundaries as the correct color
    EllipseDraw(center, radiusX, radiusY, color);
}


void VBE::PolygonDraw(Point vertex_array[], uint8_t num_vertices, uint32_t color)
{
    // Draw lines up to last line
    for (uint8_t i = 0; i < num_vertices - 1; i++)
        LineDraw(vertex_array[i], vertex_array[i+1], color);

    // Draw last line
    LineDraw(vertex_array[num_vertices - 1], vertex_array[0], color);
}
void VBE::PolygonFill(Point vertex_array[], uint8_t num_vertices, uint32_t color)
{
    Point temp;

    // Assuming this works in general, get center (Centroid) of a triangle in the polygon
    temp.X = (vertex_array[0].X + vertex_array[1].X + vertex_array[2].X) / 3;
    temp.Y = (vertex_array[0].Y + vertex_array[1].Y + vertex_array[2].Y) / 3;

    // First draw polygon sides (boundaries)
    PolygonDraw(vertex_array, num_vertices, color - 1);

    // Then fill in boundaries 
    FillBounds(temp.X, temp.Y, color, color - 1);

    // Then redraw boundaries to correct color
    PolygonDraw(vertex_array, num_vertices, color);
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