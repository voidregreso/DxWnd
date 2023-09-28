/*  ===================================================================

    syslibs.h: contains typedef definition and extern pointer declarations
    for all system calls hooked by DxWnd:
    GDI32.dll
    Kernel32.dll
    ole32.dll
    user32.dll

    ====================================================================*/

#include "Mmsystem.h"

#ifdef DXWDECLARATIONS
#undef DXWEXTERN
#undef DXWINITIALIZED
#define DXWEXTERN
//#define DXWINITIALIZED = NULL
#define DXWINITIALIZED
#else
#undef DXWEXTERN
#undef DXWINITIALIZED
#define DXWEXTERN extern
#define DXWINITIALIZED
#endif

#include "h_user32.h"
#include "h_advapi32.h"
#include "h_immlib.h"
#include "h_gdi32.h"
#include "h_kernel32.h"
#include "h_ole32.h"
#include "h_winmm.h"

/* eof */








