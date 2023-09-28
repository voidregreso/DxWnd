/*  ===================================================================
    type definitions
    ====================================================================*/

typedef HRESULT (STDAPICALLTYPE *CoCreateInstance_Type)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID FAR *);
typedef HRESULT (STDAPICALLTYPE *CoCreateInstanceEx_Type)(REFCLSID, IUnknown *, DWORD, COSERVERINFO *, DWORD, MULTI_QI *);
typedef HRESULT (STDAPICALLTYPE *CoInitialize_Type)(LPVOID);
typedef void (STDAPICALLTYPE *CoUninitialize_Type)(void);

/*  ===================================================================
    hooked APIs real pointers
    ====================================================================*/

DXWEXTERN CoCreateInstance_Type pCoCreateInstance DXWINITIALIZED;
DXWEXTERN CoCreateInstanceEx_Type pCoCreateInstanceEx DXWINITIALIZED;
DXWEXTERN CoInitialize_Type pCoInitialize DXWINITIALIZED;
DXWEXTERN CoUninitialize_Type pCoUninitialize DXWINITIALIZED;
DXWEXTERN CoInitialize_Type pOleInitialize DXWINITIALIZED;

/*  ===================================================================
    hook procedures (possibly more than one per each API...)
    ====================================================================*/

extern HRESULT STDAPICALLTYPE extCoCreateInstance(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID FAR *);
extern HRESULT STDAPICALLTYPE extCoCreateInstanceEx(REFCLSID, IUnknown *, DWORD, COSERVERINFO *, DWORD, MULTI_QI *);
extern HRESULT STDAPICALLTYPE extCoInitialize(LPVOID);
extern HRESULT STDAPICALLTYPE extOleInitialize(LPVOID);
extern void STDAPICALLTYPE extCoUninitialize(void);
