#include "display.h"

EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

static int font_get_bit (const struct font *font, char c, int row, int col);

static int font_get_bit (const struct font *font, char c, int row, int col) {
	const int width = (font->width + 7) / 8; //+7 to ensure rounding up (i.e. width = 1 for 8b font, 2 for 9b font)
	const int addr = (c * font->height + row) * width + col / 8;
	const char byte = font->data[addr];
	const int bit_index = 7 - col % 8;
	return (byte >> bit_index) & 0x1;

}

void init_gop (int mode) {
	EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_STATUS status;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
	UINTN info_sz, num_modes, native_mode;
	
	BS->LocateProtocol(&gop_guid, NULL, (void**)&gop);

	const uint32_t query_mode = (gop->Mode == NULL) ? mode : gop->Mode->Mode;
	status = gop->QueryMode(gop, query_mode, &info_sz, &info);
	if (mode != -1)
		gop->SetMode(gop, mode);
}

void set_pixel (int x, int y, colour_t colour) {
	if (x < 0 || x > gop->Mode->Info->HorizontalResolution || y < 0 || y > gop->Mode->Info->VerticalResolution)
		return;
	const unsigned int offset = gop->Mode->Info->PixelsPerScanLine * y + x;
	((colour_t*)gop->Mode->FrameBufferBase)[offset] = colour;
}

void draw_rect (int x, int y, int width, int height, colour_t colour) {
	for (int xi = 0; xi < width; ++xi)
		for (int yi = 0; yi < height; ++yi)
			set_pixel(x + xi, y + yi, colour);
}

void set_char (int x, int y, struct font *font, int scale, colour_t fg, colour_t bg, char c) {
	const int pix_x = x * font->box_width * scale;
	const int pix_y = y * font->box_height * scale;
	set_char_pix(pix_x, pix_y, font, scale, fg, bg, c);
}

void set_char_pix (int pix_x, int pix_y, struct font *font, int scale, colour_t fg, colour_t bg, char c) {
	for (int yi = 0; yi < font->box_height; ++yi)
		for (int xi = 0; xi < font->box_width; ++xi) {
			colour_t colour;
			if (xi >= font->width || yi >= font->height) {
				colour = bg;
			} else {
				const int bit = font_get_bit(font, c, yi, xi);
				colour = bit ? fg : bg;
			}
			if (colour != COLOUR_TRANSPARENT) {
				if (scale == 1) {
					set_pixel(pix_x + xi, pix_y + yi, colour);
				} else {
					draw_rect(pix_x + xi * scale, pix_y + yi * scale, scale, scale, colour);
				}
			}
		}
}

colour_t rgb (unsigned char r, unsigned char g, unsigned char b) {
	return (r << 16) + (g << 8) + b;
}

void test_gradient (void) {
	for (int x = 0; x < 256; ++x)
		for (int y = 0; y < 256; ++y)
			set_pixel(x, y, rgb(x, 255, y));
}

void test_scale (struct font *font, colour_t fg, colour_t bg, char c) {
	for (int i = 1; i <= 16; ++i)
		set_char_pix (i * (i - 1) * font->box_width / 2, 0, font, i, fg, bg, c);
}

void test_ascii (int x, int y, struct font *font, int scale, colour_t fg, colour_t bg) {
	for (char i = 0; i < 95; ++i)
		set_char (x + i, y, font, scale, fg, bg, i + ' ');
}
