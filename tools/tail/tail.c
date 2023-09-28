#define  _CRT_SECURE_NO_WARNINGS
#include "stdio.h"
#include "stdlib.h"

extern void sleep(int);

void tailfile(char *path, int percent, int linecount, int follow, int wait) {
    char line[1024 + 1];
    long len;
    FILE *f;
    do {
        f = fopen(path, "rb");
        if(f != NULL) break;
        if(wait) sleep(1);
    } while(wait);
    if(f == NULL) return;
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    //printf("len=%ld percent=%d seek=%d\n", len, percent,  (len / 100) * percent);
    fseek(f, (len / 100) * percent, SEEK_SET);
    while(fgetc(f) != '\n') if(feof(f)) break;
    do {
        while(!feof(f)) {
            if(!fgets(line, 1024, f)) break;
            fputs(line, stdout);
            if(linecount) {
                linecount --;
                if(!linecount) break;
            }
        }
        if (!follow) break;
        len = ftell(f);
        f = freopen(path, "rb", f);
        extern void usleep(int);
        usleep(100000); // 100 mSec
        fseek(f, 0, SEEK_END);
        if(ftell(f) < len) {
            // reset
            printf("\n\n*******************************************\n\n");
            fseek(f, 0, SEEK_SET); // reposition or reset
        } else {
            // continue
            fseek(f, len, SEEK_SET);
        }
        clearerr(f);
    } while (1);
    fclose(f);
}

int main(int argc, char *argv[]) {
    int percent = 90;
    int linecount = 0;
    int follow = 0;
    int wait = 0;
    if(argc == 1) {
        printf("tail: (c)gho 2018-2019\n"
               "syntax: tail [-<n>] [+<n>] paths ...\n"
               "\t-n: start dump at n%% of file size (default 90%)\n"
               "\t+n: print up to n text lines\n"
               "\t/w: wait for file creation\n"
               "\t/f: wait for following lines\n"
              );
        exit(0);
    }
    for (int i = 1; i < argc; i++) {
        char tag = argv[i][0];
        if(tag == '-')
            percent = atoi(&argv[i][1]);
        else if(tag == '+')
            linecount = atoi(&argv[i][1]);
        if(tag == '/') {
            switch(argv[i][1]) {
            case 'f':
                follow = 1;
                break;
            case 'w':
                wait = 1;
                break;
            }
        } else
            tailfile(argv[i], percent, linecount, follow, wait);
    }
}