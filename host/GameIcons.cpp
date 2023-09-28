#include "stdafx.h"
#include "GameIcons.h"
#include "Shlwapi.h"

//#define ICONSDEBUG

extern BOOL g32BitIcons;
extern void OutTrace(const char *, ...);

bool SaveIconToFile(HICON [], int, const TCHAR *);

void char2hex(unsigned char *src, char *dst, int len) {
    for (; len; len--) {
        sprintf(dst, "%02.2X", *src);
        src++;
        dst += 2;
    }
    *dst = 0; // terminator
}

int hex2char(unsigned char *dst, char *src) {
    char buf[3];
    int val;
    int len = strlen(src);
    int retlen = 0;
    buf[2] = 0; // string terminator
    for (; len; len -= 2, retlen++) {
        buf[0] = *src;
        buf[1] = *(src + 1);
        sscanf(buf, "%X", &val);
        *dst = (unsigned char)val;
        src += 2;
        dst ++;
    }
    return retlen;
}


#pragma pack(push)
#pragma pack(2)
typedef struct {
    BYTE        bWidth;          // Width, in pixels, of the image
    BYTE        bHeight;         // Height, in pixels, of the image
    BYTE        bColorCount;     // Number of colors in image (0 if >=8bpp)
    BYTE        bReserved;       // Reserved ( must be 0)
    WORD        wPlanes;         // Color Planes
    WORD        wBitCount;       // Bits per pixel
    DWORD       dwBytesInRes;    // How many bytes in this resource?
    DWORD       dwImageOffset;   // Where in the file is this image?
} ICONDIRENTRY, *LPICONDIRENTRY;

typedef struct {
    WORD           idReserved;   // Reserved (must be 0)
    WORD           idType;       // Resource Type (1 for icons)
    WORD           idCount;      // How many images?
} ICONDIR, *LPICONDIR;
#pragma pack(pop)

HICON CGameIcons::Extract(CString path) {
    HICON Icon;
    // v2.04.81: LoadLibrary fails when trying to load *.ico files, so do not return NULL on failure.
    HINSTANCE Hinst = ::LoadLibrary(path);
    if(Hinst) {
        Icon = ::ExtractIcon(Hinst, path.GetBuffer(), 0);
        ::FreeLibrary(Hinst); // v2.005.01 fixed
        if(Icon)
            return Icon;
    }
    // if not found ...
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    char SearchPath[MAX_PATH];
    strcpy(SearchPath, path);
    PathRemoveFileSpec(SearchPath);
    strcat(SearchPath, "\\*.ico");
    hFind = FindFirstFile(SearchPath, &FindFileData);
    if ((hFind != INVALID_HANDLE_VALUE) && (hFind != (HANDLE)ERROR_FILE_NOT_FOUND)) {
        strcpy(SearchPath, path);
        PathRemoveFileSpec(SearchPath);
        strcat(SearchPath, "\\");
        strcat(SearchPath, FindFileData.cFileName);
        Icon = ::ExtractIcon(NULL, SearchPath, 0);
        FindClose(hFind);
    } else
        Icon = (HICON)NULL;
    return Icon;
}

#define TMPICONPATH "extract.ico"
//#define TMPICONPATH "extract::$DATA"

