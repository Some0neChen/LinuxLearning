#ifndef __PROGRAM_TEST_H__
#define __PROGRAM_TEST_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void testProgramPidPpid();
void testForkProgram();
void testProgram();
void testForkAndWait();
void testForkAndUseFd();

#endif