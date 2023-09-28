/*  ===================================================================
    type definitions
    ====================================================================*/

typedef LONG	(WINAPI *RegCloseKey_Type)(HKEY);
typedef LONG	(WINAPI *RegCreateKey_Type)(HKEY, LPCTSTR, PHKEY);
typedef LONG	(WINAPI *RegCreateKeyEx_Type)(HKEY, LPCTSTR, DWORD, LPTSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
typedef LONG	(WINAPI *RegOpenKey_Type)(HKEY, LPCTSTR, PHKEY);
typedef LONG	(WINAPI *RegOpenKeyEx_Type)(HKEY, LPCTSTR, DWORD, REGSAM, PHKEY);
typedef LONG	(WINAPI *RegQueryValue_Type)(HKEY, LPCTSTR, LPTSTR, PLONG);
typedef LONG	(WINAPI *RegQueryValueEx_Type)(HKEY, LPCTSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
typedef LONG	(WINAPI *RegSetValueEx_Type)(HKEY, LPCTSTR, DWORD, DWORD, const BYTE *, DWORD);
typedef LONG	(WINAPI *RegFlushKey_Type)(HKEY);
typedef LONG	(WINAPI *RegEnumValueA_Type)(HKEY, DWORD, LPTSTR, LPDWORD, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
typedef LONG	(WINAPI *RegEnumKeyA_Type)(HKEY, DWORD, LPTSTR, DWORD);
typedef LONG	(WINAPI *RegEnumKeyExA_Type)(HKEY, DWORD, LPTSTR, LPDWORD, LPDWORD, LPTSTR, LPDWORD, PFILETIME);
typedef LONG	(WINAPI *RegQueryInfoKeyA_Type)(HKEY, LPSTR, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, PFILETIME);

/*  ===================================================================
    hooked APIs real pointers
    ====================================================================*/

DXWEXTERN RegCloseKey_Type pRegCloseKey DXWINITIALIZED;
DXWEXTERN RegCreateKey_Type pRegCreateKey DXWINITIALIZED;
DXWEXTERN RegCreateKeyEx_Type pRegCreateKeyEx DXWINITIALIZED;
DXWEXTERN RegOpenKey_Type pRegOpenKey DXWINITIALIZED;
DXWEXTERN RegOpenKeyEx_Type pRegOpenKeyEx DXWINITIALIZED;
DXWEXTERN RegQueryValue_Type pRegQueryValue DXWINITIALIZED;
DXWEXTERN RegQueryValueEx_Type pRegQueryValueEx DXWINITIALIZED;
DXWEXTERN RegSetValueEx_Type pRegSetValueEx DXWINITIALIZED;
DXWEXTERN RegFlushKey_Type pRegFlushKey;
DXWEXTERN RegEnumValueA_Type pRegEnumValueA;
DXWEXTERN RegEnumKeyA_Type pRegEnumKeyA;
DXWEXTERN RegEnumKeyExA_Type pRegEnumKeyExA;
DXWEXTERN RegQueryInfoKeyA_Type pRegQueryInfoKeyA;

/*  ===================================================================
    hook procedures (possibly more than one per each API...)
    ====================================================================*/

extern LONG	WINAPI extRegCloseKey(HKEY);
extern LONG	WINAPI extRegCreateKey(HKEY, LPCTSTR, PHKEY);
extern LONG	WINAPI extRegCreateKeyEx(HKEY, LPCTSTR, DWORD, LPTSTR, DWORD, REGSAM, LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
extern LONG	WINAPI extRegOpenKey(HKEY, LPCTSTR, PHKEY);
extern LONG	WINAPI extRegOpenKeyEx(HKEY, LPCTSTR, DWORD, REGSAM, PHKEY);
extern LONG	WINAPI extRegQueryValue(HKEY, LPCTSTR, LPTSTR, PLONG);
extern LONG	WINAPI extRegQueryValueEx(HKEY, LPCTSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
extern LONG	WINAPI extRegSetValueEx(HKEY, LPCTSTR, DWORD, DWORD, const BYTE *, DWORD);
extern LONG WINAPI extRegFlushKey(HKEY);
extern LONG WINAPI extRegEnumValueA(HKEY, DWORD, LPTSTR, LPDWORD, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
extern LONG WINAPI extRegEnumKeyA(HKEY, DWORD, LPTSTR, DWORD);
extern LONG WINAPI extRegEnumKeyExA(HKEY, DWORD, LPTSTR, LPDWORD, LPDWORD, LPTSTR, LPDWORD, PFILETIME);
extern LONG WINAPI extRegQueryInfoKeyA(HKEY, LPSTR, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, PFILETIME);

