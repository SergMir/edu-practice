#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifndef __NR_machine_oracle
#define __NR_machine_oracle 471
#endif

int main(void)
{
	long value;

	errno = 0;
	value = syscall(__NR_machine_oracle);
	if (value == -1 && errno) {
		fprintf(stderr, "machine_oracle syscall failed: %s\n",
			strerror(errno));
		return 1;
	}

	printf("Machine Oracle says:\n");
	printf("mvendorid from M-mode = 0x%016lx\n", (unsigned long)value);

	return 0;
}
