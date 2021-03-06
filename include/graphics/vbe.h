#pragma once

#include <stdint.h>
#include <math/math.h>
#include <vars/addresses.h>

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

class VBE {
    public:
        VBE();
		vbe_mode_info_t *gfx_mode;
        void PlotPixel(uint32_t x, uint32_t y, uint32_t color);
		void LineDraw(Point start, Point end, uint32_t color);

		void TriangleDraw(Point vertex0, Point vertex1, Point vertex2, uint32_t color);
		void TriangleFill(Point p0, Point p1, Point p2, uint32_t color);

		void RectangleDraw(Point top_left, Point bottom_right, uint32_t color);
		void RectangleFill(Point top_left, Point bottom_right, uint32_t color);

		void CircleDraw(Point center, uint16_t radius, uint32_t color);
		void CircleFill(Point center, uint16_t radius, uint32_t color);

		void EllipseDraw(Point center, uint16_t radiusX, uint16_t radiusY, uint32_t color);
		void EllipseFill(Point center, uint16_t radiusX, uint16_t radiusY, uint32_t color);

		void PolygonDraw(Point vertex_array[], uint8_t num_vertices, uint32_t color);
		void PolygonFill(Point vertex_array[], uint8_t num_vertices, uint32_t color);

		void FillBounds(uint16_t X, uint16_t Y, uint32_t fill_color, uint32_t boundary_color);

		void ColorsChange(uint32_t fg, uint32_t bg);

		ASM_FONT* sysFont;
		void PutChar(char ch, uint32_t xOff, uint32_t yOff);
		void PutChar(char ch);
		void PrintString(const char* str, uint32_t x, uint32_t y);
		void PrintString(const char* str);
		
		void ClearScreen(uint32_t color);
};

extern VBE* vbe;