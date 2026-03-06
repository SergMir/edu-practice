#include "common.h"
#include <stdio.h>	// For printf
#include <stdbool.h>	// For bool
#include <stdint.h>	// For uintptr_t
#include <stdlib.h>	// For srand()+rand()

static volatile bool start, s0, s1;
static volatile int X, Y, A, B;

static void* p1(void *arg)
{
	bool do_barriers = (uintptr_t)arg;

	set_affinity(2);

	s0 = true;
	while (!start)
		;

	X = 1;
	barrier();
	if (do_barriers)
		mb();
	A = Y;

	return NULL;
}

static void* p2(void *arg)
{
	bool do_barriers = (uintptr_t)arg;

	set_affinity(3);

	s1 = true;
	while (!start)
		;

	#if 0
	int wait = rand() % 5 + 1;
	while (--wait)
		;
	#endif

	Y = 1;
	barrier();
	if (do_barriers)
		mb();
	B = X;

	return NULL;
}

int loop(bool do_barriers)
{
	pthread_attr_t attr;
	pthread_t id1, id2;
	bool ready = false;

	pthread_attr_init(&attr);

	set_affinity(1);
	srand(time(NULL));

	int c00 = 0, c01 = 0, c10 = 0, c11 = 0;
	int cnt = 1000;

	while (--cnt) {
		start = false;
		X = 0;
		Y = 0;
		s0 = false;
		s1 = false;

		pthread_create(&id1, &attr, &p1, (void*)do_barriers);
		pthread_setname_np(id1, "p1");
		pthread_create(&id2, &attr, &p2, (void*)do_barriers);
		pthread_setname_np(id2, "p2");

		while (!s0 || !s1)
			;

		start = true;

		pthread_join(id1, NULL);
		pthread_join(id2, NULL);

		if (!A && !B)
			++c00;
		else if (!A && B)
			++c01;
		else if (A && !B)
			++c10;
		else
			++c11;
	}

	printf("with%s barriers: !A!B %6d, !AB %6d, A!B %6d, AB %6d\n",
	       do_barriers ? "" : "out", c00, c01, c10, c11);

	return 0;
}

int main()
{
	loop(false);
	loop(true);
}
