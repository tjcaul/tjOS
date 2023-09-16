#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>

#define FONT_FILE_PATH "src/font/"
#define C_FILE_PATH    "src/font/"
#define H_FILE_PATH    "include/font/"
#define FONT_FILE_SUFFIX ".font"
#define FONT_INCLUDE "font.h"
#define FONT_INCLUDE_DIR "font/"

enum {
	ARG_NAME = 1,
	ARG_WIDTH,
	ARG_HEIGHT,
	ARG_BOX_WIDTH,
	ARG_BOX_HEIGHT
};

void print_usage (char **argv);
int strtoi (const char *str);
FILE *fopen_err (const char *path, const char *mode);
void copy_line (FILE *c_file, FILE *font_file, int width, int end);

void print_usage (char **argv) {
	char *progname;
	if (argv)
		progname = argv[0];
	else
		progname = "[argv undefined]";
	fprintf(stderr, "usage: %s name width height [boxwidth boxheight]\n", progname);
}

int strtoi (const char *str) {
	char *endptr;
	long ret = strtol(str, &endptr, 0);
	if (endptr[0] != '\0') {
		fprintf(stderr, "Invalid number: '%s'\n", str);
		exit(EXIT_FAILURE);
	}
	if (ret > INT_MAX) {
		fprintf(stderr, "Number too large: %li\n", ret);
		exit(EXIT_FAILURE);
	}
	return ret;
}

FILE *fopen_err (const char *path, const char *mode) {
	FILE *ret = fopen(path, mode);
	if (!ret) {
		fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	return ret;
}

void copy_line (FILE *c_file, FILE *font_file, int width, int end) {
	int width_bytes = (width + 7) / 8;
	uint64_t line = 0;
	for (int i = 0; i < width; ++i) {
		char c;
		do
			c = fgetc(font_file);
		while (!(c == '.' || c == '#'));
		line <<= 1;
		if (c == '#')
			line |= 0x1;
	}
	line <<= 8 - (width % 8); //Align bits to left (010110010 becomes 590, not 0b4)
	for (int i = width_bytes - 1; i >= 0; --i) { //Big-endian output
		char byte = line >> (i * 8);
		fprintf(c_file, "0x%02hhx", byte);
		if (end && i == 0)
			fprintf(c_file, "  ");
		else
			fprintf(c_file, ", ");
	}
}

int main (int argc, char **argv) {
	char *base_name, *full_name, *font_file_name, *c_file_name, *h_file_name;
	int width, height, box_width, box_height;
	FILE *font_file, *c_file, *h_file;

	if (!(argc == 4 || argc == 6)) {
		print_usage(argv);
		exit(EXIT_FAILURE);
	}

	base_name = argv[ARG_NAME];

	char *endptr;
	width = strtoi(argv[ARG_WIDTH]);
	height = strtoi(argv[ARG_HEIGHT]);
	if (argc == 3) {
		box_width = width * 5 / 4;   //Reasonable default
		box_height = height * 5 / 4; //If you don't like it, specify a better one in argv!
	} else {
		box_width = strtoi(argv[ARG_BOX_WIDTH]);
		box_height = strtoi(argv[ARG_BOX_HEIGHT]);
	}

	int full_name_sz = strlen(base_name) + strlen(argv[ARG_WIDTH]) + strlen(argv[ARG_HEIGHT]) + 3;
	full_name = malloc(full_name_sz);
	font_file_name = malloc(strlen(FONT_FILE_PATH) + full_name_sz + strlen(FONT_FILE_SUFFIX));
	c_file_name = malloc(strlen(C_FILE_PATH) + full_name_sz + 3);
	h_file_name = malloc(strlen(H_FILE_PATH) + full_name_sz + 3);
	snprintf(full_name, full_name_sz, "%s_%sx%s", base_name, argv[ARG_WIDTH], argv[ARG_HEIGHT]);
	sprintf(font_file_name, "%s%s%s", FONT_FILE_PATH, full_name, FONT_FILE_SUFFIX);
	sprintf(c_file_name, "%s%s%s", C_FILE_PATH, full_name, ".c");
	sprintf(h_file_name, "%s%s%s", H_FILE_PATH, full_name, ".h");

	chdir(dirname(argv[0]));
	chdir("../../");

	font_file = fopen_err(font_file_name, "r");
	c_file = fopen_err(c_file_name, "w");
	h_file = fopen_err(h_file_name, "w");

	fprintf(h_file, "#ifndef FONT_%s\n", full_name);
	fprintf(h_file, "#define FONT_%s\n\n", full_name);
	fprintf(h_file, "#include \"%s\"\n\n", FONT_INCLUDE);
	fprintf(h_file, "extern struct font %s;\n\n", full_name);
	fprintf(h_file, "#endif\n");
	fclose(h_file);

	fprintf(c_file, "#include \"%s%s.h\"\n\n", FONT_INCLUDE_DIR, full_name);
	fprintf(c_file, "struct font %s = {\n", full_name);
	fprintf(c_file, "\t%d,\n", width);
	fprintf(c_file, "\t%d,\n", height);
	fprintf(c_file, "\t%d,\n", box_width);
	fprintf(c_file, "\t%d,\n", box_height);
	fprintf(c_file, "\t{\n");
	for (int c = 0; c < 256; ++c) {
		fprintf(c_file, "\t\t");
		for (int line = 0; line < height; ++line)
			copy_line(c_file, font_file, width, c == 255 && line == height - 1);
		char print_c = c;
		if (print_c < ' ' || print_c > '~')
			print_c = ' ';
		fprintf(c_file, "// %c\n", print_c);
	}
	fprintf(c_file, "\t}\n};\n");
	fclose(c_file);
	fclose(font_file);
	return 0;
}
