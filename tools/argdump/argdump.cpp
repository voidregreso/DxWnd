// argdump.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"


int _tmain(int argc, _TCHAR *argv[]) {
    FILE *log;
    log = fopen("argdump.txt", "a");
    for(int i = 0; i < argc; i++) fprintf(log, "ARG[%d]=\"%ls\"\n", i, argv[i]);
    fclose(log);
    return 0;
}

