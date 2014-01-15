#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage(const char *prog)
{
    fprintf(stderr, "Usage: %s -w WIDTH -h HEIGHT [-s SIZE]\n", prog);
    fprintf(stderr, "      -w  WIDTH  Image width\n");
    fprintf(stderr, "      -h  HEIGHT Image height\n");
    fprintf(stderr, "      -s  SIZE   Size of the checkers (default 100)\n");
    exit(1);
}

FILE *output;
short height = 0;
short width = 0;
int size = 100;

void parse_arguments(int argc, char *argv[])
{
	int i = 1;

	while (i < argc) {
		if (strcmp(argv[i], "-w") == 0)
			width = atoi(argv[++i]);
		else if (strcmp(argv[i], "-h") == 0)
			height = atoi(argv[++i]);
		else if (strcmp(argv[i], "-s") == 0)
			size = atoi(argv[++i]);
		i++;
	}
	output = stdout;
}

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} pixel;

pixel color[] = {
	{255,   0,   0, 255},
	{  0, 255,   0, 255},
	{  0,   0, 255, 255},
};

int main(int argc, char *argv[])
{
	parse_arguments(argc, argv);

	if (width <= 0 || height <= 0 || size <= 1)
		usage(argv[0]);

	fwrite(&width, sizeof(width), 1, output);
	fwrite(&height, sizeof(height), 1, output);

	pixel *line = malloc(sizeof(pixel) * width);

	int row, col, k;
	for (row = 0; row < height; row++) {
		for (col = 0; col < width; col++) {
			k = (row/size + col/size) % 3;
			line[col] = color[k];
		}
		fwrite(line, sizeof(pixel), width, output);
	}

	free(line);

	return 0;
}
