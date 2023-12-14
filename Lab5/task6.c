#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

int printDir(char *dirname) {
	DIR *dir = opendir(dirname);
	if (dir == NULL)
	{
		printf("Can't open directory \"%s\"\n", dirname);
		return 1;
	}

	struct dirent *dirInfo = readdir(dir);
	printf("Directory \"%s\":\n", dirname);
	while (dirInfo != NULL)
	{
		printf("\t%s\n", dirInfo -> d_name);
		dirInfo = readdir(dir);
	}

	if (closedir(dir) == -1)
	{
		printf("Can't close directory \"%s\"\n", dirname);
		return 1;
	}

	return 0;
}

int main(int argc, char *argv[], char *envp[]) {
	if (argc != 2)
	{
		printf("Usage: %s <directory>\n", argv[0]);
		return 1;
	}

	char wd[256];
	if (getcwd(wd, sizeof(wd)) == NULL)
	{
		printf("Can't get working directory");
		return 1;
	}

	int wdPrintStatus = printDir(wd);
	if (wdPrintStatus == 1)
	{
		return 1;
	}

	int directoryPrintStatus = printDir(argv[1]);
	if (wdPrintStatus == 1)
	{
		return 1;
	}

	return 0;
}
