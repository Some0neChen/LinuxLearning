#include "process_communication.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stdout, "[%d] 您误入房间了...\n", getpid());
    }
    const char* fifoName = "./pool.fifo";
    fprintf(stdout, "[%d] 舌头舔嘴角中...\n", getpid());
    int fd = open(fifoName, O_WRONLY);
    char buff[1024] = {};
    char temp[128] = {};
    int len = 0;
    sprintf(buff, "[%d] 魔女, 开始%s.\n", getpid(), argv[1]);
    while (1) {
        fprintf(stdout, "\n[%d] 输入您要执行的动作：", getpid());
        fflush(stdout);
        len = read(0, temp, 128);
        temp[len - 1] = '\0';
        if (strcmp(temp, "stop") == 0) {
            break;
        }
        if (strcmp(temp, "exit") == 0) {
            write(fd, temp, strlen(temp));
            break;
        }
        len = sprintf(buff, "[%d] 魔女, 开始%s. 并同时进行%s.", getpid(), argv[1], temp);
        fprintf(stdout, "\n%s[%d]", buff, len);
        buff[len] = '\0';
        if (write(fd, buff, strlen(buff)) == -1) {
            perror("魔女大人无奈");
            exit(EXIT_FAILURE);
        }
    }
    fprintf(stdout, "[%d]魔女大人玩累了...\n", getpid());
    close(fd);
    return 0;
}