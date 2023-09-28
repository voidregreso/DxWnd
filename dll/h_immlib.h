/*  ===================================================================
    type definitions
    ====================================================================*/

typedef BOOL	(WINAPI *ImmNotifyIME_Type)(HIMC, DWORD, DWORD, DWORD);
typedef HIMC	(WINAPI *ImmCreateContext_Type)(void);
typedef BOOL	(WINAPI *ImmDestroyContext_Type)(HIMC);
typedef BOOL	(WINAPI *ImmSetOpenStatus_Type)(HIMC, BOOL);
typedef HIMC	(WINAPI *ImmAssociateContext_Type)(HWND, HIMC);
typedef BOOL	(WINAPI *ImmSetCompositionWindow_Type)(HIMC, LPCOMPOSITIONFORM);
typedef BOOL	(WINAPI *ImmSetCompositionString_Type)(HIMC, DWORD, LPVOID, DWORD, LPVOID, DWORD);
typedef BOOL	(WINAPI *ImmGetOpenStatus_Type)(HIMC);

/*  ===================================================================
    hooked APIs real pointers
    ====================================================================*/

DXWEXTERN ImmNotifyIME_Type pImmNotifyIME DXWINITIALIZED;
DXWEXTERN ImmCreateContext_Type pImmCreateContext DXWINITIALIZED;
DXWEXTERN ImmDestroyContext_Type pImmDestroyContext DXWINITIALIZED;
DXWEXTERN ImmSetOpenStatus_Type pImmSetOpenStatus DXWINITIALIZED;
DXWEXTERN ImmAssociateContext_Type pImmAssociateContext DXWINITIALIZED;
DXWEXTERN ImmSetCompositionWindow_Type pImmSetCompositionWindow DXWINITIALIZED;
DXWEXTERN ImmSetCompositionString_Type pImmSetCompositionString DXWINITIALIZED;
DXWEXTERN ImmGetOpenStatus_Type pImmGetOpenStatus DXWINITIALIZED;

/*  ===================================================================
    hook procedures (possibly more than one per each API...)
    ====================================================================*/

extern BOOL WINAPI extImmNotifyIME(HIMC, DWORD, DWORD, DWORD);
extern HIMC	WINAPI extImmCreateContext(void);
extern BOOL	WINAPI extImmDestroyContext(HIMC);
extern BOOL	WINAPI extImmSetOpenStatus(HIMC, BOOL);
extern HIMC WINAPI extImmAssociateContext(HWND, HIMC);
extern BOOL WINAPI extImmSetCompositionWindow(HIMC, LPCOMPOSITIONFORM);
extern BOOL WINAPI extImmSetCompositionString(HIMC, DWORD, LPVOID, DWORD, LPVOID, DWORD);
extern BOOL WINAPI extImmGetOpenStatus(HIMC);

