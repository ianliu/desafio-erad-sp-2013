#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
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

#define NUM_THREADS 4

static uint16_t width, height;
static uint8_t img[MAX_SIZE] = {0,};
static uint8_t out[MAX_SIZE] = {0,};

struct slice {
	pthread_t thread;
	int x0, x1;
	int y0, y1;
} slices[NUM_THREADS];

void *smooth5_slice(void *data)
{
	uintptr_t tid = (uintptr_t)data;
	int w = width + 2*BORDER;
	int i, j, x0, x1, y0, y1;
	int r, g, b, a;
	x0 = BORDER + slices[tid].x0;
	x1 = BORDER + slices[tid].x1;
	y0 = BORDER + slices[tid].y0;
	y1 = BORDER + slices[tid].y1;
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
	return NULL;
}

void smooth5()
{
	uintptr_t k;
	int q = height / NUM_THREADS;
	int r = height % NUM_THREADS;
	int cursor = 0;

	for (k = 0; k < NUM_THREADS; k++) {
		slices[k].x0 = 0;
		slices[k].x1 = width;
		slices[k].y0 = cursor;
		slices[k].y1 = cursor + q + (r-- > 0? 1:0);
		cursor = slices[k].y1;
		pthread_create(&slices[k].thread, NULL, smooth5_slice, (void*)k);
	}

	for (k = 0; k < NUM_THREADS; k++)
		pthread_join(slices[k].thread, NULL);
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
