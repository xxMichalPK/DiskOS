#pragma once

#include <stdint.h>
#include <math/math.h>
#include <memory/memory.h>
#include <kernelUtils.h>

#define abs(a) ((a > 0) ? a : -a)
#define ROUND(a) ((int)(a + 0.5))

// VBE Mode info block - holds current graphics mode values
typedef struct {
    // Mandatory info for all VBE revisions
	uint16_t mode_attributes;
	uint8_t window_a_attributes;
	uint8_t window_b_attributes;
	uint16_t window_granularity;
	uint16_t window_size;
	uint16_t window_a_segment;
	uint16_t window_b_segment;
	uint32_t window_function_pointer;
	uint16_t bytes_per_scanline;

    // Mandatory info for VBE 1.2 and above
	uint16_t x_resolution;
	uint16_t y_resolution;
	uint8_t x_charsize;
	uint8_t y_charsize;
	uint8_t number_of_planes;
	uint8_t bits_per_pixel;
	uint8_t number_of_banks;
	uint8_t memory_model;
	uint8_t bank_size;
	uint8_t number_of_image_pages;
	uint8_t reserved1;

    // Direct color fields (required for direct/6 and YUV/7 memory models)
	uint8_t red_mask_size;
	uint8_t red_field_position;
	uint8_t green_mask_size;
	uint8_t green_field_position;
	uint8_t blue_mask_size;
	uint8_t blue_field_position;
	uint8_t reserved_mask_size;
	uint8_t reserved_field_position;
	uint8_t direct_color_mode_info;

    // Mandatory info for VBE 2.0 and above
	uint32_t physical_base_pointer;         // Physical address for flat memory frame buffer
	uint32_t reserved2;
	uint16_t reserved3;

    // Mandatory info for VBE 3.0 and above
	uint16_t linear_bytes_per_scanline;
    uint8_t bank_number_of_image_pages;
    uint8_t linear_number_of_image_pages;
    uint8_t linear_red_mask_size;
    uint8_t linear_red_field_position;
    uint8_t linear_green_mask_size;
    uint8_t linear_green_field_position;
    uint8_t linear_blue_mask_size;
    uint8_t linear_blue_field_position;
    uint8_t linear_reserved_mask_size;
    uint8_t linear_reserved_field_position;
    uint32_t max_pixel_clock;

    uint8_t reserved4[190];              // Remainder of mode info block

} __attribute__ ((packed)) vbe_mode_info_t;

struct ASM_FONT {
	uint8_t font_width;
	uint8_t font_height;
	uint16_t* chars;
};

