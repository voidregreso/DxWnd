#define  _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"

#define MAX_HEXDUMP 64
#define DEFAULT_HEXDUMP 16

char *hexdump(unsigned char *Buffer, int len) {
    static char sHexBuffer[3 * MAX_HEXDUMP + 12];
    char *s;
    int iMaxI;
    if(!Buffer) return "(NULL)";
    s = sHexBuffer;
    iMaxI = len;
    if(iMaxI > MAX_HEXDUMP) iMaxI = MAX_HEXDUMP;
    for (int i = 0; i < iMaxI; i++) {
        sprintf(s, "%02.2X,", Buffer[i]);
        s += 3;
    }
    *(--s) = 0; // eliminate last comma
    if(len > iMaxI) strcpy(s, ",...");
    return sHexBuffer;
}

char *asciidump(unsigned char *Buffer, int len) {
    static char sAsciiBuffer[MAX_HEXDUMP + 12];
    char *s;
    if(!Buffer) return "(NULL)";
    strcpy(sAsciiBuffer, " | ");
    s = &sAsciiBuffer[3];
    for (int i = 0; i < len; i++) {
        char c = Buffer[i];
        *s++ = isprint(c) ? c : '.';
    }
    *s = 0; // add string terminator
    return sAsciiBuffer;
}

void hexdumpfile(char *path, int columns, int asciiflag) {
    char buf[32];
    int seek = 0;
    int len;
    printf("--------\n");
    printf("hexdump: %s\n", path);
    printf("--------\n");
    FILE *f = fopen(path, "rb");
    if(f == NULL) return;
    while(!feof(f)) {
        len = fread(buf, 1, columns, f);
        printf("%06.6X: %s%s\n", seek, hexdump(buf, len), asciiflag ? asciidump(buf, len) : "");
        seek += columns;
    }
    printf("--------\n");
}

int main(int argc, char *argv[]) {
    int columns = DEFAULT_HEXDUMP;
    int asciiflag = 0;
    if(argc == 1) {
        printf("hdump: (c)gho 2018\n"
               "syntax: hdump [-<n>] paths ...\n"
               "\t-n: print n hex columns (default %d, up to %d maximum)\n"
               "\t+ : print also ascii text (. for unprintable chars)\n",
               DEFAULT_HEXDUMP, MAX_HEXDUMP);
        exit(0);
    }
    for (int i = 1; i < argc; i++) {
        char tag = argv[i][0];
        if(tag == '-') {
            columns = atoi(&argv[i][1]);
            if(columns > MAX_HEXDUMP) columns = MAX_HEXDUMP;
            if(columns < 0) columns = DEFAULT_HEXDUMP;
        } else if(tag == '+') asciiflag = 1;
        else
            hexdumpfile(argv[i], columns, asciiflag);
    }
}