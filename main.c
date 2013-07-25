/*
 * Copyright (C) 2013 Alber Tabone Novo <alber.tabone@gmail.com>
 * Copyright (C) 2013 Ian Liu Rodrigues <ian.liu88@gmail.com>
 * Copyright (C) 2013 João Renato Domingos do Sacramento <joaorenatods@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* Largura máxima da imagem, dada no problema */
#define MAX_W 7680

/* Altura máxima da imagem, dada no problema */
#define MAX_H 4320

/* Tamanho da borda de zeros na imagem para um smooth de janela 5x5 */
#define BORDER 2

/* Tamanho máximo de uma imagem */
#define MAX_SIZE ((MAX_W+2*BORDER)*(MAX_H+2*BORDER)*4)

/* time_it(func):
 * Esta macro executa a função `func' e imprime o tempo tomado por ela
 * caso NDEBUG não esteja definido.
 */
#ifdef NDEBUG
# define time_it(func) func()
#else
# define time_it(func) do { \
	uint64_t t0 = tick(); \
	func(); \
	printf(#func ": %ld\n", tick()-t0); \
} while(0)
#endif

#define MIN(a, b) ((a)<(b)?(a):(b))

static uint16_t width, height;
static uint8_t img[MAX_SIZE] = {0,};
static uint8_t out[MAX_SIZE] = {0,};

struct slice {
	int x0, x1;
	int y0, y1;
};

void smooth5_slice(struct slice *slice)
{
	int w = width + 2*BORDER;
	int i, j, x0, x1, y0, y1;
	int r, g, b, a;
	x0 = BORDER + slice->x0;
	x1 = BORDER + slice->x1;
	y0 = BORDER + slice->y0;
	y1 = BORDER + slice->y1;
	for (i = y0; i < y1; i++) {
		for (j = x0; j < x1; j++) {
			r = g = b = a = 0;

			r += img[((i-2)*w + (j-2))*4+0], g += img[((i-2)*w + (j-2))*4+1], b += img[((i-2)*w + (j-2))*4+2], a += img[((i-2)*w + (j-2))*4+3];
			r += img[((i-2)*w + (j-1))*4+0], g += img[((i-2)*w + (j-1))*4+1], b += img[((i-2)*w + (j-1))*4+2], a += img[((i-2)*w + (j-1))*4+3];
			r += img[((i-2)*w + (j+0))*4+0], g += img[((i-2)*w + (j+0))*4+1], b += img[((i-2)*w + (j+0))*4+2], a += img[((i-2)*w + (j+0))*4+3];
			r += img[((i-2)*w + (j+1))*4+0], g += img[((i-2)*w + (j+1))*4+1], b += img[((i-2)*w + (j+1))*4+2], a += img[((i-2)*w + (j+1))*4+3];
			r += img[((i-2)*w + (j+2))*4+0], g += img[((i-2)*w + (j+2))*4+1], b += img[((i-2)*w + (j+2))*4+2], a += img[((i-2)*w + (j+2))*4+3];

			r += img[((i-1)*w + (j-2))*4+0], g += img[((i-1)*w + (j-2))*4+1], b += img[((i-1)*w + (j-2))*4+2], a += img[((i-1)*w + (j-2))*4+3];
			r += img[((i-1)*w + (j-1))*4+0], g += img[((i-1)*w + (j-1))*4+1], b += img[((i-1)*w + (j-1))*4+2], a += img[((i-1)*w + (j-1))*4+3];
			r += img[((i-1)*w + (j+0))*4+0], g += img[((i-1)*w + (j+0))*4+1], b += img[((i-1)*w + (j+0))*4+2], a += img[((i-1)*w + (j+0))*4+3];
			r += img[((i-1)*w + (j+1))*4+0], g += img[((i-1)*w + (j+1))*4+1], b += img[((i-1)*w + (j+1))*4+2], a += img[((i-1)*w + (j+1))*4+3];
			r += img[((i-1)*w + (j+2))*4+0], g += img[((i-1)*w + (j+2))*4+1], b += img[((i-1)*w + (j+2))*4+2], a += img[((i-1)*w + (j+2))*4+3];

			r += img[((i-0)*w + (j-2))*4+0], g += img[((i-0)*w + (j-2))*4+1], b += img[((i-0)*w + (j-2))*4+2], a += img[((i-0)*w + (j-2))*4+3];
			r += img[((i-0)*w + (j-1))*4+0], g += img[((i-0)*w + (j-1))*4+1], b += img[((i-0)*w + (j-1))*4+2], a += img[((i-0)*w + (j-1))*4+3];
			r += img[((i-0)*w + (j+0))*4+0], g += img[((i-0)*w + (j+0))*4+1], b += img[((i-0)*w + (j+0))*4+2], a += img[((i-0)*w + (j+0))*4+3];
			r += img[((i-0)*w + (j+1))*4+0], g += img[((i-0)*w + (j+1))*4+1], b += img[((i-0)*w + (j+1))*4+2], a += img[((i-0)*w + (j+1))*4+3];
			r += img[((i-0)*w + (j+2))*4+0], g += img[((i-0)*w + (j+2))*4+1], b += img[((i-0)*w + (j+2))*4+2], a += img[((i-0)*w + (j+2))*4+3];

			r += img[((i+1)*w + (j-2))*4+0], g += img[((i+1)*w + (j-2))*4+1], b += img[((i+1)*w + (j-2))*4+2], a += img[((i+1)*w + (j-2))*4+3];
			r += img[((i+1)*w + (j-1))*4+0], g += img[((i+1)*w + (j-1))*4+1], b += img[((i+1)*w + (j-1))*4+2], a += img[((i+1)*w + (j-1))*4+3];
			r += img[((i+1)*w + (j+0))*4+0], g += img[((i+1)*w + (j+0))*4+1], b += img[((i+1)*w + (j+0))*4+2], a += img[((i+1)*w + (j+0))*4+3];
			r += img[((i+1)*w + (j+1))*4+0], g += img[((i+1)*w + (j+1))*4+1], b += img[((i+1)*w + (j+1))*4+2], a += img[((i+1)*w + (j+1))*4+3];
			r += img[((i+1)*w + (j+2))*4+0], g += img[((i+1)*w + (j+2))*4+1], b += img[((i+1)*w + (j+2))*4+2], a += img[((i+1)*w + (j+2))*4+3];

			r += img[((i+2)*w + (j-2))*4+0], g += img[((i+2)*w + (j-2))*4+1], b += img[((i+2)*w + (j-2))*4+2], a += img[((i+2)*w + (j-2))*4+3];
			r += img[((i+2)*w + (j-1))*4+0], g += img[((i+2)*w + (j-1))*4+1], b += img[((i+2)*w + (j-1))*4+2], a += img[((i+2)*w + (j-1))*4+3];
			r += img[((i+2)*w + (j+0))*4+0], g += img[((i+2)*w + (j+0))*4+1], b += img[((i+2)*w + (j+0))*4+2], a += img[((i+2)*w + (j+0))*4+3];
			r += img[((i+2)*w + (j+1))*4+0], g += img[((i+2)*w + (j+1))*4+1], b += img[((i+2)*w + (j+1))*4+2], a += img[((i+2)*w + (j+1))*4+3];
			r += img[((i+2)*w + (j+2))*4+0], g += img[((i+2)*w + (j+2))*4+1], b += img[((i+2)*w + (j+2))*4+2], a += img[((i+2)*w + (j+2))*4+3];

			out[(i*w + j)*4 + 0] = r / 25;
			out[(i*w + j)*4 + 1] = g / 25;
			out[(i*w + j)*4 + 2] = b / 25;
			out[(i*w + j)*4 + 3] = a / 25;
		}
	}
}

