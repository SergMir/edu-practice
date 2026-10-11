#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static size_t shell_cmd(const char *cmd, char *buf, size_t buf_len)
{
	FILE *f = popen(cmd, "r");

	if (!f) {
		return 0;
	}

	size_t ret = fread(buf, 1, buf_len, f);
	fclose(f);

	return ret;
}

int main()
{
	char isa[1024];
	shell_cmd("grep \"hart isa\" /proc/cpuinfo | sort | uniq", isa, sizeof(isa));
	isa[strlen(isa) - 1] = '\0';

	char model[128];
	shell_cmd("cat /sys/firmware/devicetree/base/model", model, sizeof(model));

	char version1[128];
	shell_cmd("cat /sys/firmware/devicetree/base/chosen/u-boot,version", version1, sizeof(version1));

	char version2[128];
	shell_cmd("uname -r", version2, sizeof(version2));
	version2[strlen(version2) - 1] = '\0';

	char email[] = "test@test.test";

	char total1[1024];
	snprintf(total1, sizeof(total1), "%s %s %s", version1, email, model);

	char total2[1024];
	snprintf(total2, sizeof(total2), "%s %s", version2, isa);

	size_t len = MIN(strlen(total1), strlen(total2)) / sizeof(uint32_t);

	uint32_t *t1 = (typeof(t1))total1;
	uint32_t *t2 = (typeof(t2))total2;

	uint32_t result = 0;

	for (unsigned int i = 0; i < len; ++i) {
		result |= (t1[i] >> 1) + (~t2[i] >> 1);
	}

	printf("Result: %u\n", result);

	return 0;
}
