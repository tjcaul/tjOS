#ifndef FONT_H
#define FONT_H

#include <stddef.h>

struct font {
	int width;
	int height;
	int box_width;
	int box_height;
	char data[];
};

#endif
