#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

struct FileInfo
{
	char *fullName;
	int fileSize;
};

struct PathSegments
{
	char** segments;
	int count;
};

typedef int (*ComparerDelegate)(const void *, const void *);

int visitDirectory(char* initDir, char* dirname, struct FileInfo** files, int* filesCount, int* filesCapacity)
{
	char *curdirname = malloc(strlen(initDir) + 1 + strlen(dirname) + 1);
	sprintf(curdirname, "%s/%s", initDir, dirname);
	DIR *directory = opendir(curdirname);
	if (directory == NULL)
	{
		printf("Can't open directory \"%s\"\n", curdirname);
		return 1;
	}

	struct dirent *directoryEntry = readdir(directory);
	while (directoryEntry != NULL)
	{
		if (strcmp(directoryEntry->d_name, ".") == 0 || strcmp(directoryEntry->d_name, "..") == 0)
		{
			directoryEntry = readdir(directory);
			continue;
		}

		char *entryName = malloc(strlen(curdirname) + 1 + strlen(directoryEntry->d_name) + 1);
		sprintf(entryName, "%s/%s", curdirname, directoryEntry->d_name);

		struct stat entryStat;
		stat(entryName, &entryStat);

		if (S_ISDIR(entryStat.st_mode))
		{
			int entryDirectoryVisitStatus = visitDirectory(initDir, entryName + strlen(initDir) + 1, files, filesCount, filesCapacity);
			if (entryDirectoryVisitStatus == 1)
			{
				return 1;
			}
		}
		else
		{
			if (*filesCount == *filesCapacity)
			{
				*filesCapacity *= 2;
				*files = realloc(*files, *filesCapacity * sizeof(struct FileInfo));
			}

			(*files)[(*filesCount)].fullName = malloc(strlen(entryName) - strlen(initDir));
			strcpy((*files)[(*filesCount)].fullName, entryName + strlen(initDir) + 1);
			(*files)[(*filesCount)].fileSize = entryStat.st_size;

			(*filesCount)++;
		}

		directoryEntry = readdir(directory);
	}

	if (closedir(directory) == -1)
	{
		printf("Can't close directory \"%s\"\n", dirname);
		return 1;
	}

	return 0;
}

int nameComparer(const void *a, const void *b)
{
	return strcmp((*(struct FileInfo*)a).fullName, (*(struct FileInfo*)b).fullName);
}

int sizeComparer(const void *a, const void *b)
{
	return ((*(struct FileInfo*)a).fileSize - (*(struct FileInfo*)b).fileSize);
}

char* getAbsolutePath(char* dirname)
{
	char *cwd = malloc(256);
	getcwd(cwd, 256);
	strcpy(cwd + strlen(cwd), "/");

	if (*(dirname) == '/')
	{
		cwd = realloc(cwd, strlen(dirname) + 1);
		strcpy(cwd, dirname);
	}
	else
	{
		cwd = realloc(cwd, strlen(dirname) + strlen(cwd) + 1);
		strcpy(cwd + strlen(cwd), dirname);
	}

	return cwd;
}

struct PathSegments getPathSegments(char* dirname)
{
	dirname = getAbsolutePath(dirname);

	int count = 0, capacity = 2;
	char** list = malloc(capacity * sizeof(char*));

	char* segment = strtok(dirname, "/");
	while (segment != NULL)
	{
		if (strlen(segment) != 0)
		{
			if (count == capacity)
			{
				capacity *= 2;
				list = realloc(list, capacity * sizeof(char*));
			}

			list[count++] = segment;
		}

		segment = strtok(NULL, "/");
	}

	struct PathSegments res;
	res.segments = list;
	res.count = count;
	return res;
}

