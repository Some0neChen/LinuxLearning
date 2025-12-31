#ifndef __PROCESS_COMMUNICATION_H__
#define __PROCESS_COMMUNICATION_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

void PsAuxCliFilter(const char* word);

#endif