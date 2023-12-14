#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char* get_current_time_string()
{
	struct timeval tv;
	struct tm *current_time;
	char* time_string = (char*)malloc(12);

	gettimeofday(&tv, NULL);

	current_time = localtime(&(tv.tv_sec));
	strftime(time_string, 12, "%X:", current_time);
	sprintf(time_string + strlen(time_string), "%03d", (int)tv.tv_usec / 1000);
	return time_string;
}

int main(int argc, char *argv[], char *envp[]) {
	pid_t pid1, pid2;

	pid1 = fork();
	if (pid1 > 0)
		pid2 = fork();

	if (pid1 > 0 && pid2 > 0) // Родитель
	{
		printf("Это Родительский процесс, его pid=%d, текущее время=%s\n", getpid(), get_current_time_string());
		system("ps -x");
		wait(NULL);
		wait(NULL);
	}
	else if (pid1 == 0) // Дочерний 1
	{
		printf("Это Первый Дочерний процесс, его pid=%d, pid родителя=%d, текущее время=%s\n",
		 getpid(), getppid(), get_current_time_string());
	}
	else if (pid2 == 0) // Дочерний 2
	{
		printf("Это Второй Дочерний процесс, его pid=%d, pid родителя=%d, текущее время=%s\n",
		 getpid(), getppid(), get_current_time_string());
	}

	return 0;
}
