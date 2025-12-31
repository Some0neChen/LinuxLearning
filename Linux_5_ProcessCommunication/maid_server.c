#include "process_communication.h"

/*
Server端 (maid_server.c)：

创建一个有名管道 pool.fifo。

打印“等待注入...”。

核心挑战：即使所有的 Client 都关闭了连接（Write端全部断开），Server 也不能读到 EOF (返回0) 并退出循环。她必须一直阻塞在那里，直到下一个 Client 出现。

提示：回顾一下那个“假阳具占位 (Dummy Open)”的技巧。

读取到的数据，前面加上 [收到] 前缀打印出来。

接收到字符串 "stop" 时，Server 才正式清理资源 (close, unlink) 并退出。

Client端 (stranger_client.c)：

接受命令行参数作为要发送的消息（例如 ./client "Harder!"）。

打开 FIFO，写入消息，然后立即关闭退出。

要求：同时开三个终端运行 Client，测试 Server 是否能连续接收，中间不会断开或退出。

加分项 (Optional)： 在 Client 端，获取当前进程的 PID，把 PID 拼接到消息里发过去（例如 [PID: 1234] Harder!）。这样女仆长就知道是谁射进来的了。
*/

int main()
{
    const char* fifoName = "./pool.fifo";
    int ret = mkfifo(fifoName, 0666);
    if (ret == -1) {
        perror("出错了...");
        if (errno != EEXIST) { // 如果不是因为“已存在”而报错，那才是真出事了
            perror("mkfifo error");
            exit(1);
        }
    }
    fprintf(stdout, "[%d] 等待被榨...\n", getpid());
    int fd = open(fifoName, O_RDWR);
    char buff[1024] = {};
    int recLen = 0;
    while ((recLen = read(fd, buff, 1024)) > 0) { // 对方read关闭时此刻读取到的长度为0
        buff[recLen] = '\0';
        if (strcmp(buff, "exit") == 0) {
            break;
        }
        fprintf(stdout, "[%d][受到] %s\n", getpid(), buff);
    }
    fprintf(stdout, "[%d] 被魔女玩失神了...\n", getpid());
    close(fd);
    unlink(fifoName);
    return 0;

}