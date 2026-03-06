#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	int b[4] = { 5, 6, 7, 8 };

	printf("%d\n", b[rand() % 3]);

	memset(&b, 0, sizeof(b));
}
