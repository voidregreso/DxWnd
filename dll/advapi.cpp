#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "syslibs.h"
#include "dxhook.h"
#include "dxhelper.h"

typedef LONG (WINAPI *RegSetValueA_Type)(HKEY, LPCSTR, DWORD, LPCSTR, DWORD);
LONG WINAPI extRegSetValueA(HKEY, LPCSTR, DWORD, LPCSTR, DWORD);
RegSetValueA_Type pRegSetValueA;

typedef BOOL (WINAPI *GetUserNameA_Type)(LPSTR, LPDWORD);
GetUserNameA_Type pGetUserNameA;
BOOL WINAPI extGetUserNameA(LPSTR, LPDWORD);

static HookEntryEx_Type Hooks[] = {
    {HOOK_IAT_CANDIDATE, 0, "RegOpenKeyA", (FARPROC)RegOpenKeyA, (FARPROC *) &pRegOpenKey, (FARPROC)extRegOpenKey},
    {HOOK_IAT_CANDIDATE, 0, "RegOpenKeyExA", (FARPROC)RegOpenKeyExA, (FARPROC *) &pRegOpenKeyEx, (FARPROC)extRegOpenKeyEx},
    {HOOK_IAT_CANDIDATE, 0, "RegCloseKey", (FARPROC)RegCloseKey, (FARPROC *) &pRegCloseKey, (FARPROC)extRegCloseKey},
    {HOOK_IAT_CANDIDATE, 0, "RegQueryValueA", (FARPROC)RegQueryValueA, (FARPROC *) &pRegQueryValue, (FARPROC)extRegQueryValue},
    {HOOK_IAT_CANDIDATE, 0, "RegQueryValueExA", (FARPROC)RegQueryValueExA, (FARPROC *) &pRegQueryValueEx, (FARPROC)extRegQueryValueEx},
    {HOOK_IAT_CANDIDATE, 0, "RegCreateKeyA", (FARPROC)RegCreateKeyA, (FARPROC *) &pRegCreateKey, (FARPROC)extRegCreateKey},
    {HOOK_IAT_CANDIDATE, 0, "RegCreateKeyExA", (FARPROC)RegCreateKeyExA, (FARPROC *) &pRegCreateKeyEx, (FARPROC)extRegCreateKeyEx},
    {HOOK_IAT_CANDIDATE, 0, "RegSetValueExA", (FARPROC)RegSetValueExA, (FARPROC *) &pRegSetValueEx, (FARPROC)extRegSetValueEx},
    {HOOK_IAT_CANDIDATE, 0, "RegFlushKey", (FARPROC)RegFlushKey, (FARPROC *) &pRegFlushKey, (FARPROC)extRegFlushKey},
    // v2.3.36
    {HOOK_IAT_CANDIDATE, 0, "RegEnumValueA", (FARPROC)RegEnumValueA, (FARPROC *) &pRegEnumValueA, (FARPROC)extRegEnumValueA},
    // v2.4.37
    {HOOK_IAT_CANDIDATE, 0, "RegEnumKeyA", (FARPROC)RegEnumKeyA, (FARPROC *) &pRegEnumKeyA, (FARPROC)extRegEnumKeyA},
    {HOOK_IAT_CANDIDATE, 0, "RegEnumKeyExA", (FARPROC)RegEnumKeyExA, (FARPROC *) &pRegEnumKeyExA, (FARPROC)extRegEnumKeyExA},
    // v2.4.97
    {HOOK_IAT_CANDIDATE, 0, "RegQueryInfoKeyA", (FARPROC)RegQueryInfoKeyA, (FARPROC *) &pRegQueryInfoKeyA, (FARPROC)extRegQueryInfoKeyA},
    // v2.5.48 - "Loony Labyrinth Pinball"
    {HOOK_IAT_CANDIDATE, 0, "RegSetValueA", (FARPROC)RegSetValueA, (FARPROC *) &pRegSetValueA, (FARPROC)extRegSetValueA},
    // v2.5.54 - "Return Fire"
    {HOOK_IAT_CANDIDATE, 0, "GetUserNameA", (FARPROC)GetUserNameA, (FARPROC *) &pGetUserNameA, (FARPROC)extGetUserNameA},
    {HOOK_IAT_CANDIDATE, 0, 0, NULL, 0, 0} // terminator
};

void MakeHot(HookEntryEx_Type *Hooks) {
    for(; Hooks->APIName; Hooks++)
        if((Hooks->HookStatus == HOOK_IAT_CANDIDATE) || (Hooks->HookStatus == HOOK_HOT_CANDIDATE))
            Hooks->HookStatus = HOOK_HOT_REQUIRED;
}

void HookAdvApi32(HMODULE module) {
    if(	 dxw.dwFlags9 & HOTREGISTRY  ) MakeHot(Hooks);
    if( (dxw.dwFlags3 & EMULATEREGISTRY) ||
            (dxw.dwFlags4 & OVERRIDEREGISTRY) ||
            (dxw.dwFlags6 & (WOW32REGISTRY | WOW64REGISTRY)) ||
            IsTraceR)
        HookLibraryEx(module, Hooks, "ADVAPI32.dll");
}

FARPROC Remap_AdvApi32_ProcAddress(LPCSTR proc, HMODULE hModule) {
    FARPROC addr;
    if(!(dxw.dwFlags3 & EMULATEREGISTRY) ||
            (dxw.dwFlags4 & OVERRIDEREGISTRY) ||
            (dxw.dwFlags6 & (WOW32REGISTRY | WOW64REGISTRY)) ||
            IsTraceR) return NULL;
    if (addr = RemapLibraryEx(proc, hModule, Hooks)) return addr;
    return NULL;
}

#define HKEY_FAKE ((HKEY)0x7FFFFFFF)
#define HKEY_MASK 0x7FFFFF00
#define IsFake(hKey) (((DWORD)hKey & HKEY_MASK) == HKEY_MASK)

static FILE *OpenFakeRegistry();
static char *hKey2String(HKEY);
static LONG myRegOpenKeyEx(HKEY, LPCTSTR, PHKEY);

