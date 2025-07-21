#include <stdio.h>

int main() {
	FILE *fp = fopen("all_one.img", "w");
	int buf;
	for (int i = 0; i < 16 * 1024 * 1024; i += 4) {
		buf = i;
		fwrite(&buf, 4, 1, fp);
	}
	fclose(fp);
	return 0;
}
