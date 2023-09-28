// thanks to https://stackoverflow.com/questions/20245262/c-program-needs-an-file-association

#include "stdafx.h"
#include "dxwndhost.h"

void RegSet(HKEY hkeyHive, const char *pszVar, const char *pszValue ) {
    HKEY hkey;
    char szValueCurrent[1000];
    char sMsg[1000];
    DWORD dwType;
    DWORD dwSize = sizeof( szValueCurrent );
    typedef LSTATUS (WINAPI * RegGetValueA_Type)(HKEY, LPCSTR, LPCSTR, DWORD, LPDWORD, PVOID, LPDWORD);
    typedef LSTATUS (WINAPI * RegQueryValueExA_Type)(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
    HMODULE hAdvAPI32 = LoadLibrary("AdvAPI32.dll");
    if(hAdvAPI32 == NULL) return;
    RegGetValueA_Type pRegGetValueA = (RegGetValueA_Type)GetProcAddress(hAdvAPI32, "RegGetValueA");
    RegQueryValueExA_Type pRegQueryValueExA = NULL;
    if(pRegGetValueA == NULL) {
        pRegQueryValueExA = (RegQueryValueExA_Type)GetProcAddress(hAdvAPI32, "RegQueryValueExA");
        if (pRegQueryValueExA == NULL) {
            CloseHandle(hAdvAPI32);
            return;
        }
    }
    int iRC;
    if(pRegGetValueA)
        iRC = (*pRegGetValueA)(hkeyHive, pszVar, NULL, RRF_RT_ANY, &dwType, szValueCurrent, &dwSize);
    else
        iRC = (*pRegQueryValueExA)(hkeyHive, pszVar, NULL, &dwType, (LPBYTE)szValueCurrent, &dwSize);
    bool bDidntExist = iRC == ERROR_FILE_NOT_FOUND;
    if (iRC != ERROR_SUCCESS && !bDidntExist) {
        sprintf_s(sMsg, 1000, "RegGetValue(%s): error=%d", pszVar, iRC);
        OutTrace("%s\n", sMsg);
        MessageBox(NULL, sMsg, "DxWnd", 0);
        return;
    }
    if ( !bDidntExist ) {
        if ( dwType != REG_SZ ) {
            OutTrace("Key \"%s\" type mismatch: type=%x\n", pszVar, dwType);
            return;
        }
        if ( strcmp( szValueCurrent, pszValue ) == 0 ) {
            OutTrace("Key \"%s\" already set\n", pszVar);
            return;
        }
    }
    DWORD dwDisposition;
    iRC = RegCreateKeyEx( hkeyHive, pszVar, 0, 0, 0, KEY_ALL_ACCESS, NULL, &hkey, &dwDisposition );
    if ( iRC != ERROR_SUCCESS ) {
        OutTrace("RegCreateKeyEx(\"%s\") error: err=%d\n", pszVar, iRC);
        return;
    }
    iRC = RegSetValueEx( hkey, "", 0, REG_SZ, (BYTE *) pszValue, strlen( pszValue ) + 1 );
    if ( iRC != ERROR_SUCCESS ) {
        OutTrace("RegSetValueEx(\"%s\") error: err=%d\n", pszVar, iRC);
        return;
    }
    if ( bDidntExist )
        sprintf_s(sMsg, 1000, "RegSet(%s): set to \"%s\"", pszVar, pszValue );
    else
        sprintf_s(sMsg, 1000, "RegSet(%s): changed \"%s\" to \"%s\"", pszVar, szValueCurrent, pszValue );
    OutTrace("%s\n", sMsg);
    //MessageBox(NULL, sMsg, "DxWnd", 0);
    RegCloseKey(hkey);
    CloseHandle(hAdvAPI32);
}

void RegClear(HKEY hkeyHive, const char *pszVar) {
    char sMsg[1000];
    // v2.04.90: replaced RegDeleteKeyExA with RegDeleteKeyA to recover WinXP support
    //int iRC = RegDeleteKeyExA(hkeyHive, pszVar, KEY_WOW64_32KEY, 0);
    int iRC = RegDeleteKeyA(hkeyHive, pszVar);
    bool bDidntExist = iRC == ERROR_FILE_NOT_FOUND;
    if(iRC != ERROR_SUCCESS && !bDidntExist) {
        sprintf_s(sMsg, 1000, "RegClear(%s): error=%d", pszVar, iRC);
        OutTrace("%s\n", sMsg);
        MessageBox(NULL, sMsg, "DxWnd", 0);
        return;
    }
    if ( bDidntExist )
        sprintf_s(sMsg, 1000, "RegClear(%s): didn't exist", pszVar);
    else
        sprintf_s(sMsg, 1000, "RegClear(%s): deleted", pszVar);
    OutTrace("%s\n", sMsg);
    //MessageBox(NULL, sMsg, "DxWnd", 0);
}

int SetUpRegistry() {
    char sDxWndCommand[MAX_PATH + 81];
    char lpFileName[MAX_PATH + 1];
    // build associated open command
    GetModuleFileName(NULL, lpFileName, MAX_PATH);
    sprintf_s(sDxWndCommand, MAX_PATH + 80, "%s \"/L:%%1\"", lpFileName);
    //app doesn't have permission for this when run as normal user, but may for Admin?  Anyway, not needed.
    //RegSet( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\DxWnd.exe", "DxWnd.exe" );
    RegSet( HKEY_CURRENT_USER, "Software\\Classes\\.dxw", "ghotik.DxWnd.1" );
    // Not needed.
    RegSet( HKEY_CURRENT_USER, "Software\\Classes\\.dxw\\Content Type", "text/plain" );
    RegSet( HKEY_CURRENT_USER, "Software\\Classes\\.dxw\\PerceivedType", "text" );
    //Not needed, but may be be a way to have wordpad show up on the default list.
    //RegSet( HKEY_CURRENT_USER, "Software\\Classes\\.dxw\\OpenWithProgIds\\ghotik.DxWnd.1", "" );
    RegSet( HKEY_CURRENT_USER, "Software\\Classes\\ghotik.DxWnd.1", "DxWnd export" );
    RegSet( HKEY_CURRENT_USER, "Software\\Classes\\ghotik.DxWnd.1\\Shell\\Open\\Command", sDxWndCommand);
    SHChangeNotify( SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL );
    return 0;
}

int ClearRegistry() {
    RegClear( HKEY_CURRENT_USER, "Software\\Classes\\.dxw\\Content Type");
    RegClear( HKEY_CURRENT_USER, "Software\\Classes\\.dxw\\PerceivedType");
    RegClear( HKEY_CURRENT_USER, "Software\\Classes\\.dxw");
    RegClear( HKEY_CURRENT_USER, "Software\\Classes\\ghotik.DxWnd.1\\Shell\\Open\\Command");
    RegClear( HKEY_CURRENT_USER, "Software\\Classes\\ghotik.DxWnd.1\\Shell\\Open");
    RegClear( HKEY_CURRENT_USER, "Software\\Classes\\ghotik.DxWnd.1\\Shell");
    RegClear( HKEY_CURRENT_USER, "Software\\Classes\\ghotik.DxWnd.1");
    //RegClear( HKEY_CURRENT_USER, "Software\\Classes",".dxw\\Content Type");
    //RegClear( HKEY_CURRENT_USER, "Software\\Classes",".dxw\\PerceivedType");
    //RegClear( HKEY_CURRENT_USER, "Software\\Classes",".dxw");
    //RegClear( HKEY_CURRENT_USER, "Software\\Classes","ghotik.DxWnd.1\\Shell\\Open\\Command");
    //RegClear( HKEY_CURRENT_USER, "Software\\Classes","ghotik.DxWnd.1\\Shell\\Open");
    //RegClear( HKEY_CURRENT_USER, "Software\\Classes","ghotik.DxWnd.1\\Shell");
    //RegClear( HKEY_CURRENT_USER, "Software\\Classes","ghotik.DxWnd.1");
    SHChangeNotify( SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL );
    return 0;
}