// int ReplaceVar(pData, lplpData, lpcbData):
// extract the token name from pData beginning up to '}' delimiter
// calculates the value of the token. If the token is unknown, the value is null string.
// if *lplpData, copies the token value string to *lplpData and increments *lplpData
// if lpcbData, increments the key length of the token value length
// returns the length of token label to advance the parsing loop

typedef enum {
    LABEL_PATH = 0,
    LABEL_WORKDIR,
    LABEL_VOID,
    LABEL_END
};

static char *sTokenLabels[LABEL_END + 1] = {
    "path}",
    "work}",
    "}",
    NULL
};

#ifndef DXW_NOTRACES
static char *sType(DWORD t) {
    char *s;
    switch(t) {
    case REG_NONE:
        s = "NONE";
        break;
    case REG_SZ:
        s = "SZ";
        break;
    case REG_EXPAND_SZ:
        s = "EXPAND_SZ";
        break;
    case REG_BINARY:
        s = "BINARY";
        break;
    case REG_DWORD:
        s = "DWORD";
        break;
    case REG_DWORD_BIG_ENDIAN:
        s = "DWORD_BIG_ENDIAN";
        break;
    case REG_LINK:
        s = "LINK";
        break;
    case REG_MULTI_SZ:
        s = "MULTI_SZ";
        break;
    case REG_RESOURCE_LIST:
        s = "RESOURCE_LIST";
        break;
    case REG_FULL_RESOURCE_DESCRIPTOR:
        s = "FULL_RESOURCE_DESCRIPTOR";
        break;
    case REG_RESOURCE_REQUIREMENTS_LIST:
        s = "RESOURCE_REQUIREMENTS_LIST";
        break;
    case REG_QWORD:
        s = "QWORD";
        break;
    default:
        s = "???";
        break;
    }
    return s;
}
#endif // DXW_NOTRACES

static int ReplaceVar(char *pData, LPBYTE *lplpData, LPDWORD lpcbData) {
    int iTokenLength;
    int iLabelLength;
    int iTokenIndex;
    char sTokenValue[MAX_PATH];
    char *p;
    // search for a matching token
    for(iTokenIndex = 0; sTokenLabels[iTokenIndex]; iTokenIndex++) {
        if(!_strnicmp(pData, sTokenLabels[iTokenIndex], strlen(sTokenLabels[iTokenIndex]))) break;
    }
    // set token label length
    iLabelLength = strlen(sTokenLabels[iTokenIndex]);
    // do replacement
    // v2.05.53: "path" vs. "work", made different
    switch(iTokenIndex) {
    case LABEL_PATH:
        GetModuleFileNameA(NULL, sTokenValue, sizeof(sTokenValue));
        //PathRemoveFileSpecA(sTokenValue);
        p = sTokenValue + strlen(sTokenValue);
        while((*p != '\\') && (p > sTokenValue)) p--;
        if(*p == '\\') *p = 0;
        break;
    case LABEL_WORKDIR:
        GetCurrentDirectory(MAX_PATH, sTokenValue);
        break;
    case LABEL_VOID:
    case LABEL_END:
        strcpy(sTokenValue, "");
        break;
    }
    // set output vars if not NULL
    iTokenLength = strlen(sTokenValue);
    OutTraceR("REPLACED token=%d val=\"%s\" len=%d\n", iTokenIndex, sTokenValue, iTokenLength);
    if(*lplpData) {
        strcpy((char *)*lplpData, sTokenValue);
        *lplpData += iTokenLength;
    }
    if(lpcbData) *lpcbData += iTokenLength;
    // return label length to advance parsing
    return iLabelLength;
}

static char *hKey2String(HKEY hKey) {
    char *skey;
    static char sKey[MAX_PATH + 1];
    static char skeybuf[10];
    if(IsFake(hKey)) {
        FILE *regf;
        char RegBuf[MAX_PATH + 1];
        regf = OpenFakeRegistry();
        if(regf != NULL) {
            HKEY hLocalKey = HKEY_FAKE;
            fgets(RegBuf, 256, regf);
            while (!feof(regf)) {
                if(RegBuf[0] == '[') {
                    if(hLocalKey == hKey) {
                        //OutTrace("building fake Key=\"%s\" hKey=%#x\n", sKey, hKey);
                        fclose(regf);
                        strcpy(sKey, &RegBuf[1]);
                        //sKey[strlen(sKey)-2]=0; // get rid of "]"
                        for(int i = strlen(sKey) - 1; i; i--) {
                            if(sKey[i] == ']') {
                                sKey[i] = 0;
                                break;
                            }
                        }
                        return sKey;
                    } else
                        hLocalKey--;
                }
                fgets(RegBuf, 256, regf);
            }
            fclose(regf);
        }
        return "HKEY_NOT_FOUND";
    }
    switch((ULONG)hKey) {
    case HKEY_CLASSES_ROOT:
        skey = "HKEY_CLASSES_ROOT";
        break;
    case HKEY_CURRENT_CONFIG:
        skey = "HKEY_CURRENT_CONFIG";
        break;
    case HKEY_CURRENT_USER:
        skey = "HKEY_CURRENT_USER";
        break;
    case HKEY_LOCAL_MACHINE:
        skey = "HKEY_LOCAL_MACHINE";
        break;
    case HKEY_USERS:
        skey = "HKEY_USERS";
        break;
    default:
        sprintf(skeybuf, "%#x", hKey);
        skey = skeybuf;
        break;
    }
    return skey;
}

static char *Unescape(char *s, char **dest) {
    if(!*dest)	*dest = (char *)malloc(strlen(s) + 100);
    else		*dest = (char *)realloc(*dest, strlen(s) + 100);
    char *t = *dest;
    for(; *s; s++) {
        if((*s == '\\') && (*(s + 1) == 'n')) {
            *t++ = '\n';
            s++;
        } else
            *t++ = *s;
    }
    *t = 0;
    return *dest;
}

static FILE *OpenFakeRegistry() {
    char sSourcePath[MAX_PATH + 1];
    sprintf_s(sSourcePath, MAX_PATH, "%s\\dxwnd.reg", GetDxWndPath());
    return fopen(sSourcePath, "r");
}

