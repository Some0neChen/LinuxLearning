#include "process_communication.h"

int main(int argc, char* argv[])
{
    fprintf(stdout,"Happy my brithday! Hello, 25 years!\n");
    fprintf(stdout,"Hello 25, I am coming for the throne.\n");
    if (argc < 2) {
        fprintf(stdout,"Argv error.");
        return 0;
    }
    PsAuxCliFilter(argv[1]);
    return 0;
}