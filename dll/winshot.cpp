#define _CRT_SECURE_NO_WARNINGS
#define SYSLIBNAMES_DEFINES

#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"

// WinScreenShot: derived from MSDN example with minor adaptations
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

//
//   FUNCTION: CaptureAnImage(HWND hWnd)
//
//   PURPOSE: Captures a screenshot into a window and then saves it in a .bmp file.
//
//   COMMENTS:
//
//      Note: This sample will attempt to create a file called captureqwsx.bmp
//

int WinScreenShot(HWND hWnd, int hash) {
    HDC hdcWindow;
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;
    char pszFile[MAX_PATH];
    hdcWindow = GetDC(hWnd);
    // Create a compatible DC which is used in a BitBlt from the window DC
    hdcMemDC = CreateCompatibleDC(hdcWindow);
    if(!hdcMemDC) {
        MessageBox(hWnd, "CreateCompatibleDC has failed", "Failed", MB_OK);
        goto done;
    }
    // Get the client area for size calculation
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    // Create a compatible bitmap from the Window DC
    hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
    if(!hbmScreen) {
        MessageBox(hWnd, "CreateCompatibleBitmap Failed", "Failed", MB_OK);
        goto done;
    }
    // Select the compatible bitmap into the compatible memory DC.
    SelectObject(hdcMemDC, hbmScreen);
    // Bit block transfer into our compatible memory DC.
    if(!BitBlt(hdcMemDC,
               0, 0,
               rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
               hdcWindow,
               0, 0,
               SRCCOPY)) {
        MessageBox(hWnd, "BitBlt has failed", "Failed", MB_OK);
        goto done;
    }
    // Get the BITMAP from the HBITMAP
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);
    BITMAPFILEHEADER   bmfHeader;
    BITMAPINFOHEADER   bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;
    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc
    // have greater overhead than HeapAlloc.
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char *lpbitmap = (char *)GlobalLock(hDIB);
    // Gets the "bits" from the bitmap and copies them into a buffer
    // which is pointed to by lpbitmap.
    GetDIBits(hdcWindow, hbmScreen, 0,
              (UINT)bmpScreen.bmHeight,
              lpbitmap,
              (BITMAPINFO *)&bi, DIB_RGB_COLORS);
    // A file is created, this is where we will save the screen capture.
    sprintf_s(pszFile, MAX_PATH, "%s\\screenshot.out\\shot.%08d.bmp", GetDxWndPath(), hash++);
    HANDLE hFile = CreateFile(pszFile,
                              GENERIC_WRITE,
                              0,
                              NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    // Add the size of the headers to the size of the bitmap to get the total file size
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    //Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
    //Size of the file
    bmfHeader.bfSize = dwSizeofDIB;
    //bfType must always be BM for Bitmaps
    bmfHeader.bfType = 0x4D42; //BM
    DWORD dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
    //Unlock and Free the DIB from the heap
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    //Close the handle for the file that was created
    CloseHandle(hFile);
    //Clean up
done:
    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    ReleaseDC(hWnd, hdcWindow);
    return 0;
}

int DCScreenShot(HDC hdcWindow, int hash) {
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;
    char pszFile[MAX_PATH];
    // Get the client area for size calculation
    RECT rcClient = dxw.GetScreenRect();
    // Create a compatible DC which is used in a BitBlt from the window DC
    hdcMemDC = CreateCompatibleDC(hdcWindow);
    if(!hdcMemDC) {
        MessageBox(NULL, "CreateCompatibleDC has failed", "Failed", MB_OK);
        goto done;
    }
    // Create a compatible bitmap from the Window DC
    hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
    if(!hbmScreen) {
        MessageBox(NULL, "CreateCompatibleBitmap Failed", "Failed", MB_OK);
        goto done;
    }
    // Select the compatible bitmap into the compatible memory DC.
    SelectObject(hdcMemDC, hbmScreen);
    // Bit block transfer into our compatible memory DC.
    if(!BitBlt(hdcMemDC,
               0, 0,
               rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
               hdcWindow,
               0, 0,
               SRCCOPY)) {
        MessageBox(NULL, "BitBlt has failed", "Failed", MB_OK);
        goto done;
    }
    // Get the BITMAP from the HBITMAP
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);
    BITMAPFILEHEADER   bmfHeader;
    BITMAPINFOHEADER   bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;
    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc
    // have greater overhead than HeapAlloc.
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char *lpbitmap = (char *)GlobalLock(hDIB);
    // Gets the "bits" from the bitmap and copies them into a buffer
    // which is pointed to by lpbitmap.
    GetDIBits(hdcWindow, hbmScreen, 0,
              (UINT)bmpScreen.bmHeight,
              lpbitmap,
              (BITMAPINFO *)&bi, DIB_RGB_COLORS);
    // A file is created, this is where we will save the screen capture.
    sprintf_s(pszFile, MAX_PATH, "%s\\screenshot.out\\shot.%08d.bmp", GetDxWndPath(), hash++);
    HANDLE hFile = CreateFile(pszFile,
                              GENERIC_WRITE,
                              0,
                              NULL,
                              CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    // Add the size of the headers to the size of the bitmap to get the total file size
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    //Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
    //Size of the file
    bmfHeader.bfSize = dwSizeofDIB;
    //bfType must always be BM for Bitmaps
    bmfHeader.bfType = 0x4D42; //BM
    DWORD dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
    //Unlock and Free the DIB from the heap
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    //Close the handle for the file that was created
    CloseHandle(hFile);
    //Clean up
done:
    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    return 0;
}