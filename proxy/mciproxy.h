
extern char *ExplainMCICommands(DWORD);
extern char *ExplainMCIFlags(DWORD, DWORD);

extern void OutTrace(const char *, ...);

#define OutDebugSND OutTrace
#define OutTraceSND OutTrace
#define OutTraceDW  OutTrace
#define OutTraceE   OutTrace
#define IsDebugSND TRUE
#define IsTraceSND TRUE
#define IsTraceDW TRUE

typedef MCIERROR(WINAPI *mciSendCommand_Type)(MCIDEVICEID, UINT, DWORD_PTR, DWORD_PTR);
typedef MCIERROR(WINAPI *mciSendStringA_Type)(LPCTSTR, LPTSTR, UINT, HANDLE);
typedef MCIERROR(WINAPI *mciSendStringW_Type)(LPCWSTR, LPWSTR, UINT, HANDLE);

extern mciSendCommand_Type pmciSendCommandA;
extern mciSendCommand_Type pmciSendCommandW;
extern mciSendStringA_Type pmciSendStringA;
extern mciSendStringW_Type pmciSendStringW;

