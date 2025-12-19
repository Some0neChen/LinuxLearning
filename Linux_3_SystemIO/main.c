#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>   // 召唤咒语 open
#include <unistd.h>  // 动作咒语 write, read, lseek, close

// 魅魔的身体构造
typedef struct {
    int id;             // 编号 (4 bytes)
    char name[20];      // 真名 (20 bytes)
    int loyalty;        // 忠诚度 (4 bytes) -> 这是我们要修改的敏感点！
} SuccubusBody;

int main()
{
    int SuccubusNums = 3;
    SuccubusBody sexyBeauty[3];
    for (int i = 0; i < SuccubusNums; i++)
    {
        fprintf(stdout, "\nPlease input num %d succubus info(id, name, loyalty): ", i + 1);
        fscanf(stdin, "%d %s %d", &sexyBeauty[i].id, sexyBeauty[i].name, &sexyBeauty[i].loyalty);
    }

    const char* fileName = "./succubus.dat";
    int fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (fd == -1) {         // 0代表ok，非0代表一场
        perror("open"); // 打印错误信息
        exit(EXIT_FAILURE);    // 退出程序，若系统调用_exit()函数，则会不清理环境
    }
    write(fd, sexyBeauty, sizeof(sexyBeauty));

    int loyaltyArr[3];
    for (int i = 0; i < SuccubusNums; i++)
    {
        fprintf(stdout, "\nPlease input num %d succubus loyalty: ", i + 1);
        fscanf(stdin, "%d", &loyaltyArr[i]);
    }

    lseek(fd, sizeof(int) + sizeof(char) * 20, SEEK_SET); // 这里要重新把指针指向第一个再去改
    for (int i = 0; i < SuccubusNums; ++i) {
        write(fd, &loyaltyArr[i], sizeof(int));
        lseek(fd, sizeof(SuccubusBody), SEEK_CUR);
    }

    // 这里要重新把指针指向第一个再去读
    lseek(fd, 0, SEEK_SET);
    read(fd, sexyBeauty, sizeof(sexyBeauty));
    fputs("\nEnjoy the succubus after your hard work!", stderr);
    for (int i = 0; i < SuccubusNums; i++)
    {
        fprintf(stdout, "\nSuccubus id[%d] name[%s] loyalty[%d]", sexyBeauty[i].id, sexyBeauty[i].name, sexyBeauty[i].loyalty);
    }
    
    close(fd);
    return 0;
}