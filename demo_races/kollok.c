#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>

#define __NR_fpmi_kollok 471

static const char *questions[26] = {
	['A'-'A'] = "Атомарные операции — докажите линеаризуемость",
	['C'-'A'] = "Сжатые инструкции — оцените сложность",
	['D'-'A'] = "Double float — расскажите про NaN и бесконечности",
	['F'-'A'] = "Single float — IEEE 754",
	['H'-'A'] = "Гипервизор — внутреннее устройство",
	['I'-'A'] = "Базовый ISA — обязательный вопрос, сдают все студенты",
	['M'-'A'] = "Целочисленное умножение — FFT и Карацуба",
	['S'-'A'] = "Supervisor mode — page fault handling",
	['U'-'A'] = "User mode — ABI, calling convention, стек вызовов",
	['V'-'A'] = "Векторные расширения — SIMD vs Vector ISA",
};

int main()
{
	unsigned long misa;
	long raw;
	int xlen, number, i;

	errno = 0;
	raw = syscall(__NR_fpmi_kollok);
	if (raw == -1) {
		fprintf(stderr,
			"Деканат не открыл дверь. errno=%d. Пересдача в сентябре\n",
			errno);
		return 1;
	}
	misa = (unsigned long)raw;

	xlen = (int)(misa >> 62) & 0x3;

	printf("misa  = 0x%016lx\n", misa);
	printf("XLEN  = %s\n", xlen == 2 ? "64-bit" : "32-bit");
	printf("билет:\n\n");

	number = 1;
	for (i = 0; i < 26; ++i) {
		if (!(misa & (1UL << i)) || !questions[i])
			continue;
		printf("  %2d. [%c] %s\n", number++, 'A' + i, questions[i]);
	}

	if (number == 1)
		printf("  (счастливый билет)\n");

	return 0;
}