static char *IconSerialize4Bit(HICON Icon) {
    // save icon to char buffer
    PICTDESC desc = { sizeof(PICTDESC)};
    desc.picType = PICTYPE_ICON;
    desc.icon.hicon = Icon;
    IPicture *pPicture = 0;
    HRESULT hr = OleCreatePictureIndirect(&desc, IID_IPicture, FALSE, (void **)&pPicture);
    if(FAILED(hr)) return NULL;
    IStream *pStream = 0;
    CreateStreamOnHGlobal(0, TRUE, &pStream);
    LONG cbSize = 0;
    hr = pPicture->SaveAsFile(pStream, TRUE, &cbSize);
    if(FAILED(hr)) return NULL;
    HGLOBAL hBuf = 0;
    GetHGlobalFromStream(pStream, &hBuf);
    void *buffer = GlobalLock(hBuf);
    char *hexbuf = (char *)malloc(cbSize * 2 + 16);
    char2hex((unsigned char *)buffer, hexbuf, cbSize);
#ifdef ICONSDEBUG
    HANDLE hFile = CreateFile("debug.ico", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if(hFile) {
        DWORD written = 0;
        WriteFile(hFile, buffer, cbSize, &written, 0);
        CloseHandle(hFile);
    }
#endif
    return hexbuf;
}

static char *IconSerialize32Bit(HICON Icon) {
    _unlink(TMPICONPATH);
    SaveIconToFile(&Icon, 1, TMPICONPATH);
    FILE *f = fopen(TMPICONPATH, "rb");
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    BYTE *buf = (BYTE *)malloc(len);
    fseek(f, 0, SEEK_SET);
    fread(buf, len, 1, f);
    char *hbuf = (char *)malloc(2 * len + 16);
    char2hex(buf, hbuf, len);
    free(buf);
    fclose(f);
    _unlink(TMPICONPATH);
    return hbuf;
}

char *CGameIcons::Serialize(HICON Icon) {
    if(g32BitIcons)
        return IconSerialize32Bit(Icon);
    else
        return IconSerialize4Bit(Icon);
}

HICON CGameIcons::DeSerialize(char *buf) {
    int len = strlen(buf);
    if(!len) return NULL; // no serialized icon
    _unlink(TMPICONPATH);
    HANDLE hFile = CreateFile(
                       TMPICONPATH,
                       GENERIC_WRITE,
                       0, // FILE_SHARE_READ | FILE_SHARE_WRITE,
                       0,
                       CREATE_ALWAYS,
                       0, // FILE_ATTRIBUTE_TEMPORARY, // FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
                       0);
    if(!hFile) {
        OutTrace("CreateFile failure: path=%s error=%d\n", TMPICONPATH, GetLastError());
        return NULL;
    }
    unsigned char *buffer = (unsigned char *)malloc(len / 2 + 16);
    len = hex2char(buffer, buf);
    if(!len) {
        OutTrace("icon hex2char error\n");
        CloseHandle(hFile);
        _unlink(TMPICONPATH);
        return NULL;
    }
    DWORD written = 0;
    WriteFile(hFile, buffer, len, &written, 0);
    CloseHandle(hFile);
    if(len != written) {
        OutTrace("WriteFile failure: error=%d\n", GetLastError());
        CloseHandle(hFile);
        _unlink(TMPICONPATH);
        return NULL;
    }
    HICON hResIcon = (HICON)LoadImage(
                         NULL,
                         TMPICONPATH,
                         IMAGE_ICON,
                         0,
                         0,
                         LR_DEFAULTSIZE | LR_LOADFROMFILE);
    if(hResIcon == NULL)
        OutTrace("de-serialize error %d\n", GetLastError());
#ifndef DEBUG
    _unlink(TMPICONPATH);
#endif
    return hResIcon;
}

HICON CGameIcons::GrayIcon(HICON Icon) {
    // n.b. gets in input a icon handle for any type of icon and returns a gray 4 bits x pixel icon
    BYTE *buf;
    char *hexbuf;
    BYTE *bmpdata, *p;
    ICONDIRENTRY *id;
    HICON GrayIco;
    PICTDESC desc = { sizeof(PICTDESC)};
    desc.picType = PICTYPE_ICON;
    desc.icon.hicon = Icon;
    IPicture *pPicture = 0;
    HRESULT hr = OleCreatePictureIndirect(&desc, IID_IPicture, FALSE, (void **)&pPicture);
    if(FAILED(hr)) return NULL;
    IStream *pStream = 0;
    CreateStreamOnHGlobal(0, TRUE, &pStream);
    LONG cbSize = 0;
    hr = pPicture->SaveAsFile(pStream, TRUE, &cbSize);
    if(FAILED(hr)) {
        OutTrace("GrayIcon: SaveAsFile error hr=%#x\n", hr);
        return NULL;
    }
    HGLOBAL hBuf = 0;
    hr = GetHGlobalFromStream(pStream, &hBuf);
    if(FAILED(hr)) {
        OutTrace("GrayIcon: GetHGlobalFromStream error hr=%#x\n", hr);
        return NULL;
    }
    buf = (BYTE *)GlobalLock(hBuf);
    if(buf == NULL) {
        OutTrace("GrayIcon: GlobalLock error err=%d\n", GetLastError());
        return NULL;
    }
    // point to bitmap data and replace all pixels (2 pixels x byte) with 0x77 = gray,gray
    id = (ICONDIRENTRY *)&buf[6];
    //OutTrace("GrayIcon: buf=%#x\n", buf);
    //OutTrace("GrayIcon: id=%d\n", id);
    //OutTrace("GrayIcon: imageoffset=%d\n", id->dwImageOffset);
    //OutTrace("GrayIcon: bmpdata=%d\n", id->dwImageOffset+sizeof(BITMAPINFOHEADER)+sizeof(ICONDIR));
    bmpdata = &buf[id->dwImageOffset + sizeof(BITMAPINFOHEADER) + sizeof(ICONDIR)];
    int bmaplen = (32 * 16) + 58;
    for(p = bmpdata; p < bmpdata + bmaplen; p++) {
        // Windows default 16 colors palette:
        // 0: black		1: maroon	2: green	3: olive	4: navy		5: purple	6: teal		7: silver
        // 8: grey		9: red		A: lime		B: yellow	C: blue		D: fuchsia	E: aqua		F: white
        //if(*p) *p= 0x77; -- not all pixels are colored ....
        //*p= 0x77; // silver
        //*p= 0xEE; // clear gray
        //*p= 0x00; // black
        *p = 0x88; // grey
    }
    hexbuf = (char *)malloc(2 * cbSize + 16);
    char2hex(buf, hexbuf, cbSize);
    GrayIco = DeSerialize(hexbuf);
    free(hexbuf);
    return GrayIco;
}
