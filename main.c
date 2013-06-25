#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	int file = open("image.in", 0);

	if (file == -1) {
		perror("open");
		exit(1);
	}



	return 0;
}
