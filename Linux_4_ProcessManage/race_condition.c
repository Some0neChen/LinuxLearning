#include "program_test.h"
#include <string.h>
#include <fcntl.h>
/*
📝 试炼三：并发写日志 (race_condition.c)
目标：验证 Fork 后各自 Open 文件的竞争问题。

要求：

先 Fork。

父子进程各自执行 int fd = open("race.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);（注意：不加 O_APPEND）。

父进程：循环 5 次，每次往文件写 "PARENT\n"，每次写完 sleep(1)。

子进程：循环 5 次，每次往文件写 "CHILD\n"，每次写完 sleep(1)。

父进程最后负责 wait() 子进程。

结果分析：

运行程序。

cat race.log。

你看到了什么？是 10 行完整的文字，还是乱七八糟的覆盖？

修正：修改 open 参数，加上 O_APPEND，再次运行，验证结果。
*/

void testForkAndUseFd()
{
    // 先测试创建完fd再fork
    /*
    const char* fileName = "./race.log";
    int times = 5;
    int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open err");
        exit(EXIT_FAILURE);
    }
    fprintf("Get fd %d\n", fd);

    fprintf(stdout, "Fork begin.Right now pid is %d.\n", getpid());
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork err");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        const char* txt = "CHILD\n";
        fprintf(stdout, "我是子进程 %d[from %d], 我开始写\n", getpid(), getppid());
        while (times--) {
            write(fd, txt, strlen(txt));
            sleep(1);
        }
        exit(EXIT_SUCCESS);
    }
    const char* txt = "PARENT\n";
    fprintf(stdout, "我是父进程 %d, 我开始写\n", getpid());
    while (times--) {
        write(fd, txt, strlen(txt));
        sleep(1);
    }
    // 收！
    fprintf(stdout, "我是父进程 %d, 我开始收！\n", getpid());
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        fprintf(stdout, "任务完成，返回值：%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        fprintf(stdout, "任务失败，错误码：%d\n", WTERMSIG(status));
    }
    return;
    */

    // 测试先fork再创建fd
    const char* fileName = "./race.log";
    int times = 5;

    fprintf(stdout, "Fork begin.Right now pid is %d.\n", getpid());
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork err");
        exit(EXIT_FAILURE);
    }

    // int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    // 测试加O_APPEND场景
    int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);
    if (fd == -1) {
        perror("open err");
        exit(EXIT_FAILURE);
    }
    fprintf("Get fd %d\n", fd);

    if (pid == 0) {
        const char* txt = "CHILD\n";
        fprintf(stdout, "我是子进程 %d[from %d], 我开始写\n", getpid(), getppid());
        while (times--) {
            write(fd, txt, strlen(txt));
            sleep(1);
        }
        exit(EXIT_SUCCESS);
    }
    const char* txt = "PARENT\n";
    fprintf(stdout, "我是父进程 %d, 我开始写\n", getpid());
    while (times--) {
        write(fd, txt, strlen(txt));
        sleep(1);
    }
    // 收！
    fprintf(stdout, "我是父进程 %d, 我开始收！\n", getpid());
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        fprintf(stdout, "任务完成，返回值：%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        fprintf(stdout, "任务失败，错误码：%d\n", WTERMSIG(status));
    }
    return;
}