void smooth5()
{
	int i, j;
	int size = 64;
	int qw = width / size;
	int rw = width % size;
	int qh = height / size;
	int rh = height % size;
	int sw = qw + (rw == 0? 0:1);
	int sh = qh + (rh == 0? 0:1);

	struct slice slice;

	for (i = 0; i < sh; i++) {
		for (j = 0; j < sw; j++) {
			slice.x0 = j*size;
			slice.x1 = MIN((j+1)*size, width);
			slice.y0 = i*size;
			slice.y1 = MIN((i+1)*size, height);
			smooth5_slice(&slice);
		}
	}
}

void read_or_die(int fd, void *buf, size_t count)
{
	ssize_t sz = read(fd, buf, count);
	if (sz != count)
		exit(2);
}

void write_or_die(int fd, const void *buf, size_t count)
{
	ssize_t sz = write(fd, buf, count);
	if (sz != count)
		exit(3);
}

void load()
{
	int i;
	int file = open("image.in", O_RDONLY);

	if (file == -1) {
		perror("open");
		exit(1);
	}

	read_or_die(file, &width, sizeof(width));
	read_or_die(file, &height, sizeof(height));

	for (i = BORDER; i < height + BORDER; i++)
		read_or_die(file, &img[(i*(width+BORDER*2) + BORDER)*4], width*4);
}

void store()
{
	int i;
	int file = creat("image.out", 0644);

	if (file == -1) {
		perror("creat");
		exit(1);
	}

	write_or_die(file, &width, sizeof(width));
	write_or_die(file, &height, sizeof(height));

	for (i = BORDER; i < height + BORDER; i++)
		write_or_die(file, &out[(i*(width+BORDER*2) + BORDER)*4], width*4);
}

uint64_t tick()
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return tp.tv_sec*1000000000 + tp.tv_nsec;
}

int main(int argc, char *argv[])
{
	time_it(load);
	time_it(smooth5);
	time_it(store);
	return 0;
}
