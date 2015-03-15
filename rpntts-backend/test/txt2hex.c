#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

	int i = 0;
	int arglen = 0;

	if (argc != 2) {
		printf("Need exactly one argument!\n");
		return -1;
	}

	arglen = strlen(argv[1]);

	for (i = 0; i < arglen; i++) {
		printf("%02X", argv[1][i]);
	}
	printf("\n");

	return 0;

}
