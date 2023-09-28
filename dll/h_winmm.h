/*  ===================================================================
    type definitions
    ====================================================================*/

typedef MCIERROR(WINAPI *mciSendCommand_Type)(MCIDEVICEID, UINT, DWORD_PTR, DWORD_PTR);
typedef MCIERROR(WINAPI *mciSendStringA_Type)(LPCTSTR, LPTSTR, UINT, HANDLE);
typedef MCIERROR(WINAPI *mciSendStringW_Type)(LPCWSTR, LPWSTR, UINT, HANDLE);
typedef DWORD	(WINAPI *timeGetTime_Type)(void);
typedef MMRESULT(WINAPI *timeKillEvent_Type)(UINT);
typedef MMRESULT(WINAPI *timeSetEvent_Type)(UINT, UINT, LPTIMECALLBACK, DWORD_PTR, UINT);

/*  ===================================================================
    hooked APIs real pointers
    ====================================================================*/

DXWEXTERN mciSendCommand_Type pmciSendCommandA DXWINITIALIZED;
DXWEXTERN mciSendCommand_Type pmciSendCommandW DXWINITIALIZED;
DXWEXTERN mciSendStringA_Type pmciSendStringA DXWINITIALIZED;
DXWEXTERN mciSendStringW_Type pmciSendStringW DXWINITIALIZED;
DXWEXTERN timeGetTime_Type ptimeGetTime DXWINITIALIZED;
DXWEXTERN timeKillEvent_Type ptimeKillEvent DXWINITIALIZED;
DXWEXTERN timeSetEvent_Type ptimeSetEvent DXWINITIALIZED;

/*  ===================================================================
    hook procedures (possibly more than one per each API...)
    ====================================================================*/

extern MCIERROR WINAPI extmciSendCommandA(MCIDEVICEID, UINT, DWORD_PTR, DWORD_PTR);
extern MCIERROR WINAPI extmciSendCommandW(MCIDEVICEID, UINT, DWORD_PTR, DWORD_PTR);
extern MCIERROR WINAPI extmciSendStringW(LPCWSTR, LPWSTR, UINT, HANDLE);
extern MCIERROR WINAPI extmciSendStringA(LPCTSTR, LPSTR, UINT, HANDLE);
extern DWORD WINAPI exttimeGetTime(void);
extern MMRESULT WINAPI exttimeSetEvent(UINT, UINT, LPTIMECALLBACK, DWORD_PTR, UINT);
extern MMRESULT WINAPI exttimeKillEvent(UINT);
