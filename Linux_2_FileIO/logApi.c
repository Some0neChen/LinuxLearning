#include "logApi.h"
#include <stdio.h>
#include <string.h>

/*
 * 函数功能：返回传入文件名的文件指针（追加写入）
 * 行为逻辑：文件不存在则创建文件，文件存在则变为追写模式
 */
void getWriteFilePtr(FileOperDataE* data)
{
    const char* func_name = "getWriteFilePtr";
    printf("[INFO][%s] enter.\n", func_name);

    data->fileStream =  fopen(data->fileName, "a"); // "a" : 追写模式
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get filename “%s” fileStream failed.\n", func_name, data->fileName);
    }
    return;
}

/*
 * 函数功能：返回传入文件名的文件指针（读取）
 * 行为逻辑：取到和不到都直接返回
 */
void getReadFilePtr(FileOperDataE* data)
{
    const char* func_name = "getReadFilePtr";
    printf("[INFO][%s] enter.\n", func_name);

    // FILE *fopen(const char *filename, const char *mode);
    FILE* fileStream = fopen(data->fileName, "r");    // "r" : 只读模式
    data->fileStream = fileStream;
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get filename “%s” fileStream failed.\n", func_name, data->fileName);
    }
    return;
}

/*
 * 函数功能：返回传入文件名的文件指针（覆盖写入）
 * 行为逻辑：文件不存在则创建文件，文件存在则销毁再写入
 */
void getCoverWriteFilePtr(FileOperDataE* data)
{
    const char* func_name = "getCoverWriteFilePtr";
    printf("[INFO][%s] enter.\n", func_name);

    data->fileStream =  fopen(data->fileName, "w"); // "w" : 销毁写入
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get filename “%s” fileStream failed.\n", func_name, data->fileName);
    }
    return;
}

/*
 * 函数功能：返回传入文件名的文件指针（修改写入）
 * 行为逻辑：文件不存在则创建文件，文件存在则销毁再写入
 */
void getAlterWriteFilePtr(FileOperDataE* data)
{
    const char* func_name = "getAlterWriteFilePtr";
    printf("[INFO][%s] enter.\n", func_name);

    data->fileStream =  fopen(data->fileName, "r+"); // "w" : 修改读写
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get filename “%s” fileStream failed.\n", func_name, data->fileName);
    }
    return;
}

/*
 * 函数功能：返回二进制写入指针
 * 行为逻辑：取到和不到都直接返回
 */
void getBinaryWriteFilePtr(FileOperDataE* data)
{
    const char* func_name = "getBinaryWriteFilePtr";
    printf("[INFO][%s] enter.\n", func_name);

    // FILE *fopen(const char *filename, const char *mode);
    // "wb" : 二进制写入模式，文件不存在则创建文件，文件存在则销毁再创建文件
    FILE* fileStream = fopen(data->fileName, "wb");
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get filename “%s” fileStream failed.\n", func_name, data->fileName);
    }
    data->fileStream = fileStream;
}

/*
 * 函数功能：返回二进制读出指针(尝试使用rb+)
 * 行为逻辑：取到和不到都直接返回
 */
void getBinaryReadFilePtr(FileOperDataE* data)
{
    const char* func_name = "getBinaryReadFilePtr";
    printf("[INFO][%s] enter.\n", func_name);

    // FILE *fopen(const char *filename, const char *mode);
    // "rb+" : 二进制读写模式，文件不存在则创建文件，文件存在则销毁再创建文件
    FILE* fileStream = fopen(data->fileName, "rb+");
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get filename “%s” fileStream failed.\n", func_name, data->fileName);
    }
    data->fileStream = fileStream;
}

/*
 * 函数功能：清空环境变量、释放内存空间
 * 行为逻辑：将文件指针关闭及置空，将数据的字符串内存释放及置空，字符串大小置0
 */
void FileOperDataFree(FileOperDataE* data)
{
    if (data->fileStream != NULL) {
        fclose(data->fileStream);
        data->fileStream = NULL;
    }
    if (data->buffer != NULL) {
        free(data->buffer);
        data->buffer = NULL;
        data->bufferSize = 0;
    }
    if (data->girls != NULL) {
        free(data->girls);
        data->girls = NULL;
        data->girlNums = 0;
    }
    return;
}

/*
 * 函数功能：用传入的字符串，挨个读入写到文件中
 * 行为逻辑：
 */