int copyFile(char *sourceName, char *destName)
{
	FILE *source = fopen(sourceName, "r");
	if (source == NULL)
	{
		return 1;
	}

	struct stat sourceStats;
	stat(sourceName, &sourceStats);

	FILE *dest = fopen(destName, "w");
	if (dest == NULL)
	{
		return 1;
	}

	chmod(destName, sourceStats.st_mode);

	char ch = fgetc(source);
	while (ch != EOF)
	{
		putc(ch, dest);
		ch = fgetc(source);
	}

	if (fclose(source) == EOF)
	{
		printf("Can't close file \"%s\"\n", sourceName);
		return 1;
	}
	if (fclose(dest) == EOF)
	{
		printf("Can't close file \"%s\"\n", destName);
		return 1;
	}

	return 0;
}

int createDirForFile(char* fileName)
{
	struct PathSegments pathSegments = getPathSegments(fileName);
	char* acc = malloc(1);
	*acc = 0;
	for (int i = 0; i < pathSegments.count - 1; i++)
	{
		acc = realloc(acc, strlen(acc) + 1 + strlen(pathSegments.segments[i]) + 1);
		sprintf(acc, "%s/%s", acc, pathSegments.segments[i]);

		DIR *dir = opendir(acc);
		if (dir == NULL)
		{
			if (strcmp(pathSegments.segments[i], ".") == 0 || strcmp(pathSegments.segments[i], "..") == 0)
				continue;
			if (mkdir(acc, 0755) == -1)
				return 1;
		} 
		else
			closedir(dir);
	}
	return 0;
}

void copyFiles(char* destDir, char* sourceDir, struct FileInfo* files, int filesCount)
{
	char *destFileName;
	char *sourceFileName;
	for (int i = 0; i < filesCount; i++)
	{
		destFileName = malloc(strlen(destDir) + 1 + strlen(files[i].fullName) + 1);
		sprintf(destFileName, "%s/%s", destDir, files[i].fullName);

		sourceFileName = malloc(strlen(sourceDir) + 1 + strlen(files[i].fullName) + 1);
		sprintf(sourceFileName, "%s/%s", sourceDir, files[i].fullName);

		if (createDirForFile(destFileName) == 1)
		{
			printf("Can't create directory for file \"%s\"\n", destFileName);
			continue;
		}

		if (copyFile(sourceFileName, destFileName) == 1)
		{
			printf("Can't copy file \"%s\" to \"%s\"", sourceFileName, destFileName);
			continue;
		}

		destFileName = realpath(destFileName, NULL);

		printf("Directory: %s, FileName: %s, FileSize: %d\nCopied successfully!\n", dirname(destFileName), basename(destFileName), files[i].fileSize);
	}
}

int main(int argc, char *argv[], char *envp[]) {

	#pragma region Init
	if (argc != 4)
	{
		printf("Usage: %s <directory> <sort_mode> <new_directory>\nsort_mode: 1 - by file size(incr), 2 - by file name\n", argv[0]);
		return 1;
	}

	char *endPtr;
	int sortMode = strtol(argv[2], &endPtr, 10);
	if (*endPtr != '\0' || sortMode < 0 || sortMode > 2)
	{
		printf("Usage: %s <directory> <sort_mode> <new_directory>\nsort_mode: 1 - by file size(incr), 2 - by file name\n", argv[0]);
		return 1;
	}

	int filesCount = 0, filesCapacity = 2;
	struct FileInfo *files = malloc(filesCapacity * sizeof(struct FileInfo));
	#pragma endregion

	#pragma region collect file infos
	int initVisitStatus = visitDirectory(getAbsolutePath(argv[1]), ".", &files, &filesCount, &filesCapacity);
	if (initVisitStatus == 1)
	{
		return 1;
	}
	#pragma endregion

	#pragma region sort files
	ComparerDelegate comparer = sizeComparer;
	if (sortMode == 2)
	{
		comparer = nameComparer;
	}
	qsort(files, filesCount, sizeof(struct FileInfo), comparer);
	#pragma endregion

	#pragma region copy files
	copyFiles(getAbsolutePath(argv[3]), getAbsolutePath(argv[1]), files, filesCount);
	#pragma endregion

	return 0;
}
