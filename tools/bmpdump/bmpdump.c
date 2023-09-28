#define  _CRT_SECURE_NO_WARNINGS
#include "windows.h"
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

static char *sCompression(DWORD c) {
    static char *cs[] = {
        "BI_RGB", "BI_RLE8", "BI_RLE4", "BI_BITFIELDS", "BI_JPEG", "BI_PNG", "other"
    };
    if(c > BI_PNG) return cs[BI_PNG + 1];
    return cs[c];
}

#define columns 16
#define asciiflag FALSE

struct {
    DWORD        bV4RedMask;
    DWORD        bV4GreenMask;
    DWORD        bV4BlueMask;
    DWORD        bV4AlphaMask;
} v4extra;

void bmpdump(char *path) {
    BITMAPFILEHEADER fileheader;
    BITMAPINFOHEADER ih;
    LPBITMAPINFOHEADER lpbi;
    char buf[32];
    int seek = 0;
    int len;
    int toread;
    printf("--------\n");
    printf("bmpdump: %s\n", path);
    printf("--------\n");
    FILE *f = fopen(path, "rb");
    if(f == NULL) return;
    len = fread(&fileheader, sizeof(BITMAPFILEHEADER), 1, f);
    if(len != 1) {
        printf("!! missing file header\n");
        return;
    }
    if(strncmp((char *)&fileheader, "BM", 2)) {
        printf("!! bad file header - no BM\n");
        return;
    }
    printf("size=%d\n", fileheader.bfSize);
    printf("reserved=%d~%d\n", fileheader.bfReserved1, fileheader.bfReserved2);
    printf("offbits=%d\n", fileheader.bfOffBits);
    fseek(f, 0x0E, SEEK_SET); // -- INFOHEADER
    len = fread(&ih, sizeof(BITMAPINFOHEADER), 1, f);
    if(len != 1) {
        printf("!! missing info header\n");
        return;
    }
    printf("hdr size=%d\n", ih.biSize);
    printf("img size (W x H)=(%d x %d)\n", ih.biWidth, ih.biHeight);
    printf("planes=%d\n", ih.biPlanes);
    printf("bpp=%d\n", ih.biBitCount);
    printf("compression=%#x(%s)\n", ih.biCompression, sCompression(ih.biCompression));
    if ((ih.biCompression == BI_BITFIELDS) &&
            !((ih.biBitCount == 16) || (ih.biBitCount == 32)))
        printf("!! BI_BITFIELDS compression requires 16 or 32 bpp color depth\n");
    printf("imagesize=%d\n", ih.biSizeImage);
    printf("resolution (X x Y)=(%d x %d)\n", ih.biXPelsPerMeter, ih.biYPelsPerMeter);
    printf("ncolours=%d\n", ih.biClrUsed);
    printf("important colours=%d\n", ih.biClrImportant);
    if (ih.biCompression == BI_BITFIELDS) {
        len = fread(&v4extra, sizeof(v4extra), 1, f);
        if(len != 1) {
            printf("!! missing bitfields info\n");
            return;
        }
        printf("R mask=%#x\n", v4extra.bV4RedMask);
        printf("G mask=%#x\n", v4extra.bV4GreenMask);
        printf("B mask=%#x\n", v4extra.bV4BlueMask);
        printf("A mask=%#x\n", v4extra.bV4AlphaMask);
    }
    if(ih.biBitCount > 8)
        printf("no palette\n");
    else {
        if(ih.biClrUsed) toread = 4 * ih.biClrUsed;
        else toread = 0x1 << ih.biBitCount;
        fseek(f, sizeof(BITMAPFILEHEADER) + ih.biSize, SEEK_SET);
        while(toread) {
            int nextread = toread;
            if(nextread > columns) nextread = columns;
            len = fread(buf, 1, nextread, f);
            printf("%06.6X: %s%s\n", seek, hexdump(buf, len), asciiflag ? asciidump(buf, len) : "");
            if(len < nextread) {
                printf("!! partial palette block\n");
                return;
            }
            seek += len;
            toread -= len;
        }
    }
    printf("--------\n");
}

int main(int argc, char *argv[]) {
    char *bmpfile;
    if(argc == 1) {
        printf("bmpdump: (c)gho 2019\n"
               "syntax: bmpdump paths ...\n");
        exit(0);
    }
    for (int i = 1; i < argc; i++) {
        bmpfile = argv[i];
        bmpdump(bmpfile);
    }
}