static LONG SeekFakeKey(FILE *regf, HKEY hKey) {
    LONG res;
    res = ERROR_FILE_NOT_FOUND;
    char RegBuf[MAX_PATH + 1];
    HKEY hCurKey = HKEY_FAKE + 1;
    fgets(RegBuf, 256, regf);
    while (!feof(regf)) {
        if(RegBuf[0] == '[')
            hCurKey--;
        if(hCurKey == hKey) {
            //OutTraceREGDebug("DEBUG: SeekFakeKey fount key at line=%s\n", RegBuf);
            res = ERROR_SUCCESS;
            break;
        }
        fgets(RegBuf, 256, regf);
    }
    return res;
}

static LONG SeekValueName(FILE *regf, LPCTSTR lpValueName) {
    LONG res;
    char RegBuf[MAX_PATH + 1];
    long KeySeekPtr;
    res = ERROR_FILE_NOT_FOUND;
    // v2.04.01: fix to handle the '@' case properly
    if(lpValueName) if(!lpValueName[0]) lpValueName = NULL;
    KeySeekPtr = ftell(regf);
    fgets(RegBuf, 256, regf);
    while (!feof(regf)) {
        if(!lpValueName) {
            if((RegBuf[0] == '@') || (!strcmp(RegBuf, "\"\"="))) {
                fseek(regf, KeySeekPtr, SEEK_SET);
                return ERROR_SUCCESS;
            }
        } else {
            if((RegBuf[0] == '"') &&
                    !_strnicmp(lpValueName, &RegBuf[1], strlen(lpValueName)) &&
                    (RegBuf[strlen(lpValueName) + 1] == '"') &&
                    (RegBuf[strlen(lpValueName) + 2] == '=')) {
                fseek(regf, KeySeekPtr, SEEK_SET);
                return ERROR_SUCCESS;
            }
        }
        // the next Key definition "[" can stop the search
        if(RegBuf[0] == '[') return res;
        KeySeekPtr = ftell(regf);
        fgets(RegBuf, 256, regf);
    }
    return res;
}

static LONG SeekValueIndex(FILE *regf, DWORD dwIndex, LPCTSTR lpValueName, LPDWORD lpcchValueName) {
    LONG res;
    char RegBuf[MAX_PATH + 1];
    long KeySeekPtr;
    res = ERROR_NO_MORE_ITEMS;
    KeySeekPtr = ftell(regf);
    fgets(RegBuf, 256, regf);
    dwIndex++;
    while (!feof(regf) && dwIndex) {
        if(RegBuf[0] == '"') dwIndex--;
        if(dwIndex == 0) {
            fseek(regf, KeySeekPtr, SEEK_SET);
            //sscanf(RegBuf, "\"%s\"=", lpValueName);
            strncpy((char *)lpValueName, strtok(&RegBuf[1], "\""), *lpcchValueName);
            *lpcchValueName = strlen(lpValueName);
            //OutTrace("DEBUG: lpValueName=%s len=%d\n", lpValueName, *lpcchValueName);
            return ERROR_SUCCESS;
        }
        if(RegBuf[0] == '[') return res;
        KeySeekPtr = ftell(regf);
        fgets(RegBuf, 256, regf);
    }
    return res;
}

static DWORD GetKeyValue(
    FILE *regf,
    char *ApiName,
    LPCTSTR lpValueName,
    LPDWORD lpType, // beware: could be NULL
    LPBYTE lpData,  // beware: could be NULL
    LPDWORD lpcbData) {
    LONG res;
    LPBYTE lpb;
    char *pData;
    char RegBuf[MAX_PATH + 1];
    DWORD cbData = 0;
    DWORD dwType;
    //OutTrace("GetKeyValue: ValueName=%s\n", lpValueName);
    fgets(RegBuf, 256, regf);
    if(RegBuf[0] == '@')
        pData = &RegBuf[2];
    else
        pData = &RegBuf[strlen(lpValueName) + 3];
    lpb = lpData;
    if(lpcbData) {
        cbData = *lpcbData;
        *lpcbData = 0;
    }
    do {
        if((*pData == '"') || (*pData == '(')) { // string value
            if(*pData == '(') {
                sscanf(pData, "(%d)\"", &dwType);
                pData += strlen("(n)\"");
            } else {
                dwType = REG_SZ;
                pData++;
            }
            while(*pData && (*pData != '"')) {
                if(*pData == '\\') {
                    pData++;
                    switch(*pData) {
                    case '{': {
                        pData++; // skip '{'
                        pData += ReplaceVar(pData, &lpb, lpcbData);
                        continue; // v2.04.13 fix
                    }
                    break;
                    default:
                        break; // skip first '\'
                    }
                }
                if(lpData && lpcbData) if(*lpcbData < cbData) *lpb++ = *pData;
                pData++;
                if(lpcbData) (*lpcbData)++;
            }
            if(lpcbData) (*lpcbData)++; // extra space for string terminator ?Get
            if(lpData && lpcbData) if(*lpcbData <= cbData) *lpb = 0; // string terminator
            OutTraceR("%s: type=%s cbData=%d Data=\"%s\"\n",
                      ApiName, sType(dwType),
                      lpcbData ? *lpcbData : 0,
                      lpData ? (char *)lpData : "(NULL)");
            // v2.04.14 fix: ERROR_MORE_DATA should be returned only in case lpData is not NULL
            res = ERROR_SUCCESS;
            if(lpData && lpcbData)
                if (*lpcbData > cbData) res = ERROR_MORE_DATA;
            break;
        }
        if(!strncmp(pData, "dword:", strlen("dword:"))) { //dword value
            DWORD val;
            dwType = REG_DWORD;
            pData += strlen("dword:");
            sscanf(pData, "%x", &val); // v2.05.19: fix - "%#x" -> "%x" !!!
            if(lpData) {
                if (cbData >= sizeof(DWORD)) {
                    memcpy(lpData, &val, sizeof(DWORD));
                    res = ERROR_SUCCESS;
                } else
                    res = ERROR_MORE_DATA;
            } else
                res = ERROR_SUCCESS; // data not needed
            if (lpcbData) *lpcbData = sizeof(DWORD);
            OutTraceR("%s: type=REG_DWORD cbData=%#x Data=%#x\n",
                      ApiName, lpcbData ? *lpcbData : 0, val);
            break;
        }
        if (!strncmp(pData, "hex", strlen("hex"))) { // default hex value REG_BINARY or custom hex value type hex(n)
            BYTE *p;
            p = (BYTE *)pData;
            if (!strncmp(pData, "hex(", strlen("hex("))) {
                sscanf(pData, "hex(%d):", &dwType);
                p += strlen("hex(n):");
            } else {
                dwType = REG_BINARY;
                p += strlen("hex:");
            }
            while(TRUE) {
                p[strlen((char *)p) - 1] = 0; // eliminates \n at the end of line
                while(strlen((char *)p) >= 3) {
                    if(lpcbData && (*lpcbData < cbData) && lpData) { // v2.05.54
                        UINT c;
                        sscanf((char *)p, "%x,", &c); // v2.05.27 fix - no # in sscanf format
                        *lpb = (BYTE)c;
                        lpb++;
                    }
                    p += 3;
                    if(lpcbData) (*lpcbData)++;
                }
                if(*p == '\\') {
                    fgets(RegBuf, 256, regf);
                    pData = RegBuf;
                    p = (BYTE *)pData;
                    while (*p == ' ') p++; // v2.05.54: skip leading blanks
                } else break;
            }
            OutTraceR("%s: type=%s cbData=%d Data=%s\n",
                      ApiName, sType(dwType),
                      lpcbData ? *lpcbData : 0,
                      lpData ? hexdump(lpData, *lpcbData) : "(NULL)");
            if(lpType) *lpType = dwType;
            if(lpcbData)   // v2.05.54
                res = (*lpcbData > cbData) ? ERROR_MORE_DATA : ERROR_SUCCESS;
            else
                res = ERROR_SUCCESS;
            break;
        }
    } while(FALSE);
    return res;
}