void writeCharInfoToFile(FileOperDataE* data)
{
    const char* func_name = "writeCharInfoToFile";
    printf("[INFO][%s] enter.\n", func_name);
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get fileStream failed.\n", func_name);
        return;
    }

    unsigned int size = strlen(data->content);
    printf("[INFO][%s] get content size %d.\n", func_name, size);
    int ret = 0;
    for (int i = 0; i < size; ++i) {
        ret = fputc(data->content[i], data->fileStream);
        if (ret == EOF) {
            printf("[ERR][%s] putc index[%d]: %c failed.\n", func_name, i, data->content[i]);
            break;
        }
        if (i % 4 == 0) {
            fflush(data->fileStream);
        }
    }
    return;
}

/*
 * 函数功能：用传入的字符串，直接写到文件中
 * 行为逻辑：
 */
void writeStringInfoToFile(FileOperDataE* data)
{
    const char* func_name = "writeStringInfoToFile";
    printf("[INFO][%s] enter.\n", func_name);
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get fileStream failed.\n", func_name);
        return;
    }

    unsigned int size = strlen(data->content);
    printf("[INFO][%s] get content size %d.\n", func_name, size);
    int ret = fputs(data->content, data->fileStream);
    printf("[INFO][%s] write string ret %d.\n", func_name, ret);
    fflush(data->fileStream);
    return;
}

/*
 * 函数功能：用传入的字符串，根据偏移量覆盖写入到文件中
 * 行为逻辑：如果传入的offset超出文本原有数据大小，则直接赋空值，并到了偏移量再写入
 *          但如此一来后续再读文本时，从头开始读读到NULL就返回EOF表示读完了，不会读出后续的数据
 */
void coverWriteStringInfoToFile(FileOperDataE* data)
{
    const char* func_name = "writeStringInfoToFile";
    printf("[INFO][%s] enter.\n", func_name);
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get fileStream failed.\n", func_name);
        return;
    }

    unsigned int size = strlen(data->content);
    printf("[INFO][%s] get content size %d.\n", func_name, size);
    // 把指针指向文件的某处地方
    // SEEK_SET从头 SEEK_CUR当前 SEEK_END往后
    int ret = fseek(data->fileStream, data->offset, SEEK_SET);
    printf("[INFO][%s] move fileStream ptr ret %d.\n", func_name, ret);
    fputs(data->content, data->fileStream);
    printf("[INFO][%s] write string ret %d.\n", func_name, ret);
    fflush(data->fileStream);
    return;
}

/*
 * 函数功能：挨个字符取回本地文件中的字符
 * 行为逻辑：
 */
void readCharInfoFromFile(FileOperDataE* data)
{
    const char* func_name = "readCharInfoFromFile";
    printf("[INFO][%s] enter.\n", func_name);
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get fileStream failed.\n", func_name);
        return;
    }
    if (data->bufferSize == 0) {
        printf("[ERR][%s] int inputing buffSize is zero.\n", func_name);
        return;
    }

    int ret = 0;
    UINT32 index = 0;
    while ((ret = fgetc(data->fileStream)) != EOF && ((index++) < data->bufferSize)) {
        data->buffer[index - 1] = ret;
    }
    if (ret == EOF) {
        printf("[INFO][%s] read index[%d] EOF.\n", func_name, index);
    }
    data->buffer[index] = '\0';
    return;
}

/*
 * 函数功能：读取文件中的全部内容
 * 行为逻辑：一下吞一行，遇到结尾或'\n'时停止
 *          当完全读完时，返回NULL（即EOF）。可用这个特点来便利循环
 */
void readStringInfoFromFile(FileOperDataE* data)
{
    const char* func_name = "readStringInfoFromFile";
    printf("[INFO][%s] enter.\n", func_name);
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get fileStream failed.\n", func_name);
        return;
    }
    if (data->bufferSize == 0) {
        printf("[ERR][%s] int inputing buffSize is zero.\n", func_name);
        return;
    }

    char* ret;
    int line = 0;
    char* buffPtr = data->buffer;
    UINT32 buffSize = data->bufferSize;
    printf("[INFO][%s] get text %d.\n", func_name, line);
    printf("==========================================\n");
    while ((ret = fgets(buffPtr, buffSize, data->fileStream)) != NULL) {
        printf("Line[%2d]\t:%s", line, buffPtr);
        if (strlen(buffPtr) > buffSize) {
            return;
        }
        buffSize -= strlen(buffPtr);
        buffPtr += strlen(buffPtr);
        ++line;
    }
    printf("==========================================\n");
    printf("[INFO][%s] fgets total line %d.\n", func_name, line);
    return;
}

/*
 * 函数功能：把结构体中的内容格式化写到文件中
 * 行为逻辑：执行一次就清空缓存进文件中保存
 */
