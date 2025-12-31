#include "process_communication.h"
#include <sys/wait.h>


void PsAuxCliFilter(const char* word)
{
    int fd[2] = {};
    pipe(fd);
    fprintf(stdout, "My pid is %d, now I begin fork.\n", getpid());
    pid_t pid = fork();
    int ret;

    if (pid == -1) {
        perror("Fork err.");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        fprintf(stdout, "My pid is %d, the fork pid is %d.\n", getpid(), pid);
        ret = close(fd[0]);
        if (ret < 0) {
            perror("Close err.");
            exit(EXIT_FAILURE);
        }
        dup2(fd[1], STDOUT_FILENO);
        execlp("ps", "ps", "aux", NULL);
        perror("Exec failed.");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "My pid is %d, origin pid is %d.\n", getpid(), getppid());
    ret = close(fd[1]);
    if (ret < 0) {
        perror("Close err.");
        exit(EXIT_FAILURE);
    }
    char text[4096] = {};
    char *buff;
    int SIZE = 1024;
    int readSize = 0;
    int buffIndex = 0;
    buff = malloc(sizeof(char) * SIZE);

    while ((readSize = read(fd[0], text, sizeof(text))) > 0) {
        for (int i = 0; i < readSize; ++i) {
            if (buffIndex >= SIZE) {
                SIZE *= 2;
                buff = realloc(buff, SIZE);
            }
            if (text[i] != '\n') {
                buff[buffIndex++] = text[i];
                continue;
            }
            buff[buffIndex] = '\0';
            if (strstr(buff, word) != NULL) {
                fprintf(stdout, "%s\n", buff);
            }
            buffIndex = 0;
        }
    }
    free(buff);
    close(fd[0]);
    fprintf(stdout, "\n%d print over.\n", getpid());
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        fprintf(stdout, "任务完成，返回值：%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        fprintf(stdout, "任务失败，错误码：%d\n", WTERMSIG(status));
    }
    exit(EXIT_SUCCESS);
}