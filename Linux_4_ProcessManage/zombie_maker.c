#include "program_test.h"

/*
🧟 试炼二：制造“活死人” (zombie_maker.c)
目标：亲手制造一个僵尸进程，并用系统命令验证它。

要求：

fork() 一个子进程。

子进程：打印“我是子进程，我先走了”，然后立刻 exit(0)。

父进程：打印“我是父进程，我要睡 20 秒，不给儿子收尸”。

调用 sleep(20)。

验证步骤：

在程序运行的这 20 秒内，去另一个终端输入 ps -ef | grep zombie_maker。

预期结果：你应该能看到子进程后面有一个 <defunct> 或 Z 标记。

20 秒后父进程退出，观察僵尸进程是否消失。
*/
void testForkAndWait()
{
    fprintf(stdout, "Fork begin.Right now pid is %d.\n", getpid());
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork err");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        fprintf(stdout, "我是子进程 %d[from %d], 睡个5秒我就先走了\n", getpid(), getppid());
        sleep(5);
        exit(EXIT_SUCCESS);
    } else {
        fprintf(stdout, "我是父进程 %d, 我要睡 20 秒, 醒了再说吧\n", getppid());
        sleep(20);
        fprintf(stdout, "我是父进程 %d, 睡够了, 干活！\n", getppid());
        waitpid(pid, NULL, 0);
    }
    return;
}