class VBE_C {
    public:
        vbe_mode_info_t *gfx_mode;
        ASM_FONT* sysFont;
        VBE_C() {
            gfx_mode = (vbe_mode_info_t*)0x5000;
            sysFont = (ASM_FONT*)0x1000;
        }
        void PlotPixel(uint32_t x, uint32_t y, uint32_t color) {
            uint8_t bpp = (gfx_mode->bits_per_pixel+1) / 8;
            *(uint32_t*)(gfx_mode->physical_base_pointer + (x*bpp) + (y * gfx_mode->x_resolution * bpp)) = color;
        }
		void LineDraw(Point start, Point end, uint32_t color) {
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

		void TriangleDraw(Point vertex0, Point vertex1, Point vertex2, uint32_t color) {
            LineDraw(vertex0, vertex1, color);
            LineDraw(vertex1, vertex2, color);
            LineDraw(vertex2, vertex0, color);
        }
        void TriangleFill(Point p0, Point p1, Point p2, uint32_t color) {
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


        void RectangleDraw(Point top_left, Point bottom_right, uint32_t color)
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
        void RectangleFill(Point top_left, Point bottom_right, uint32_t color)
        {
        // Brute force method 
        for (uint16_t y = top_left.Y; y < bottom_right.Y; y++)
            for (uint16_t x = top_left.X; x < bottom_right.X; x++)
                PlotPixel(x, y, color);
        }


        void CircleDraw(Point center, uint16_t radius, uint32_t color)
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
        void CircleFill(Point center, uint16_t radius, uint32_t color)
        {
            for (int16_t y = -radius; y <= radius; y++)
                for (int16_t x = -radius; x <= radius; x++)
                    if (x*x + y*y < radius*radius - radius)
                        PlotPixel(center.X + x, center.Y + y, color);
        }


        void EllipseDraw(Point center, uint16_t radiusX, uint16_t radiusY, uint32_t color)
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
        void EllipseFill(Point center, uint16_t radiusX, uint16_t radiusY, uint32_t color)
        {
            // First draw boundaries a different color
            EllipseDraw(center, radiusX, radiusY, color - 1);

            // Then fill in the boundaries
            FillBounds(center.X, center.Y, color, color - 1);

            // Then redraw boundaries as the correct color
            EllipseDraw(center, radiusX, radiusY, color);
        }


        void PolygonDraw(Point vertex_array[], uint8_t num_vertices, uint32_t color)
        {
            // Draw lines up to last line
            for (uint8_t i = 0; i < num_vertices - 1; i++)
                LineDraw(vertex_array[i], vertex_array[i+1], color);

            // Draw last line
            LineDraw(vertex_array[num_vertices - 1], vertex_array[0], color);
        }
        void PolygonFill(Point vertex_array[], uint8_t num_vertices, uint32_t color)
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
        void FillBounds(uint16_t X, uint16_t Y, uint32_t fill_color, uint32_t boundary_color) {
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


        void ColorsChange(uint32_t fg, uint32_t bg) {
            fgColor = fg;
            bgColor = bg;
        }


		void PutChar(char ch, uint32_t xOff, uint32_t yOff) {
            uint8_t bpp = (gfx_mode->bits_per_pixel+1) / 8;                         // Get # of bytes per pixel, add 1 to fix 15bpp modes
            uint8_t bytes_per_char_line = ((sysFont->font_width - 1) / 8) + 1;
            uint8_t char_size = bytes_per_char_line * sysFont->font_height;
            uint8_t* font_char = (uint8_t *)(0x1000 + ((ch * char_size) - char_size));
            yOff = (yOff * sysFont->font_height);
            xOff = (xOff * sysFont->font_width);

            if (ch == '\n') {
                Cursor.Y++;
                return;
            } 
            if (ch == '\r') {
                Cursor.X = 0;
                return;
            }
            if (ch == '\t') {
                Cursor.X += 4;
                return;
            }

            uint8_t *framebuffer = (uint8_t *)(gfx_mode->physical_base_pointer + ((yOff*gfx_mode->x_resolution + xOff) * bpp));
            for (uint32_t line = 0; line < sysFont->font_height; line++) { 
                uint32_t num_px_drawn = 0;
                for (int8_t byte = bytes_per_char_line - 1; byte >= 0; byte--) {
                    for (int8_t bit = 7; bit >= 0; bit--) {
                        // If bit is set draw text color pixel, if not draw background color
                        if (font_char[line * bytes_per_char_line + byte] & (1 << bit)) {
                            *((uint32_t *)framebuffer) = fgColor;
                        } else {
                            *((uint32_t *)framebuffer) = bgColor;
                        }
                        framebuffer += bpp;  // Next pixel position
                        num_px_drawn++;
                        // If done drawing all pixels in current line of char, stop and go on
                        if (num_px_drawn == sysFont->font_width) {
                            num_px_drawn = 0;
                            break;
                        }
                    }
                }
                framebuffer += (gfx_mode->x_resolution - sysFont->font_width) * bpp;
            }
            Cursor.X++;
        }
		void PutChar(char ch) {
            PutChar(ch, Cursor.X, Cursor.Y);
        }
		void PrintString(const char* str, uint32_t x, uint32_t y) {
            Cursor.X = x;
            Cursor.Y = y;
            while (*str != '\0') {
                PutChar(*str);
                x++;
                UNUSED(*str++);
            }
        }
		void PrintString(const char* str) {
            PrintString(str, Cursor.X, Cursor.Y);
        }
		
		void ClearScreen(uint32_t color) {
            memset((void*)(gfx_mode->physical_base_pointer), color, gfx_mode->x_resolution * gfx_mode->y_resolution * ((gfx_mode->bits_per_pixel+1) / 8));
        }
};

VBE_C VBE;
void VBE_Init() {
    Cursor = { 0, 0 };
    fgColor = 0xFFFFFFFF;
    bgColor = 0xFF000000;
    VBE = VBE_C();
}