#pragma once

#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>	// For setaffinity
#include <unistd.h>	// For usleep

#define READ_ONCE(x)	(*(const volatile typeof(x) *)&(x))

#define WRITE_ONCE(x, val)				\
	do {						\
		*(volatile typeof(x) *)&(x) = (val);	\
	} while (0)

#define barrier()	asm volatile("" ::: "memory");

#if defined(__x86_64__)
#define mb()		asm volatile("mfence" ::: "memory");
#define rmb()		asm volatile("lfence" ::: "memory");
#define wmb()		asm volatile("sfence" ::: "memory");
#elif defined(__riscv)
#define mb()		asm volatile("fence iorw,iorw" ::: "memory");
#define rmb()		asm volatile("fence ir,ir" ::: "memory");
#define wmb()		asm volatile("fence ow,ow" ::: "memory");
#elif defined(__powerpc64__)
#define mb()		asm volatile("sync" : : : "memory")
#define rmb()		asm volatile("sync" : : : "memory")
#define wmb()		asm volatile("sync" : : : "memory")
#else
#error "Unknown arch!"
#endif

static void set_affinity(int cpu)
{
	cpu_set_t set;

	CPU_ZERO(&set);
	CPU_SET(cpu, &set);
	sched_setaffinity(pthread_self(), sizeof(set), &set);
}

#define msleep(msecs)	usleep((msecs) * 1000);
