#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE *f = fopen(argv[1], "w");
	if (f == NULL)
	{
		printf("Can't open file \"%s\"\n", argv[1]);
		return 1;
	}

	printf("Press Ctrl-F to exit\n");
	int ch = getc(stdin);

	while (ch != 6)
	{
		putc(ch, f);
		ch = getc(stdin);
	}

	if (fclose(f) == EOF)
	{
		printf("Can't close file \"%s\"\n", argv[1]);
		return 1;
	}

	return 0;
}
