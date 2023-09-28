//#define DXWNDEMULATE3DNOW

#ifdef DXWNDEMULATE3DNOW
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <psapi.h>
#include <dbghelp.h>
#include <smmintrin.h>
#include "dxwnd.h"

#define MMXOFFSET 32
#define MMXREGDUMP

static void PI2FD(BYTE *p, int cmdlen, PCONTEXT e) {
    int iReg1, iReg2;
    float *f = (float *) & (e->ExtendedRegisters[MMXOFFSET]);
    int *i = (int *) & (e->ExtendedRegisters[MMXOFFSET]);
    iReg1 = (*(p + 2) >> 3) & 0x7;
    iReg2 = (*(p + 2) >> 0) & 0x7;
#ifdef MMXREGDUMP
    OutTrace("> PI2FD MM%d, MM%d\n", iReg1, iReg2);
#endif
    f[4 * iReg1] = (float)i[4 * iReg2];
    f[4 * iReg1 + 1] = (float)i[4 * iReg2 + 1];
#ifdef MMXREGDUMP
    OutTrace("> PI2FD MM%d=%i, %i\n", iReg1, i[4 * iReg1], i[4 * iReg1 + 1]);
#endif
}

static void FP2ID(BYTE *p, int cmdlen, PCONTEXT e) {
    int iReg1, iReg2;
    float *f = (float *) & (e->ExtendedRegisters[MMXOFFSET]);
    int *i = (int *) & (e->ExtendedRegisters[MMXOFFSET]);
    iReg1 = (*(p + 2) >> 3) & 0x7;
    iReg2 = (*(p + 2) >> 0) & 0x7;
#ifdef MMXREGDUMP
    OutTrace("> FP2ID MM%d, MM%d\n", iReg1, iReg2);
#endif
    i[4 * iReg1] = (int)f[4 * iReg2];
    i[4 * iReg1 + 1] = (int)f[4 * iReg2 + 1];
#ifdef MMXREGDUMP
    OutTrace("> FP2ID MM%d=%i, %i\n", iReg1, i[4 * iReg1], i[4 * iReg1 + 1]);
#endif
}

static void FPADD(BYTE *p, int cmdlen, PCONTEXT e) {
    int iReg1, iReg2;
    float *f = (float *) & (e->ExtendedRegisters[MMXOFFSET]);
    iReg1 = (*(p + 2) >> 3) & 0x7;
    iReg2 = (*(p + 2) >> 0) & 0x7;
#ifdef MMXREGDUMP
    OutTrace("> FPADD MM%d=%g,%g, MM%d=%g,%g\n",
             iReg1, f[4 * iReg1], f[4 * iReg1 + 1],
             iReg2, f[4 * iReg2], f[4 * iReg2 + 1]);
#endif
    f[4 * iReg1] = f[4 * iReg1] + f[4 * iReg2];
    f[4 * iReg1 + 1] = f[4 * iReg1 + 1] + f[4 * iReg2 + 1];
#ifdef MMXREGDUMP
    OutTrace("> FPADD MM%d=%g, %g\n", iReg1, f[4 * iReg1], f[4 * iReg1 + 1]);
#endif
}

static void FPSUB(BYTE *p, int cmdlen, PCONTEXT e) {
    int iReg1, iReg2;
    float *f = (float *) & (e->ExtendedRegisters[MMXOFFSET]);
    iReg1 = (*(p + 2) >> 3) & 0x7;
    iReg2 = (*(p + 2) >> 0) & 0x7;
#ifdef MMXREGDUMP
    OutTrace("> FPSUB MM%d=%f,%f, MM%d=%f,%f\n",
             iReg1, f[4 * iReg1], f[4 * iReg1 + 1],
             iReg2, f[4 * iReg2], f[4 * iReg2 + 1]);
#endif
    f[4 * iReg1] = f[4 * iReg1] - f[4 * iReg2];
    f[4 * iReg1 + 1] = f[4 * iReg1 + 1] - f[4 * iReg2 + 1];
#ifdef MMXREGDUMP
    OutTrace("> FPSUB MM%d=%g, %g\n", iReg1, f[4 * iReg1], f[4 * iReg1 + 1]);
#endif
}