#ifndef DXW_NOTRACES
#define IfLogKeyValue(a, b, c, d, e) if(IsTraceR) LogKeyValue(a, b, c, d, e)
static void LogKeyValue(char *ApiName, LONG res, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    char sInfo[1024];
    if(res) {
        OutTrace("%s: ERROR res=%d\n", ApiName, res);
        return;
    }
    sprintf(sInfo, "%s: res=ERROR_SUCCESS size=%d type=%#x(%s)",
            ApiName, lpcbData ? *lpcbData : 0, lpType ? *lpType : 0, lpType ? ExplainRegType(*lpType) : "none");
    if(lpType && lpData && lpcbData) {
        DWORD cbData = *lpcbData;
        switch(*lpType) {
        case REG_SZ:
            sprintf(sInfo, "%s Data=\"%*.*s\"\n", sInfo, cbData - 1, cbData - 1, lpData);
            break;
        case REG_DWORD:
            sprintf(sInfo, "%s Data=%#x\n", sInfo, *(DWORD *)lpData);
            break;
        case REG_BINARY:
            sprintf(sInfo, "%s Data=%s\n", sInfo, hexdump((BYTE *)lpData, cbData));
            break;
        case REG_NONE:
            sprintf(sInfo, "%s Data=\"%s\"\n", sInfo, lpData);
            break;
        default:
            sprintf(sInfo, "%s Data=???\n", sInfo);
            break;
        }
    } else
        sprintf(sInfo, "%s\n", sInfo);
    OutTrace(sInfo);
}
#else
#define IfLogKeyValue(a, b, c, d, e)
#define LogKeyValue(a, b, c, d, e)
#endif

static LONG myRegOpenKeyEx(
    LPCSTR label,
    HKEY hKey,
    LPCTSTR lpSubKey,
    PHKEY phkResult) {
    FILE *regf;
    char sKey[MAX_PATH + 1];
    char RegBuf[MAX_PATH + 1];
    sprintf(sKey, "%s\\%s", hKey2String(hKey), lpSubKey);
    OutTraceR("%s: searching for key=\"%s\"\n", label, sKey);
    regf = OpenFakeRegistry();
    if(regf != NULL) {
        if(phkResult) *phkResult = HKEY_FAKE;
        fgets(RegBuf, 256, regf);
        while (!feof(regf)) {
            if(RegBuf[0] == '[') {
                // beware: registry keys are case insensitive. Must use _strnicmp instead of strncmp
                if((!_strnicmp(&RegBuf[1], sKey, strlen(sKey))) && (RegBuf[strlen(sKey) + 1] == ']')) {
                    OutTraceR("%s: found fake Key=\"%s\" hkResult=%#x\n", label, sKey, phkResult ? *phkResult : 0);
                    fclose(regf);
                    return ERROR_SUCCESS;
                } else {
                    if(phkResult) (*phkResult)--;
                }
            }
            fgets(RegBuf, 256, regf);
        }
        fclose(regf);
    }
    return ERROR_FILE_NOT_FOUND;
}

// ---------------------------------------------------------------------------------

LONG WINAPI extRegOpenKey( // so far, found only in "Moon Child"
    HKEY hKey,
    LPCTSTR lpSubKey,
    PHKEY phkResult) {
    LONG res;
    ApiName("RegOpenKey");
    OutTraceR("%s: hKey=%#x(%s) SubKey=\"%s\"\n", ApiRef, hKey, hKey2String(hKey), lpSubKey);
    if(dxw.dwFlags4 & OVERRIDEREGISTRY) {
        res = myRegOpenKeyEx(ApiRef, hKey, lpSubKey, phkResult);
        if(res == ERROR_SUCCESS) return res;
    }
    res = (*pRegOpenKey)(hKey, lpSubKey, phkResult);
    OutTraceR("%s: res=%#x phkResult=%#x\n", ApiRef, res, phkResult ? *phkResult : 0);
    if((res == ERROR_SUCCESS) || !(dxw.dwFlags3 & EMULATEREGISTRY) || (dxw.dwFlags4 & OVERRIDEREGISTRY)) return res;
    return myRegOpenKeyEx(ApiRef, hKey, lpSubKey, phkResult);
}

