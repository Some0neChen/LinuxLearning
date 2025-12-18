#include <string.h>

#include "logApi.h"

int main()
{
    FileOperDataE data = { 0 };
    // 测试基础输入输出fputc、fgetc、fputs、fgets
    // data.fileName = "./LogFile/loginfo_20251217.txt";
    // 测试文件IO写入写入单个字符
    // data.content  = "i love you.\n";
    // data.mode     = FILE_IO_MODE_INPUT_CHAR;
    
    // 测试文件IO写入字符串
    // data.content    = "The poem of summer.\n";
    // data.mode       = FILE_IO_MODE_INPUT_STRING;

    // 测试挨个读字符
    // data.bufferSize = 1024;
    // data.buffer = (char*)malloc(sizeof(char) * data.bufferSize);
    // memset(data.buffer, 0, sizeof(char) * data.bufferSize);
    // data.mode = FILE_IO_MODE_READ_CHAR;

    // 测试清空写入字符串
    data.fileName   = "./LogFile/LoveLetter.txt";
    data.mode       = FILE_IO_MODE_COVER_WRITE;
    data.content    = "I love you in all my life.\n";
    logModuleExecute(&data);

    // 测试修改写入
    data.mode       = FILE_IO_MODE_ALTER_WRITE;
    data.offset     = 10;
    data.content    = " forever, no mater time.\n";
    logModuleExecute(&data);

    // 测试直接读全部字符串
    data.bufferSize = 1024;
    data.buffer = (char*)malloc(sizeof(char) * data.bufferSize);
    memset(data.buffer, 0, sizeof(char) * data.bufferSize);
    data.mode = FILE_IO_MODE_READ_STRING;

    logModuleExecute(&data);

    printf("The reading text is:\n");
    printf("%s", data.buffer);

    // 测试格式化IO
    // data.fileName = "./LogFile/GirlsInfo.txt";
    // data.girlNums = 3;
    // data.girls = malloc(sizeof(JK_Girl) * data.girlNums);
    // memset(data.girls, 0, sizeof(JK_Girl) * data.girlNums);

    // 测试格式化输入
    // data.girls[0].id = 1;
    // data.girls[1].id = 2;
    // data.girls[2].id = 3;
    // strcpy(data.girls[0].name, "Alice");
    // strcpy(data.girls[1].name, "Elizebath");
    // strcpy(data.girls[2].name, "Kiriko");
    // data.girls[0].sensitive_val = 17;
    // data.girls[1].sensitive_val = 18;
    // data.girls[2].sensitive_val = 17;
    // data.mode = FILE_IO_MODE_INPUT_FORMAT;
    //测试二进制数据写入
    // data.fileName = "./LogFile/GirlsInfo.data";
    // data.mode = FILE_IO_MODE_INPUT_BINARY;

    // 测试格式化读取数据到结构体中
    // data.mode = FILE_IO_MODE_READ_FORMAT;
    // 测试二进制读取数据
    // data.mode = FILE_IO_MODE_READ_BINARY;



    // logModuleExecute(&data);
    // for (int i = 0; i < data.girlNums; ++i) {
    //     printf("[INFO][Girl] id:%d name:%s snesitive:%d.\n",
    //         data.girls[i].id, data.girls[i].name, data.girls[i].sensitive_val);
    // }

    FileOperDataFree(&data);
    return 0;
}