static void FPMIN(BYTE *p, int cmdlen, PCONTEXT e) {
    int iReg1, iReg2;
    float *f = (float *) & (e->ExtendedRegisters[MMXOFFSET]);
    iReg1 = (*(p + 2) >> 3) & 0x7;
    iReg2 = (*(p + 2) >> 0) & 0x7;
#ifdef MMXREGDUMP
    OutTrace("> FPMIN MM%d=%f,%f, MM%d=%f,%f\n",
             iReg1, f[4 * iReg1], f[4 * iReg1 + 1],
             iReg2, f[4 * iReg2], f[4 * iReg2 + 1]);
#endif
    f[4 * iReg1] = (f[4 * iReg1] < f[4 * iReg2]) ? f[4 * iReg1] : f[4 * iReg2];
    f[4 * iReg1 + 1] = (f[4 * iReg1 + 1] < f[4 * iReg2 + 1]) ? f[4 * iReg1 + 1] : f[4 * iReg2 + 1];
#ifdef MMXREGDUMP
    OutTrace("> FPMIN MM%d=%g, %g\n", iReg1, f[4 * iReg1], f[4 * iReg1 + 1]);
#endif
}

static void FPMUL(BYTE *p, int cmdlen, PCONTEXT e) {
    int iReg1, iReg2;
    float *f = (float *) & (e->ExtendedRegisters[MMXOFFSET]);
    iReg1 = (*(p + 2) >> 3) & 0x7;
    iReg2 = (*(p + 2) >> 0) & 0x7;
#ifdef MMXREGDUMP
    OutTrace("> FPMUL MM%d=%f,%f, MM%d=%f,%f\n",
             iReg1, f[4 * iReg1], f[4 * iReg1 + 1],
             iReg2, f[4 * iReg2], f[4 * iReg2 + 1]);
#endif
    f[4 * iReg1] = f[4 * iReg1] * f[4 * iReg2];
    f[4 * iReg1 + 1] = f[4 * iReg1 + 1] * f[4 * iReg2 + 1];
#ifdef MMXREGDUMP
    OutTrace("> FPMUL MM%d=%g, %g\n", iReg1, f[4 * iReg1], f[4 * iReg1 + 1]);
#endif
}

static void FPRCP(BYTE *p, int cmdlen, PCONTEXT e) {
    int iReg1, iReg2;
    float *f = (float *) & (e->ExtendedRegisters[MMXOFFSET]);
    iReg1 = (*(p + 2) >> 3) & 0x7;
    iReg2 = (*(p + 2) >> 0) & 0x7;
#ifdef MMXREGDUMP
    OutTrace("> FPRCP MM%d=%f,%f, MM%d=%f,%f\n",
             iReg1, f[4 * iReg1], f[4 * iReg1 + 1],
             iReg2, f[4 * iReg2], f[4 * iReg2 + 1]);
#endif
    f[4 * iReg1] = (float)1.0 / f[4 * iReg2];
    f[4 * iReg1 + 1] = (float)1.0 / f[4 * iReg2 + 1];
#ifdef MMXREGDUMP
    OutTrace("> FPRCP MM%d=%g, %g\n", iReg1, f[4 * iReg1], f[4 * iReg1 + 1]);
#endif
}

