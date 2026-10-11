#include "common.h"
#include <malloc.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdbool.h>

static int pagesize;
static const int fsize = 64;

static volatile bool ready = false;

typedef int (*f_t)(int, int);

static int f1(int a, int b)
{
	return a + b;
}

static int f2(int a, int b)
{
	return a * b;
}

static int run_f(f_t f)
{
	int a = 1;
	for (int i = 0; i < 4; ++i)
		a = f(a, a + 1);
	return a;
}

static int prot(void *b, bool x)
{
	if (mprotect(b, pagesize, PROT_READ | (x ? PROT_EXEC : PROT_WRITE)) < -1) {
		int err = errno;
		printf("buf RW: %d (%s)\n", err, strerror(err));
		return -1;
	}

	return 0;
}

static void* p1(void *buf)
{
	set_affinity(2);

	while (!ready)
		;
	barrier();
	mb();

	if (prot(buf, false))
		return NULL;

	memcpy(buf, f2, fsize);

	if (prot(buf, true))
		return NULL;

	ready = false;

	return NULL;
}

static void reloop(void *buf, int *val1, int *val2, bool threaded)
{
	pthread_t tid;

	if (threaded) {
		pthread_attr_t attr;

		pthread_attr_init(&attr);
		pthread_create(&tid, &attr, &p1, buf);
		pthread_setname_np(tid, "p1");
	}

	*val1 = *val2 = 0;

	f_t f = buf;

	if (prot(buf, false))
		return;

	memcpy(buf, f1, fsize);

	if (prot(buf, true))
		return;

	__clear_cache(f, f + fsize);
	*val1 = run_f(f);

	if (threaded) {
		ready = true;

		pthread_join(tid, NULL);
	} else {
		if (prot(buf, false))
			return;
		memcpy(buf, f2, fsize);
		if (prot(buf, true))
			return;
	}

	*val2 = run_f(f);
}

static void loop(void *buf, bool threaded)
{
	int val_orig1 = run_f(f1);
	int val_orig2 = run_f(f2);
	int count_failed1 = 0, count_failed2 = 0;
	int N = 256;

	for (int i = 0; i < N; ++i) {
		int val1, val2;
		reloop(buf, &val1, &val2, threaded);

		if (val1 != val_orig1)
			++count_failed1;
		if (val2 != val_orig2)
			++count_failed2;
		if (val2 != val_orig1 && val2 != val_orig2)
			printf("weird val2: %d\n", val2);
	}

	printf("%s: failed1 %d, failed2 %d, OK %d\n",
	       threaded ? "Threaded" : "Single-thread",
	       count_failed1, count_failed2, N - count_failed1 - count_failed2);
}

int main()
{
	pagesize = getpagesize();
	set_affinity(1);

	void *buf = mmap(NULL, pagesize,
			 PROT_READ | PROT_WRITE | PROT_EXEC,
			 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (buf == MAP_FAILED) {
		int err = errno;
		printf("mmap() failed: %s\n", strerror(err));
		return -1;
	}

	loop(buf, false);
	loop(buf, true);

	return 0;
}
