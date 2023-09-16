#ifndef DISPLAY_H
#define DISPLAY_H

#include <stddef.h>
#include "efi.h"
#include "efilib.h"
#include "font.h"

#define COLOUR_TRANSPARENT 0xff000000

typedef uint32_t colour_t;

void init_gop (int mode);
void set_pixel (int x, int y, colour_t colour);
void draw_rect (int x, int y, int width, int height, colour_t colour);
void set_char (int x, int y, struct font *font, int scale, colour_t fg, colour_t bg, char c);
void set_char_pix (int pix_x, int pix_y, struct font *font, int scale, colour_t fg, colour_t bg, char c);
colour_t rgb (unsigned char r, unsigned char g, unsigned char b);
void test_gradient (void);
void test_scale (struct font *font, colour_t fg, colour_t bg, char c);
void test_ascii (int x, int y, struct font *font, int scale, colour_t fg, colour_t bg);

#endif