static void FPACC(BYTE *p, int cmdlen, PCONTEXT e) {
    int iReg1, iReg2;
    float *f = (float *) & (e->ExtendedRegisters[MMXOFFSET]);
    iReg1 = (*(p + 2) >> 3) & 0x7;
    iReg2 = (*(p + 2) >> 0) & 0x7;
#ifdef MMXREGDUMP
    OutTrace("> FPACC MM%d=%f,%f, MM%d=%f,%f\n",
             iReg1, f[4 * iReg1], f[4 * iReg1 + 1],
             iReg2, f[4 * iReg2], f[4 * iReg2 + 1]);
#endif
    // t.b.d.
#ifdef MMXREGDUMP
    OutTrace("> FPACC MM%d=%g, %g\n", iReg1, f[4 * iReg1], f[4 * iReg1 + 1]);
#endif
}

static void EMMS() {
    OutTrace("> EMMS\n");
    _asm {
        emms
    }
}

static void FEMMS() {
#ifdef MMXREGDUMP
    OutTrace("> FEMMS\n");
#endif
    //_asm {
    //	femms
    //}
}

void Emulate3DNow(BYTE *p, int cmdlen, PCONTEXT e) {
    BYTE t = *(p + cmdlen - 1);
    //OutTrace("DxWExceptionHandler: mmcontext[ 0] = %s\n", hexdump(&e->ExtendedRegisters[0], 32));
    //OutTrace("DxWExceptionHandler: mmcontext[32] = %s\n", hexdump(&e->ExtendedRegisters[32], 32));
    //OutTrace("DxWExceptionHandler: mmcontext[64] = %s\n", hexdump(&e->ExtendedRegisters[64], 32));
    float *f = (float *) & (e->ExtendedRegisters[MMXOFFSET]);
#ifdef MMXREGDUMP
    for (int i = 0; i < 8; i++)
        OutTrace("MM%d=%f, %f\n", i, f[4 * i], f[4 * i + 1]);
#endif
    switch(t) {
    case 0x0D:
        PI2FD(p, cmdlen, e);
        break;
    case 0x0E:
        FEMMS();
        break;
    case 0x1D:
        FP2ID(p, cmdlen, e);
        break;
    case 0x77:
        EMMS();
        break;
    case 0x94:
        FPMIN(p, cmdlen, e);
        break;
    case 0x96:
        FPRCP(p, cmdlen, e);
        break;
    case 0x9A:
        FPSUB(p, cmdlen, e);
        break;
    case 0x9E:
        FPADD(p, cmdlen, e);
        break;
    case 0xAE:
        FPACC(p, cmdlen, e);
        break;
    case 0xB4:
        FPMUL(p, cmdlen, e);
        break;
    default:
        OutTrace("DxWExceptionHandler: opcode=unknown\n");
        break;
    }
#ifdef MMXREGDUMP
    for (int i = 0; i < 8; i++)
        OutTrace("MM%d=%f, %f\n", i, f[4 * i], f[4 * i + 1]);
#endif
    __m64 *qMM0, *qMM1, *qMM2, *qMM3, *qMM4, *qMM5, *qMM6, *qMM7;
    qMM0 = (__m64 *)&f[ 0];
    qMM1 = (__m64 *)&f[ 4];
    qMM2 = (__m64 *)&f[ 8];
    qMM3 = (__m64 *)&f[12];
    qMM4 = (__m64 *)&f[16];
    qMM5 = (__m64 *)&f[18];
    qMM6 = (__m64 *)&f[20];
    qMM7 = (__m64 *)&f[24];
    _asm {
        movq QWORD PTR qMM0, mm0
        movq QWORD PTR qMM1, mm1
        movq QWORD PTR qMM2, mm2
        movq QWORD PTR qMM3, mm3
        movq QWORD PTR qMM4, mm4
        movq QWORD PTR qMM5, mm5
        movq QWORD PTR qMM6, mm6
        movq QWORD PTR qMM7, mm7
        ;
        emms
    }
#if 0
    float f1, f2;
    _asm {
        movq mm0, QWORD PTR f1
        movq mm1, QWORD PTR f2
    }
    OutTrace(">> test: (%f, %f)\n", f1, f2);
#endif
}
#endif