LONG WINAPI extRegOpenKeyEx(
    HKEY hKey,
    LPCTSTR lpSubKey,
    DWORD ulOptions,
    REGSAM samDesired,
    PHKEY phkResult) {
    LONG res;
    ApiName("RegOpenKeyEx");
    OutTraceR("%s: hKey=%#x(%s) SubKey=\"%s\" Options=%#x\n",
              ApiRef, hKey, hKey2String(hKey), lpSubKey, ulOptions);
    if(dxw.dwFlags4 & OVERRIDEREGISTRY) {
        res = myRegOpenKeyEx(ApiRef, hKey, lpSubKey, phkResult);
        if(res == ERROR_SUCCESS) return res;
    }
    if(dxw.dwFlags6 & WOW64REGISTRY) {
        ulOptions &= ~KEY_WOW64_32KEY;
        ulOptions |= KEY_WOW64_64KEY;
    }
    if(dxw.dwFlags6 & WOW32REGISTRY) {
        ulOptions &= ~KEY_WOW64_64KEY;
        ulOptions |= KEY_WOW64_32KEY;
    }
    res = (*pRegOpenKeyEx)(hKey, lpSubKey, ulOptions, samDesired, phkResult);
    OutTraceR("%s: res=%#x phkResult=%#x\n", ApiRef, res, phkResult ? *phkResult : 0);
    if((res == ERROR_SUCCESS) || !(dxw.dwFlags3 & EMULATEREGISTRY) || (dxw.dwFlags4 & OVERRIDEREGISTRY)) return res;
    return myRegOpenKeyEx(ApiRef, hKey, lpSubKey, phkResult);
}

// extRegQueryValue: legacy API, almost always replaced by extRegQueryValueEx but referenced
// in "Warhammer 40.000 Shadow of the Horned Rat"

LONG WINAPI extRegQueryValue(
    HKEY hKey,
    LPCTSTR lpSubKey,
    LPTSTR lpValue,
    PLONG lpcbValue) {
    LONG res;
    ApiName("RegQueryValue");
    FILE *regf;
    OutTraceR("%s: hKey=%#x(%s) SubKey=\"%s\"\n", ApiRef, hKey, hKey2String(hKey), lpSubKey);
    if (!IsFake(hKey)) {
        res = (*pRegQueryValue)(hKey, lpSubKey, lpValue, lpcbValue);
        IfLogKeyValue(ApiRef, res, 0, (LPBYTE)lpValue, (LPDWORD)lpcbValue);
        return res;
    }
    regf = OpenFakeRegistry();
    if(regf == NULL) {
        OutTraceR("%s: error in OpenFakeRegistry err=%s\n", ApiRef, GetLastError());
        return ERROR_FILE_NOT_FOUND;
    }
    res = SeekFakeKey(regf, hKey);
    if(res != ERROR_SUCCESS) {
        OutTraceR("%s: error in SeekFakeKey res=%#x hKey=%#x\n", ApiRef, res, hKey);
        return res;
    }
    res = SeekValueName(regf, lpSubKey);
    if(res != ERROR_SUCCESS) {
        OutTraceR("%s: error in SeekValueName res=%#x ValueName=%s\n", ApiRef, res, lpSubKey);
        return res;
    }
    res = GetKeyValue(regf, ApiRef, lpSubKey, NULL, (LPBYTE)lpValue, (LPDWORD)lpcbValue);
    IfLogKeyValue(ApiRef, res, NULL, (LPBYTE)lpValue, (LPDWORD)lpcbValue);
    fclose(regf);
    return res;
}

