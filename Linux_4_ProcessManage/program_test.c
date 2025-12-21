#include "program_test.h"

/*
 *  函数教学:初识进程
 *          main函数中执行该函数，可通过ps -ef查看打印的pid及ppid，查看在系统中的调用
 */
void testProgramPidPpid()
{
    fprintf(stdout, "Well, I'm comming! My pid is %d, and my father pid is %d\n",
        getpid(), getppid());
    fprintf(stdout, "I'm Living! Please use 'ps -ef | grep main' search for me!\n");

    while (1) {
        sleep(1); // 这里不加sleep，执行程序后查看top，可看到程序直接把cpu拉满到百分百了
    }
}

void testForkProgram()
{
    fprintf(stdout, "Fork begin.\n");
    int cumNums = 100;
    fprintf(stdout, "CumNums Value is %d, addr is [%u]\n", cumNums, &cumNums);
    int ret = fork();
    if (ret == 0) {
        cumNums = 80;
        fprintf(stdout, "Right now, I'm the new succbus.\n");
        fprintf(stdout, "As the new succbus, my pid is %d, the old succbus pid is %d\n",
            getpid(), getppid());
        fprintf(stdout, "New succbus, get CumNums Value is %d, addr is [%u]\n",
            cumNums, &cumNums);
        while (1) {
            sleep(1);
        }
    }
    if (ret != 0) {
        fprintf(stdout, "Right now, I'm the master succbus.The return value is %d.\n", ret);
        fprintf(stdout, "As the master succbus, my pid is %d, the origin pid is %d.\n",
            getpid(), getppid());
        fprintf(stdout, "master succbus, get CumNums Value is %d, addr is [%u]\n",
            cumNums, &cumNums);
        while (1) {
            sleep(1);
        }
    }
}

void testExecAndWaitProgram()
{
    fprintf(stdout, "Fork begin.\n");
}