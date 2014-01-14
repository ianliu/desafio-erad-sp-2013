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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <immintrin.h>

/* Largura máxima da imagem, dada no problema */
#define MAX_W 7680

/* Altura máxima da imagem, dada no problema */
#define MAX_H 4320

/* Tamanho da borda de zeros na imagem para um smooth de janela 5x5 */
#define BORDER 4

/* Tamanho máximo de uma imagem */
#define MAX_SIZE ((MAX_W+2*BORDER)*(MAX_H+2*BORDER)*4)

/* Tamanho da fatia */
#define SLICE 64

/* Número máximo de fatias de tamanho SLICE */
#define MAX_SLICES ((MAX_W/SLICE+1) * (MAX_H/SLICE+1))

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

#ifndef NUM_THREADS
# define NUM_THREADS 8
#endif

#define ALIGN 8

static uint16_t width, height;
static uint8_t img[MAX_SIZE + ALIGN] __attribute__((aligned(0x10))) = {0,};
static uint8_t out[MAX_SIZE] = {0,};
static int NUM_SLICES;

struct slice {
	int x0, x1;
	int y0, y1;
} slices[MAX_SLICES];

void smooth5_slice(struct slice *slice)
{
	int w = width + 2*BORDER;
	int i, j, k, l, x0, x1, y0, y1;
	x0 = BORDER + slice->x0;
	x1 = BORDER + slice->x1;
	y0 = BORDER + slice->y0;
	y1 = BORDER + slice->y1;
	for (i = y0; i < y1; i++) {
		for (j = x0; j < x1; j += 16) {
			int idx;
			__m128i line;
			__m128i zero = _mm_setzero_si128();
			__m128i s0 = _mm_setzero_si128();
			__m128i s1 = _mm_setzero_si128();
			__m128i s2 = _mm_setzero_si128();
			__m128i s3 = _mm_setzero_si128();
			__m128i s4 = _mm_setzero_si128();
			__m128i s5 = _mm_setzero_si128();
			__m128i s6 = _mm_setzero_si128();
			__m128i s7 = _mm_setzero_si128();
			__m128i s8 = _mm_setzero_si128();
			__m128i s9 = _mm_setzero_si128();

			for (k = i-2; k <= i+2; k++) {
				idx = (k*w + j-2)*4;
				line = _mm_load_si128((const __m128i *)&img[idx + ALIGN]);
				s0 = _mm_add_epi16(s0, _mm_unpacklo_epi8(line, zero));
				s1 = _mm_add_epi16(s1, _mm_unpackhi_epi8(line, zero));

				idx += 16;
				line = _mm_load_si128((const __m128i *)&img[idx + ALIGN]);
				s2 = _mm_add_epi16(s2, _mm_unpacklo_epi8(line, zero));
				s3 = _mm_add_epi16(s3, _mm_unpackhi_epi8(line, zero));

				idx += 16;
				line = _mm_load_si128((const __m128i *)&img[idx + ALIGN]);
				s4 = _mm_add_epi16(s4, _mm_unpacklo_epi8(line, zero));
				s5 = _mm_add_epi16(s5, _mm_unpackhi_epi8(line, zero));

				idx += 16;
				line = _mm_load_si128((const __m128i *)&img[idx + ALIGN]);
				s6 = _mm_add_epi16(s6, _mm_unpacklo_epi8(line, zero));
				s7 = _mm_add_epi16(s7, _mm_unpackhi_epi8(line, zero));

				idx += 16;
				line = _mm_load_si128((const __m128i *)&img[idx + ALIGN]);
				s8 = _mm_add_epi16(s8, _mm_unpacklo_epi8(line, zero));
				s9 = _mm_add_epi16(s9, _mm_unpackhi_epi8(line, zero));
			}

			uint64_t pixels[21];
			uint64_t s = 0;

			_mm_storeu_si128((__m128i *)&pixels[0], s0);
			_mm_storeu_si128((__m128i *)&pixels[2], s1);
			_mm_storeu_si128((__m128i *)&pixels[4], s2);
			_mm_storeu_si128((__m128i *)&pixels[6], s3);
			_mm_storeu_si128((__m128i *)&pixels[8], s4);
			_mm_storeu_si128((__m128i *)&pixels[10], s5);
			_mm_storeu_si128((__m128i *)&pixels[12], s6);
			_mm_storeu_si128((__m128i *)&pixels[14], s7);
			_mm_storeu_si128((__m128i *)&pixels[16], s8);
			_mm_storeu_si128((__m128i *)&pixels[18], s9);

			for (l = 0; l < 5; l++)
				s += pixels[l];

			for (l = j; l < j+16; l++) {
				out[((i-BORDER)*width + l-BORDER)*4 + 0] = ((s & 0x000000000000ffff)) / 25;
				out[((i-BORDER)*width + l-BORDER)*4 + 1] = ((s & 0x00000000ffff0000) >> 16) / 25;
				out[((i-BORDER)*width + l-BORDER)*4 + 2] = ((s & 0x0000ffff00000000) >> 32) / 25;
				out[((i-BORDER)*width + l-BORDER)*4 + 3] = ((s & 0xffff000000000000) >> 48) / 25;
				s -= pixels[l-j];
				s += pixels[l-j+5];
			}
		}
	}
}

void create_slices()
{
	int i, j, k = 0;
	int size = 64;
	int qw = width / size;
	int rw = width % size;
	int qh = height / size;
	int rh = height % size;
	int sw = qw + (rw == 0? 0:1);
	int sh = qh + (rh == 0? 0:1);

	for (i = 0; i < sh; i++) {
		for (j = 0; j < sw; j++) {
			slices[k].x0 = j*size;
			slices[k].x1 = MIN((j+1)*size, width);
			slices[k].y0 = i*size;
			slices[k].y1 = MIN((i+1)*size, height);
			k++;
		}
	}
	NUM_SLICES = k;
}

void *smooth_slices(void *data)
{
	int i;
	uintptr_t tid = (uintptr_t) data;
	for (i = tid; i < NUM_SLICES; i += NUM_THREADS)
		smooth5_slice(&slices[i]);
	return NULL;
}

void smooth5()
{
	uintptr_t k;
	create_slices();
	pthread_t threads[NUM_THREADS];

	for (k = 0; k < NUM_THREADS; k++)
		pthread_create(&threads[k], NULL, smooth_slices, (void*)k);

	for (k = 0; k < NUM_THREADS; k++)
		pthread_join(threads[k], NULL);
	return;
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
		read_or_die(file, &img[(i*(width+BORDER*2) + BORDER)*4 + ALIGN], width*4);
}

void store()
{
	int file = creat("image.out", 0644);

	if (file == -1) {
		perror("creat");
		exit(1);
	}

	write_or_die(file, &width, sizeof(width));
	write_or_die(file, &height, sizeof(height));
	ssize_t written = 0;
	while (written < width*height*4)
		written += write(file, &out[0], width*height*4);
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