LONG WINAPI extRegQueryValueEx(
    HKEY hKey,
    LPCTSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType, // beware: could be NULL
    LPBYTE lpData,  // beware: could be NULL
    LPDWORD lpcbData) {
    LONG res;
    ApiName("RegQueryValueEx");
    FILE *regf;
    DWORD cbData = 0;
    OutTraceR("%s: hKey=%#x(\"%s\") ValueName=\"%s\" Reserved=%#x lpType=%#x lpData=%#x lpcbData=%#x\n",
              ApiRef, hKey, hKey2String(hKey), lpValueName, lpReserved, lpType, lpData, lpcbData);
    if (!IsFake(hKey)) {
        res = (*pRegQueryValueEx)(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
        IfLogKeyValue(ApiRef, res, lpType, lpData, lpcbData);
        return res;
    }
    regf = OpenFakeRegistry();
    if(regf == NULL) {
        OutTraceR("%s: error in OpenFakeRegistry err=%s\n", ApiRef, GetLastError());
        return ERROR_FILE_NOT_FOUND;
    }
    res = SeekFakeKey(regf, hKey);
    if(res != ERROR_SUCCESS) {
        OutTraceR("%s: error in SeekFakeKey res=%#x hKey=%#x\n", ApiRef, res, hKey);
        return res;
    }
    res = SeekValueName(regf, lpValueName);
    if(res != ERROR_SUCCESS) {
        OutTraceR("%s: error in SeekValueName res=%#x ValueName=%s\n", ApiRef, res, lpValueName);
        return res;
    }
    res = GetKeyValue(regf, ApiRef, lpValueName, lpType, lpData, lpcbData);
    IfLogKeyValue(ApiRef, res, lpType, lpData, lpcbData);
    fclose(regf);
    return res;
}

LONG WINAPI extRegCloseKey(HKEY hKey) {
    OutTraceR("RegCloseKey: hKey=%#x\n", hKey);
    if (IsFake(hKey)) return ERROR_SUCCESS;
    return (*pRegCloseKey)(hKey);
}

LONG WINAPI extRegFlushKey(HKEY hKey) {
    OutTraceR("RegFlushKey: hKey=%#x\n", hKey);
    if (IsFake(hKey)) return ERROR_SUCCESS;
    return (*pRegFlushKey)(hKey);
}

LONG WINAPI extRegSetValueEx(HKEY hKey, LPCTSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE *lpData, DWORD cbData) {
    ApiName("RegSetValueEx");
#ifndef DXW_NOTRACES
    if (IsTraceR) {
        char sInfo[1024];
        sprintf(sInfo, "%s: hKey=%#x ValueName=\"%s\" Type=%#x(%s) cbData=%d", ApiRef, hKey, lpValueName, dwType, ExplainRegType(dwType), cbData);
        switch(dwType) {
        case REG_DWORD:
            OutTrace("%s Data=%#x\n", sInfo, *(DWORD *)lpData);
            break;
        case REG_NONE:
            OutTrace("%s Data=\"%s\"\n", sInfo, lpData);
            break;
        case REG_BINARY:
            OutTrace("%s Data=%s\n", sInfo, hexdump((BYTE *)lpData, cbData));
            break;
        case REG_SZ:
            OutTrace("%s Data=\"%*.*s\"\n", sInfo, cbData - 1, cbData - 1, lpData);
            break;
        default:
            OutTrace("%s\n", sInfo);
        }
    }
#endif
    if(IsFake(hKey) && (dxw.dwFlags3 & EMULATEREGISTRY)) {
        OutTraceR("%s: SUPPRESS registry key set\n", ApiRef);
        return ERROR_SUCCESS;
    }
    return (*pRegSetValueEx)(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

LONG WINAPI extRegSetValueA(HKEY hKey, LPCSTR lpSubKey, DWORD dwType, LPCSTR lpData, DWORD cbData) {
    ApiName("RegSetValueA");
#ifndef DXW_NOTRACES
    if (IsTraceR) {
        char sInfo[1024];
        sprintf(sInfo, "%s: hKey=%#x(\"%s\") subkey=\"%s\" type=%d(%s) cbdata=%d ",
                ApiRef, hKey, hKey2String(hKey), lpSubKey, dwType, ExplainRegType(dwType), cbData);
        switch(dwType) {
        case REG_DWORD:
            OutTrace("%s Data=%#x\n", sInfo, *(DWORD *)lpData);
            break;
        case REG_NONE:
            OutTrace("%s Data=\"%s\"\n", sInfo, lpData);
            break;
        case REG_BINARY:
            OutTrace("%s Data=%s\n", sInfo, hexdump((BYTE *)lpData, cbData));
            break;
        case REG_SZ:
            OutTrace("%s Data=\"%*.*s\"\n", sInfo, cbData, cbData, lpData);
            break;
        default:
            OutTrace("%s\n", sInfo);
        }
    }
#endif
    if(IsFake(hKey) && (dxw.dwFlags3 & EMULATEREGISTRY)) {
        OutTraceR("%s: SUPPRESS registry key set\n", ApiRef);
        return ERROR_SUCCESS;
    }
    return (*pRegSetValueA)(hKey, lpSubKey, dwType, lpData, cbData);
}

LONG WINAPI extRegCreateKeyEx(HKEY hKey, LPCTSTR lpSubKey, DWORD Reserved, LPTSTR lpClass, DWORD dwOptions, REGSAM samDesired,
                              LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition) {
    ApiName("RegCreateKeyEx");
    LONG ret;
    OutTraceR("%s: hKey=%#x(%s) SubKey=\"%s\" Class=%#x\n", ApiRef, hKey, hKey2String(hKey), lpSubKey, lpClass);
    if (dxw.dwFlags3 & EMULATEREGISTRY) {
        *phkResult = HKEY_FAKE;
        // V2.3.12: return existing fake key if any ....
        if(dxw.dwFlags4 & OVERRIDEREGISTRY) myRegOpenKeyEx("RegCreateKeyEx", hKey, lpSubKey, phkResult);
        if(lpdwDisposition) *lpdwDisposition = REG_OPENED_EXISTING_KEY;
        ret = ERROR_SUCCESS;
    } else {
        LONG ret = (*pRegCreateKeyEx)(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired,
                                      lpSecurityAttributes, phkResult, lpdwDisposition);
    }
    if(ret == ERROR_SUCCESS)
        OutTraceR("%s: ret=%#x hkey=%#x disp=%#x\n", ApiRef, ret, *phkResult, lpdwDisposition ? *lpdwDisposition : 0);
    else
        OutTraceE("%s: ret=%#x err=%d\n", ApiRef, ret, GetLastError());
    return ret;
}

LONG WINAPI extRegCreateKey(HKEY hKey, LPCTSTR lpSubKey, PHKEY phkResult) {
    ApiName("RegCreateKey");
    OutTraceR("%s: hKey=%#x(%s) SubKey=\"%s\"\n", ApiRef, hKey, hKey2String(hKey), lpSubKey);
    if (dxw.dwFlags3 & EMULATEREGISTRY) {
        *phkResult = HKEY_FAKE;
        // V2.3.12: return existing fake key if any ....
        if(dxw.dwFlags4 & OVERRIDEREGISTRY) myRegOpenKeyEx("RegCreateKey", hKey, lpSubKey, phkResult);
        return ERROR_SUCCESS;
    } else {
        LONG ret = (*pRegCreateKey)(hKey, lpSubKey, phkResult);
        OutTraceR("%s: ret=%#x\n", ApiRef, ret);
        return ret;
    }
}

LONG WINAPI extRegEnumValueA(HKEY hKey, DWORD dwIndex, LPTSTR lpValueName, LPDWORD lpcchValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {
    LONG res;
    ApiName("RegEnumValue");
    OutTraceR("%s: hKey=%#x(\"%s\") index=%d cchValueName=%d Reserved=%#x lpType=%#x lpData=%#x lpcbData=%#x\n",
              ApiRef, hKey, hKey2String(hKey), dwIndex, *lpcchValueName, lpReserved, lpType, lpData, lpcbData);
    if (!IsFake(hKey)) {
        res = (*pRegEnumValueA)(hKey, dwIndex, lpValueName, lpcchValueName, lpReserved, lpType, lpData, lpcbData);
        IfLogKeyValue(ApiRef, res, lpType, lpData, lpcbData);
        return res;
    }
    // try emulated registry
    FILE *regf;
    regf = OpenFakeRegistry();
    if(regf == NULL) return ERROR_FILE_NOT_FOUND;
    res = SeekFakeKey(regf, hKey);
    if(res != ERROR_SUCCESS) return res;
    res = SeekValueIndex(regf, dwIndex, lpValueName, lpcchValueName);
    if(res != ERROR_SUCCESS) return res;
    res = GetKeyValue(regf, ApiRef, lpValueName, lpType, lpData, lpcbData);
    IfLogKeyValue(ApiRef, res, lpType, lpData, lpcbData);
    fclose(regf);
    return res;
}

LONG WINAPI extRegEnumKeyA(HKEY hKey, DWORD dwIndex, LPTSTR lpName, DWORD cchName) {
    LONG res;
    ApiName("RegEnumKey");
    OutTraceR("%s: hKey=%#x(\"%s\") index=%d cchName=%d\n",
              ApiRef, hKey, hKey2String(hKey), dwIndex, cchName);
    if (!IsFake(hKey)) {
        res = (*pRegEnumKeyA)(hKey, dwIndex, lpName, cchName);
        OutTraceR("%s: res=%#x name=%s\n",
                  ApiRef, res, (res == ERROR_SUCCESS) ? lpName : "(???)");
        return res;
    }
    // try emulated registry
    FILE *regf;
    char sUpKey[MAX_PATH];
    char sSubKey[MAX_PATH];
    UINT len;
    regf = OpenFakeRegistry();
    if(regf == NULL) return ERROR_FILE_NOT_FOUND;
    res = SeekFakeKey(regf, hKey - dwIndex);
    if(res == ERROR_SUCCESS) {
        strcpy(sUpKey, hKey2String(hKey));
        strcpy(sSubKey, hKey2String(hKey - dwIndex - 1));
        len = strlen(sUpKey);
        if(!strncmp(sSubKey, sUpKey, len)) {
            strncpy(lpName, sSubKey + len + 1, cchName);
            // to do: handling of lpClass and lpcClass when not NULL
            res = ERROR_SUCCESS;
            OutTraceR("%s: enum keynum=%d name=%s\n", ApiRef, dwIndex, lpName);
        } else
            res = ERROR_NO_MORE_ITEMS;
    }
    fclose(regf);
    OutTraceR("%s: res=%#x\n", ApiRef, res);
    return res;
}

LONG WINAPI extRegEnumKeyExA(HKEY hKey, DWORD dwIndex, LPTSTR lpName, LPDWORD lpcName, LPDWORD lpReserved,
                             LPTSTR lpClass, LPDWORD lpcClass, PFILETIME lpftLastWriteTime) {
    LONG res;
    ApiName("RegEnumKeyEx");
    OutTraceR("%s: hKey=%#x(\"%s\") index=%d cName=%d cClass=%d\n",
              ApiRef, hKey, hKey2String(hKey), dwIndex, lpcName ? *lpcName : 0, lpcClass ? *lpcClass : 0);
    if (!IsFake(hKey)) {
        res = (*pRegEnumKeyExA)(hKey, dwIndex, lpName, lpcName, lpReserved, lpClass, lpcClass, lpftLastWriteTime);
        if(res == ERROR_SUCCESS) {
            OutTraceR("%s: res=SUCCESS Name=%s cName=%d Class=%s cClass=%d\n",
                      ApiRef, lpName, lpcName ? *lpcName : 0, lpClass ? lpClass : "(NULL)", lpcClass ? *lpcClass : 0);
        } else
            OutTraceR("%s: res=%#x\n", ApiRef, res);
        return res;
    }
    // try emulated registry
    FILE *regf;
    char sUpKey[MAX_PATH];
    char sSubKey[MAX_PATH];
    UINT len;
    regf = OpenFakeRegistry();
    if(regf == NULL) return ERROR_FILE_NOT_FOUND;
    res = SeekFakeKey(regf, hKey - dwIndex);
    if(res == ERROR_SUCCESS) {
        strcpy(sUpKey, hKey2String(hKey));
        strcpy(sSubKey, hKey2String(hKey - dwIndex - 1));
        len = strlen(sUpKey);
        if(!strncmp(sSubKey, sUpKey, len)) {
            strncpy(lpName, sSubKey + len + 1, *lpcName);
            *lpcName = strlen(lpName);
            // to do: handling of lpClass and lpcClass when not NULL
            res = ERROR_SUCCESS;
            OutTraceR("%s: enum keynum=%d name=%s cName=%d\n", ApiRef, dwIndex, lpName, *lpcName);
        } else
            res = ERROR_NO_MORE_ITEMS;
    }
    fclose(regf);
    OutTraceR("%s: res=%#x\n", ApiRef, res);
    return res;
}

BOOL WINAPI extGetUserNameA(LPSTR lpBuffer, LPDWORD pcbBuffer) {
    BOOL res;
    ApiName("GetUserNameA");
    OutTraceR("%s: maxlen=%d\n", ApiRef, *pcbBuffer);
    res = (*pGetUserNameA)(lpBuffer, pcbBuffer);
    if(res)
        OutTraceR("%s: user=\"%s\" len=%d\n", ApiRef, lpBuffer, *pcbBuffer);
    else
        OutTraceE("%s: err=%d\n", ApiRef, GetLastError());
    return res;
}

#define IsQueried(p) ((p==NULL) ? "(NULL)": "Yes")
LONG WINAPI extRegQueryInfoKeyA(
    HKEY hKey,
    LPSTR lpClass,
    LPDWORD lpcchClass,
    LPDWORD lpReserved,
    LPDWORD lpcSubKeys,
    LPDWORD lpcbMaxSubKeyLen,
    LPDWORD lpcbMaxClassLen,
    LPDWORD lpcValues,
    LPDWORD lpcbMaxValueNameLen,
    LPDWORD lpcbMaxValueLen,
    LPDWORD lpcbSecurityDescriptor,
    PFILETIME lpftLastWriteTime) {
    // seen in "Fallen Haven"
    LONG res;
    ApiName("RegQueryInfoKeyA");
    OutTraceR("%s: hKey=%#x(\"%s\") cchClass=%d QUERY: Class=%s SubKeys=%s MaxSubKeyLen=%s MaxClassLen=%s "
              "Values=%s ValueNameLen=%s MaxValueLen=%s SecurityDescr=%s LastWriteTime=%s\n",
              ApiRef,
              hKey, hKey2String(hKey),
              lpcchClass ? *lpcchClass : 0,
              IsQueried(lpClass),
              IsQueried(lpcSubKeys),
              IsQueried(lpcbMaxSubKeyLen),
              IsQueried(lpcbMaxClassLen),
              IsQueried(lpcValues),
              IsQueried(lpcbMaxValueNameLen),
              IsQueried(lpcbMaxValueLen),
              IsQueried(lpcbSecurityDescriptor),
              IsQueried(lpftLastWriteTime)
             );
    if (!IsFake(hKey)) {
        res = (*pRegQueryInfoKeyA)(
                  hKey,
                  lpClass,
                  lpcchClass,
                  lpReserved,
                  lpcSubKeys,
                  lpcbMaxSubKeyLen,
                  lpcbMaxClassLen,
                  lpcValues,
                  lpcbMaxValueNameLen,
                  lpcbMaxValueLen,
                  lpcbSecurityDescriptor,
                  lpftLastWriteTime);
    } else {
        FILE *regf;
        regf = OpenFakeRegistry();
        if(regf == NULL) {
            OutTraceR("%s: error in OpenFakeRegistry err=%s\n", ApiRef, GetLastError());
            return ERROR_FILE_NOT_FOUND;
        }
        res = ERROR_FILE_NOT_FOUND;
        char RegBuf[MAX_PATH + 1];
        HKEY hCurKey = HKEY_FAKE + 1;
        fgets(RegBuf, 256, regf);
        while (!feof(regf)) {
            if(RegBuf[0] == '[')
                hCurKey--;
            if(hCurKey == hKey) {
                //OutTraceREGDebug("DEBUG: SeekFakeKey fount key at line=%s\n", RegBuf);
                res = ERROR_SUCCESS;
                if(lpcchClass) {
                    // No support for custom keys at the moment ...
                    //if(lpClass && (*lpcchClass > strlen(RegBuf))) strncpy(lpClass, RegBuf, *lpcchClass);
                    //*lpcchClass = strlen(RegBuf);
                    if(lpClass) *lpClass = 0;
                    if(lpcchClass) *lpcchClass = 0;
                }
                break;
            }
            fgets(RegBuf, 256, regf);
        }
        if(res == ERROR_SUCCESS) {
            int iValueNameLen, iMaxValueNameLen, iValues;
            if (lpcSubKeys) *lpcSubKeys = 0;
            if (lpcbMaxSubKeyLen) *lpcbMaxSubKeyLen = 0; // ???
            if (lpcbMaxClassLen) *lpcbMaxClassLen = 0; // ???
            if (lpcSubKeys) *lpcSubKeys = 0; // ???
            iValues = 0;
            iMaxValueNameLen = 0;
            fgets(RegBuf, 256, regf);
            while (!feof(regf)) {
                if (RegBuf[0] == '[') break;
                if(RegBuf[0] == '"') {
                    iValues++;
                    if (lpcbMaxValueNameLen) {
                        int i;
                        for(i = 1; RegBuf[i]; i++) if(RegBuf[i] == '"') break;
                        iValueNameLen = i - 1;
                        if(iValueNameLen > iMaxValueNameLen) iMaxValueNameLen = iValueNameLen; // save maximum value
                    }
                }
                fgets(RegBuf, 256, regf);
            }
            OutDebugR("> Values=%d\n", iValues);
            OutDebugR("> MaxValueNameLen=%d\n", iMaxValueNameLen);
            if (lpcValues) *lpcValues = iValues;
            if (lpcbMaxValueNameLen) *lpcbMaxValueNameLen = iMaxValueNameLen;
        }
    }
    if(res == ERROR_SUCCESS) {
        char sTime[81];
        SYSTEMTIME st;
        strcpy(sTime, "");
        if (lpftLastWriteTime) {
            if(FileTimeToSystemTime(lpftLastWriteTime, &st)) {
                sprintf(sTime, "%02.2d/%02.2d/%04.4d %02.2d:%02.2d:%02.2d",
                        st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
            }
        }
        OutTraceR("%s: res=SUCCESS hKey=%#x(\"%s\") "
                  "Class=%s cchClass=%d SubKeys=%d MaxSubKeyLen=%d "
                  "MaxClassLen=%d Values=%d MaxValueNameLen=%d SecDescr=%s LastWriteTime=%s\n",
                  ApiRef,
                  hKey, hKey2String(hKey),
                  lpClass ? lpClass : "(NULL)",
                  lpcchClass ? *lpcchClass : 0,
                  lpcSubKeys ? *lpcSubKeys : 0,
                  lpcbMaxSubKeyLen ? *lpcbMaxSubKeyLen : 0,
                  lpcbMaxClassLen ? *lpcbMaxClassLen : 0,
                  lpcValues ? *lpcValues : 0,
                  lpcbMaxValueNameLen ? *lpcbMaxValueNameLen : 0,
                  lpcbSecurityDescriptor ? "(NULL)" : "(tbd)",
                  lpftLastWriteTime ? sTime : "(NULL)"
                 );
    } else
        OutTraceR("%s: res=%#x err=%d\n", ApiRef, res, GetLastError());
    return res;
}

#ifdef TOBEDONE
LONG WINAPI RegQueryInfoKey(
    _In_        HKEY      hKey,
    _Out_opt_   LPTSTR    lpClass,
    _Inout_opt_ LPDWORD   lpcClass,
    _Reserved_  LPDWORD   lpReserved,
    _Out_opt_   LPDWORD   lpcSubKeys,
    _Out_opt_   LPDWORD   lpcMaxSubKeyLen,
    _Out_opt_   LPDWORD   lpcMaxClassLen,
    _Out_opt_   LPDWORD   lpcValues,
    _Out_opt_   LPDWORD   lpcMaxValueNameLen,
    _Out_opt_   LPDWORD   lpcMaxValueLen,
    _Out_opt_   LPDWORD   lpcbSecurityDescriptor,
    _Out_opt_   PFILETIME lpftLastWriteTime
);
#endif