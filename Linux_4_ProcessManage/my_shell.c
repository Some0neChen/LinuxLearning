#include "program_test.h"


/*
🦁 试炼一：编写“魔王简易 Shell” (my_shell.c)
目标：模拟一个极简的 Shell，能执行带参数的命令，并捕获返回值。

要求：

程序启动后打印提示符 魔王Shell >。

使用 fork() 创建子进程。

子进程：使用 execlp 执行 ps -ef 命令（列出所有进程）。

进阶要求：如果 exec 失败（比如把命令改成瞎写的），必须打印错误原因，并返回一个特定的错误码（如 127）。

父进程：使用 wait() 阻塞等待。

如果子进程正常退出，打印：“任务完成，返回值：X”。

如果子进程异常退出（exec 失败），打印：“任务失败，错误码：X”。

禁止使用 system() 函数！
*/
void testProgram()
{
    fprintf(stdout, "魔王shell >,此时pid为 %d\n", getpid());
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork err");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        fprintf(stdout, "此时pid为 %d, 父pid为 %d\n", getpid(), getppid());
        // execlp("ps", "ps", "-ef", NULL);
        // 构造命令异常场景
        execlp("k", "a", "b", NULL);
        perror("execlp err");
        exit(127);
    }
    int status;
    while (waitpid(pid, &status,WNOHANG) == 0) {
        fprintf(stdout, "此时我是父pid %d, 我在等我的儿子\n", getpid());
        sleep(1);
    }
    if (WIFEXITED(status)) {
        fprintf(stdout, "任务完成，返回值：%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        fprintf(stdout, "任务失败，错误码：%d\n", WTERMSIG(status));
    }
    return;
}