

// main.cpp

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdio.h>
#include <wingdi.h>
#include <gl\gl.h>

#define LOG(format, ...) wprintf(format L"\n", __VA_ARGS__)

void syntax() {
    printf("ogldump v1.0 (c)GHO 2019\n");
    printf("\nsyntax: ogldump [/e] [/i]\n");
}

void dump_extensions() {
    char *p = (char *)glGetString(GL_EXTENSIONS);
    char *space;
    char extension[81];
    int len;
    while(space = strchr(p, ' ')) {
        len = space - p;
        strncpy(extension, p, len);
        extension[len] = 0;
        printf("%s\n", extension);
        p += len + 1;
    }
    printf("%s\n", p);
    //printf("extensions:\n%s\n", glGetString(GL_EXTENSIONS));
}

void dump_info() {
    printf("vendor: %s\n", glGetString(GL_VENDOR));
    printf("renderer: %s\n", glGetString(GL_RENDERER));
    printf("version: %s\n", glGetString(GL_VERSION));
}

int main(int argc, char *argv[]) {
    HDC hdc;
    int  iPixelFormat;
    if(argc < 2) {
        syntax();
        return 0;
    }
    hdc = GetDC(GetDesktopWindow());
    if(!hdc) {
        printf("GetDC error %d\n", GetLastError());
        return -1;
    }
    //if(!SetPixelFormat(dc, 0, NULL)) {
    //	printf("SetPixelFormat error %d\n", GetLastError());
    //	return -1;
    //}
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd
        1,                     // version number
        PFD_DRAW_TO_WINDOW |   // support window
        PFD_SUPPORT_OPENGL |   // support OpenGL
        PFD_DOUBLEBUFFER,      // double buffered
        PFD_TYPE_RGBA,         // RGBA type
        24,                    // 24-bit color depth
        0, 0, 0, 0, 0, 0,      // color bits ignored
        0,                     // no alpha buffer
        0,                     // shift bit ignored
        0,                     // no accumulation buffer
        0, 0, 0, 0,            // accum bits ignored
        32,                    // 32-bit z-buffer
        0,                     // no stencil buffer
        0,                     // no auxiliary buffer
        PFD_MAIN_PLANE,        // main layer
        0,                     // reserved
        0, 0, 0                // layer masks ignored
    };
    // get the best available match of pixel format for the device context
    iPixelFormat = ChoosePixelFormat(hdc, &pfd);
    // make that the pixel format of the device context
    if (!SetPixelFormat(hdc, iPixelFormat, &pfd)) {
        printf("SetPixelFormat error %d\n", GetLastError());
        return -1;
    }
    HGLRC hglrc = wglCreateContext(hdc);
    if(!hglrc) {
        printf("wglCreateContext error %d\n", GetLastError());
        return -1;
    }
    if (!wglMakeCurrent(hdc, hglrc)) {
        printf("wglMakeCurrent error %d\n", GetLastError());
        return -1;
    }
    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "/e")) dump_extensions();
        if(!strcmp(argv[i], "/i")) dump_info();
    }
    getchar();
    return 0;
}