void writeFormatStringInfoIntoFile(FileOperDataE* data)
{
    const char* func_name = "writeFormatStringInfoIntoFile";
    printf("[INFO][%s] enter.\n", func_name);
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get fileStream failed.\n", func_name);
        return;
    }
    if (data->girlNums == 0) {
        printf("[ERR][%s] int inputing girl nums is zero.\n", func_name);
        return;
    }
    int ret = 0;
    for (int i = 0; i < data->girlNums; ++i) {
        ret = fprintf(data->fileStream, "[Girl_Card] Index[%d] name:[%12s] sensitive[%2d]\n",
            data->girls[i].id, data->girls[i].name, data->girls[i].sensitive_val);
        fflush(data->fileStream);
        printf("[INFO][%s] write girl[%d] ret %d.\n", func_name, i, ret);
    }
    return;
}

/*
 * 函数功能：把文件中的内容通过格式化写到写到结构体中
 * 行为逻辑：
 */
void readFormatStringInfoFromFile(FileOperDataE* data)
{
    const char* func_name = "readFormatStringInfoFromFile";
    printf("[INFO][%s] enter.\n", func_name);
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get fileStream failed.\n", func_name);
        return;
    }
    if (data->girlNums == 0) {
        printf("[ERR][%s] int inputing girl nums is zero.\n", func_name);
        return;
    }
    int ret = 0;
    for (int i = 0; i < data->girlNums; ++i) {
        // [ %[^]]]表示在[]内，跳过前置空格，并一直读直到遇到]为止
        // 而%s的机制是一直取，直到遇到空格为止，这里如果用[%s]，会把]也吃进去，然后期待读到],但是后面是空格，直接就罢工了
        ret = fscanf(data->fileStream, "[Girl_Card] Index[%d] name:[ %[^]]] sensitive[%2d]\n",
            &data->girls[i].id, data->girls[i].name, &data->girls[i].sensitive_val); // 得用指针接受
        printf("[INFO][%s] read girl[%d] ret %d.\n", func_name, i, ret);
    }

    return;
}

void writeBianryInfoIntoFile(FileOperDataE* data)
{
    const char* func_name = "writeBianryInfoIntoFile";
    printf("[INFO][%s] enter.\n", func_name);
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get fileStream failed.\n", func_name);
        return;
    }
    if (data->girlNums == 0) {
        printf("[ERR][%s] int inputing girl nums is zero.\n", func_name);
        return;
    }

    // 这里第二个参数是要读的结构体大小，第三个参数代表要读几个
    int ret = fwrite(data->girls, sizeof(JK_Girl), data->girlNums, data->fileStream);
    // 返回值为写入的结构体数量，正常情况下就是第三个参数
    // 写入字节序
    printf("[INFO][%s] wirte girl binary data ret %d.\n", func_name, ret);
    return;
}

void readBinaryInfoFromFile(FileOperDataE* data)
{
    const char* func_name = "readBinaryInfoFromFile";
    printf("[INFO][%s] enter.\n", func_name);
    if (data->fileStream == NULL) {
        printf("[ERR][%s] get fileStream failed.\n", func_name);
        return;
    }
    if (data->girlNums == 0) {
        printf("[ERR][%s] int inputing girl nums is zero.\n", func_name);
        return;
    }

    UINT32 girlDataSize = sizeof(JK_Girl) * data->girlNums;
    int ret = fread(data->girls, girlDataSize, data->girlNums, data->fileStream);
    // 返回值为读取的结构体数量，正常情况下就是第三个参数
    printf("[INFO][%s] read girl binary data ret %d.\n", func_name, ret);
    return;
}

// 表驱动，第一个成员函数为获取FILE指针，第二个为对应模式的处理函数
ModeFuncPair ModeFUncMap[FILE_IO_MODE_TYPE_MAX] = {
    {getWriteFilePtr,       writeCharInfoToFile},
    {getReadFilePtr,        readCharInfoFromFile},
    {getCoverWriteFilePtr,  writeStringInfoToFile},
    {getAlterWriteFilePtr,  coverWriteStringInfoToFile},
    {getWriteFilePtr,       writeStringInfoToFile},
    {getReadFilePtr,        readStringInfoFromFile},
    {getWriteFilePtr,       writeFormatStringInfoIntoFile},
    {getReadFilePtr,        readFormatStringInfoFromFile},
    {getBinaryWriteFilePtr, writeBianryInfoIntoFile},
    {getBinaryReadFilePtr,  readBinaryInfoFromFile}
};

// 文件IO操作入口
void logModuleExecute(FileOperDataE* data)
{
    const char* func_name = "logModuleExecute";
    if (data->mode >= FILE_IO_MODE_TYPE_MAX) {
        printf("[ERR][%s] get mode[%u] exceed.\n", func_name, data->mode);
        return;
    }
    ModeFUncMap[data->mode].fileStreamGetByModeFunc(data);
    ModeFUncMap[data->mode].fileOperGetByModeFunc(data);
    return;
}