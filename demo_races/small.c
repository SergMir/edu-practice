#include <stdio.h>

static int f(void)
{
	int a = 0;
	a = a + 1;

	int b = 0;
	b++;

	return a + b;
}

int main()
{
	printf("%d\n", f());
}
