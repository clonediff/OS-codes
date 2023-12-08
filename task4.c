#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]) {
	if (argc != 3)
	{
		printf("Usage: %s <filename> <N>\n", argv[0]);
		return 1;
	}

	char *endPtr;
	int n = strtol(argv[2], &endPtr, 10);
	if (*endPtr != '\0' || n < 0)
	{
		printf("Usage: %s <filename> <N>, N should be integer and >= 0\n", argv[0]);
		return 1;
	}

	FILE *f = fopen(argv[1], "r");
	if (f == NULL)
	{
		printf("Can't open file \"%s\"\n", argv[1]);
		return 1;
	}

	char printCh = fgetc(f);
	while (printCh != EOF)
	{
		int counter = 0;
		while ((n == 0 || counter < n) && printCh != EOF)
		{
			if (printCh == '\n')
				counter++;
			putc(printCh, stdout);
			printCh = fgetc(f);
		}
		if (printCh != EOF)
		{
			int userCh = getc(stdin);
		}
	}

	if (fclose(f) == EOF)
	{
		printf("Can't close file \"%s\"\n", argv[1]);
		return 1;
	}

	return 0;
}
