#ifndef __LOGAPI_H__
#define __LOGAPI_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int UINT32;

typedef struct FileOperData FileOperDataE;
typedef void (*fileIoOperFunc)(FileOperDataE*);
typedef void (*getFileStreamPtrFunc)(FileOperDataE*);

// 定义学妹的身体构造 (结构体)
typedef struct  {
    int id;             // 学号
    char name[20];      // 名字
    int sensitive_val;  // 敏感度 (私密数据)
} JK_Girl;

typedef enum FILE_IO_MODE {
    FILE_IO_MODE_INPUT_CHAR = 0,    // 写字符
    FILE_IO_MODE_READ_CHAR,         // 读字符
    FILE_IO_MODE_COVER_WRITE,       // 覆盖写入
    FILE_IO_MODE_ALTER_WRITE,       // 修改写入
    FILE_IO_MODE_INPUT_STRING,      // 写字符串
    FILE_IO_MODE_READ_STRING,       // 读字符串
    FILE_IO_MODE_INPUT_FORMAT,      // 格式化写入
    FILE_IO_MODE_READ_FORMAT,       // 格式化读出
    FILE_IO_MODE_INPUT_BINARY,      // 二进制写入
    FILE_IO_MODE_READ_BINARY,       // 二进制读出
    FILE_IO_MODE_TYPE_MAX
} FILE_IO_MODE_E;

struct FileOperData {       // 文件IO操作结构体
    FILE* fileStream;       // 文件流指针
    const char* fileName;   // 所要操作文件的路径
    const char* content;    // 写文件字符串时要写入的内容
    char* buffer;           // 读文件时用来保存读取读到数据
    UINT32 bufferSize;      // 读取缓冲区大小
    FILE_IO_MODE_E mode;    // 操作模式
    long offset;             // 修改写入时所要开始覆盖写入的位置
    UINT32 girlNums;        // 结构体数量
    JK_Girl* girls;         // 要写入的结构体
};

typedef struct {
    fileIoOperFunc fileStreamGetByModeFunc;
    fileIoOperFunc fileOperGetByModeFunc;
} ModeFuncPair;

void logModuleExecute(FileOperDataE* data);
void FileOperDataFree(FileOperDataE* data);

#endif