#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage(const char *prog)
{
    fprintf(stderr, "Usage: %s INPUT PPM_OUTPUT\n", prog);
    fprintf(stderr, "Converts the custom image format to PPM\n");
    exit(1);
}

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} pixel;

short height = 0;
short width = 0;

int main(int argc, char *argv[])
{
	FILE *input = fopen(argv[1], "r");
	FILE *output = fopen(argv[2], "w");

	fread(&width, sizeof(width), 1, input);
	fread(&height, sizeof(height), 1, input);
	fprintf(output, "P6\n%d %d\n%d\n", width, height, 255);

	pixel *line = malloc(sizeof(pixel) * width);

	int row, col;
	for (row = 0; row < height; row++) {
		fread(line, sizeof(pixel), width, input);
		for (col = 0; col < width; col++)
			fwrite(&line[col], sizeof(unsigned char), 3, output);
	}

	free(line);

	return 0;
}
