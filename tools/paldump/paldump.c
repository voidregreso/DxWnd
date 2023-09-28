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

typedef struct {
    unsigned int size;               /* Header size in bytes      */
    int width, height;               /* Width and height of image */
    unsigned short int planes;       /* Number of colour planes   */
    unsigned short int bits;         /* Bits per pixel            */
    unsigned int compression;        /* Compression type          */
    unsigned int imagesize;          /* Image size in bytes       */
    int xresolution, yresolution;    /* Pixels per meter          */
    unsigned int ncolours;           /* Number of colours         */
    unsigned int importantcolours;   /* Important colours         */
} INFOHEADER;

void palettedump(char *path, int columns, int asciiflag) {
    char buf[32];
    int seek = 0;
    int len;
    int toread;
    INFOHEADER ih;
    printf("--------\n");
    printf("paldump: %s\n", path);
    printf("--------\n");
    FILE *f = fopen(path, "rb");
    if(f == NULL) return;
    char fileheader[14];
    fread(fileheader, 1, 14, f);
    if(strncmp(fileheader, "BM", 2)) {
        printf("bad file hgeader - no BM\n");
        return;
    }
    fseek(f, 0x0E, SEEK_SET); // -- INFOHEADER
    fread(&ih, 1, sizeof(INFOHEADER), f);
    printf("hdr size=%d img size=%dx%d bpp=%d ncolours=%d\n",
           ih.size,
           ih.width, ih.height,
           ih.bits,
           ih.ncolours);
    if(ih.bits > 8) {
        printf("no palette\n");
        return;
    }
    toread = 4 * ih.ncolours;
    fseek(f, 0x0E + ih.size, SEEK_SET);
    while(toread) {
        int nextread = toread;
        if(nextread > columns) nextread = columns;
        len = fread(buf, 1, nextread, f);
        printf("%06.6X: %s%s\n", seek, hexdump(buf, len), asciiflag ? asciidump(buf, len) : "");
        seek += len;
        toread -= len;
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
            palettedump(argv[i], columns, asciiflag);
    }
}