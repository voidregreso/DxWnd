void LogSurfaceAttributes(FILE *, LPDDSURFACEDESC);
char *ExplainDDError(DWORD);
void SetHook(void *, void *, void **, char *);
char *sGUID(GUID *);
int GUIDSurfaceVersion(GUID *, char *);
int GUIDDDrawVersion(GUID *, char *);

extern FILE *TraceLog;
extern void trace(const char *, ...);

extern void HookDDSession(LPDIRECTDRAW *, int);
extern void HookSurface(LPDIRECTDRAWSURFACE *, int);
extern void HookDDClipper(LPDIRECTDRAWCLIPPER *);
extern void HookDDPalette(LPDIRECTDRAWPALETTE *);
extern void HookDDColorControl(LPDIRECTDRAWCOLORCONTROL *);
extern void DumpRefS(BOOL, int, LPDIRECTDRAWSURFACE);

#define dumprefi(lpdds) DumpRefS(TRUE,  dxversion, (LPDIRECTDRAWSURFACE)lpdds)
#define dumpreff(lpdds) DumpRefS(FALSE, dxversion, (LPDIRECTDRAWSURFACE)lpdds)

extern BOOL	bLogAll;
extern BOOL	bLogAttach;
extern BOOL	bLogBlits;
extern BOOL	bLogRefCount;
extern BOOL	bLogRelease;
extern BOOL	bLogQueryInterface;
