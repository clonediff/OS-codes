#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_current_time_string()
{
	struct timeval tv;
	char *time_string = malloc(30);

	gettimeofday(&tv, NULL);

	sprintf(time_string, "%ld", tv.tv_sec * 1000 + (int)tv.tv_usec / 1000);
	return time_string;
}

int main(int argc, char *argv[], char *envp[])
{
	if (argc != 3)
	{
		printf("Usage: %s <gensCount> <childrenCount>\n", argv[0]);
		return 1;
	}

	int i, j, gensCount, childrenCount;
	int processNum = 0;

	char *endPtr;
	gensCount = strtol(argv[1], &endPtr, 10);
	if (*endPtr != '\0' || gensCount <= 0)
	{
		printf("Usage: %s <gensCount> <childrenCount>, gensCount should be integer and > 0\n", argv[0]);
		return 1;
	}

	childrenCount = strtol(argv[2], &endPtr, 10);
	if (*endPtr != '\0' || childrenCount <= 0)
	{
		printf("Usage: %s <gensCount> <childrenCount>, childrenCount should be integer and > 0\n", argv[0]);
		return 1;
	}

	for (i = 0; i < gensCount; i++)
		if (fork() == 0)
		{
			processNum = i + 1;
			for (j = 0; j < childrenCount; j++)
				if (fork() == 0)
				{
					processNum = gensCount + childrenCount * i + j + 1;
					break;
				}
			break;
		}
	if (processNum == 0)
		printf("Номер процесса в дереве=%d, pid=%d, ppid=%d, текуще время=%s\n",
			processNum, getpid(), getppid(), get_current_time_string());
	else
		while (1)
		{
			printf("\033[1;3%dmНомер процесса в дереве=%d, pid=%d, ppid=%d, текуще время=%s\033[0m\n",
				processNum, processNum, getpid(), getppid(), get_current_time_string());
			usleep(processNum * 200 * 1000);
		}

	if (processNum == 0)
		for (i = 0; i < gensCount; i++)
			wait(NULL);
	else if (1 <= processNum && processNum <= gensCount)
		for (i = 0; i < childrenCount; i++)
			wait(NULL);

	return 0;
}
