#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[], char *envp[]) {
	if (argc != 3)
	{
		printf("Usage: %s <source> <dest>\n", argv[0]);
		return 1;
	}

	FILE *source = fopen(argv[1], "r");
	if (source == NULL)
	{
		printf("Can't open file \"%s\"\n", argv[1]);
		return 1;
	}

	struct stat sourceStats;
	stat(argv[1], &sourceStats);

	FILE *dest = fopen(argv[2], "w");
	if (dest == NULL)
	{
		printf("Can't open file \"%s\"\n", argv[2]);
		return 1;
	}

	chmod(argv[2], sourceStats.st_mode);

	char ch = fgetc(source);
	while (ch != EOF)
	{
		putc(ch, dest);
		ch = fgetc(source);
	}

	if (fclose(source) == EOF)
	{
		printf("Can't close file \"%s\"\n", argv[1]);
		return 1;
	}

	if (fclose(dest) == EOF)
	{
		printf("Can't close file \"%s\"\n", argv[2]);
		return 1;
	}

	return 0;
}
