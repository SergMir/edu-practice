#include "common.h"
#include <stdio.h>	// For printf
#include <stdbool.h>	// For bool
#include <stdint.h>	// For uintptr_t
#include <stdlib.h>	// For srand()+rand()

static bool ready;

static void* p1(void *arg)
{
	int cpu = (uintptr_t)arg;

	printf("--> %s (cpu %d)\n", __func__, cpu);
	set_affinity(cpu);

	msleep(2);

	#if 1
	ready = true;
	#else
	WRITE_ONCE(ready, true);
	#endif

	printf("%s: made ready\n", __func__);

	printf("<-- %s\n", __func__);
	return NULL;
}

static void* p2(void *arg)
{
	int cpu = (uintptr_t)arg;

	printf("--> %s (cpu %d)\n", __func__, cpu);
	set_affinity(cpu);

	#if 1
	unsigned long long wait_cnt = 10;
	while (!ready) {
		usleep(100 * 1000);
	}
	#elif 1
	unsigned long long wait_cnt = 100 * 1000 * 1000;
	while (!ready) {
		if (!--wait_cnt)
			break;
	}
	#else
	unsigned long long wait_cnt = -1;
	while (!READ_ONCE(ready)) {
		if (!--wait_cnt)
			break;
	}
	#endif

	printf("%s: %s%s\n", __func__,
	       ready ? "ready" : "NOT ready!",
	       wait_cnt ? "" : " (timeout)");

	printf("<-- %s\n", __func__);
	return NULL;
}

int main()
{
	pthread_attr_t attr;
	pthread_t id1, id2;
	bool ready = false;

	pthread_attr_init(&attr);

	set_affinity(1);
	srand(time(NULL));

	pthread_create(&id1, &attr, &p1, (void*)2);
	pthread_create(&id2, &attr, &p2, (void*)3);

	pthread_join(id1, NULL);
	pthread_join(id2, NULL);

	return 0;
}
