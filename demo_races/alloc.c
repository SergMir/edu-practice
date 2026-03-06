#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void alloc_1x1T(void) {
	const size_t zsize = (size_t)1024 * 1024 * 1024 * 1024;
	char *arr = malloc(zsize);

	if (!arr) {
		printf("Can't alloc\n");
		return;
	}
	printf("Allocated, press <Enter>\n");
	getchar();
	mb();

	WRITE_ONCE(arr[0], 7);
	printf("Write to 0: OK, press <Enter>\n");
	getchar();
	mb();

	WRITE_ONCE(arr[zsize / 2], 0);
	printf("Write to %dM: OK, press <Enter>\n", zsize / 1024 / 1024 / 2);
	getchar();

	const int M = 1024 * 1024;
	const int P = 4 * 1024;
	for (size_t m = 0; m < zsize; m += M) {
		for (size_t p = 0; p < M; ++p) {
			WRITE_ONCE(arr[zsize / 2 + m + p], 0);
			mb();
			if (m > 157 * 1024 * 1024)
				if ((0 == p % P) ||
				    (1 == p % P) ||
				    (P / 2 == p % P) ||
				    ((P / 2 + 1) == p % P) ||
				    ((P - 1) == p % P))
					printf("Write to 0x%x: OK\n", p);
		}
		printf("Write to %dM: OK\n", m / 1024 / 1024);
		if (m == 100 * 1024 * 1024)
			getchar();
	}
}

static void alloc_128x128M(void)
{
	const size_t S = 128 * 1024 * 1024;
	const int N = 128;
	char* z[N];
	size_t total_allocated = 0;
	printf("z: %p\n", z);

	for (int i = 0; i < N; ++i) {
		z[i] = malloc(S);
		total_allocated += S;
		if (z[i]) {
			printf("Allocated i %d, %zuM total, %p\n", i, total_allocated / 1024 / 1024, z[i]);
		} else {
			printf("Allocation failed\n");
		}
	}

	for (int i = 0; i < N; ++i) {
		z[i][0] = i;
		printf("Write to i %d: OK\n", i);
	}
	printf("Header: \n");
	for (int i = 0; i < 16; ++i) {
		printf("%02x ", *((char*)(z[0] - 16 + i)));
	}
	getchar();
}

int main(int argc, const char* argv[])
{
	if (argc == 2 && !strncmp("128", argv[1], 3))
		alloc_128x128M();
	else
		alloc_1x1T();

	return 0;
}
