#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_DEPRECATE 1

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <psapi.h>
#include <dbghelp.h>
#include "dxwnd.h"
#include "dxwcore.hpp"
#include "disasm.h"
#include "dwdisasm.h"

//#define DXWNDEMULATE3DNOW
#define DXW_MAX_CONSECUTIVE_EXCEPTIONS 20
#define HANDLEVGAPALETTE TRUE
#define HANDLEHWSYNCWAIT TRUE

LONG WINAPI DxWExceptionHandler(LPEXCEPTION_POINTERS ExceptionInfo) {
    static HMODULE disasmlib = NULL;
    static int iExceptionCounter = 0;
    static VOID *pNext1 = 0;
    static VOID *pNext2 = 0;
    static DWORD iStartPaletteIndex = 0;
    extern void mySetPalette(int, int, LPPALETTEENTRY);
    static BYTE VGAPalette[256 * 3]; // emulates the content of VGA palette data
    BOOL ret;
    OutTraceSYS("DxWExceptionHandler: exception code=%#x flags=%#x addr=%#x\n",
                ExceptionInfo->ExceptionRecord->ExceptionCode,
                ExceptionInfo->ExceptionRecord->ExceptionFlags,
                ExceptionInfo->ExceptionRecord->ExceptionAddress);
    DWORD oldprot;
    PVOID target = ExceptionInfo->ExceptionRecord->ExceptionAddress;
    int cmdlen;
    t_disasm da;
    if(!disasmlib) {
        if (!(disasmlib = LoadDisasm())) return EXCEPTION_CONTINUE_SEARCH;
        (*pPreparedisasm)();
    }
    if(ExceptionInfo->ExceptionRecord->ExceptionCode <= 0x80000000) {
        // from https://stackoverflow.com/questions/12298406/how-to-treat-0x40010006-exception-in-vectored-exception-handler
        // Exception codes with values less than 0x80000000 are just informal and never an indicator
        // of real trouble. In general, you should never mess with exceptions that you don't recognize
        // and don't want to explicitly handle. Let Windows continue searching for a handler by
        // returning EXCEPTION_CONTINUE_SEARCH, the debugger will probably catch it in this case.
        OutTraceSYS("DxWExceptionHandler: EXCEPTION_CONTINUE_SEARCH\n");
        return EXCEPTION_CONTINUE_SEARCH;
    }
    switch(ExceptionInfo->ExceptionRecord->ExceptionCode) {
    case 0x80000003: // INT3 debugging exception, ignore it
    case 0xc0000008: // invalid handle - Better ignore. Ref. "Dungeon Keeper, Deeper Dungeons D3D"
    case 0xe06d7363: // v2.05.05: from MSDN: The Visual C++ compiler uses exception code 0xE06D7363 for C++ exceptions.
        // https://blogs.msdn.microsoft.com/oldnewthing/20100730-00/?p=13273
        // fixes "Rage of Mages - Allods" with "Handle exceptions" flag.
        return EXCEPTION_CONTINUE_SEARCH;
        break;
    case 0xc0000096: // CLI Priviliged instruction exception (Resident Evil), FB (Asterix & Obelix)
        // also OUT & REP instructions to write to output ports, used by legacy DOS to
        // write palette data ("Dark Judgement").
        // To do: possible usage of different registers, handling of D flag set
        if(HANDLEVGAPALETTE) {
            //Command:  REP OUTS DX,BYTE PTR DS:[ESI]
            //Hex dump: F3:6E
            //
            //Copies ECX words from the memory to the hardware port.
            //
            //On each iteration, processor reads byte from the memory at address [ESI]
            //and sends it to the port addressed by the contents of the 16-bit register
            //DX. If flag D is cleared, increments ESI by 2, otherwise decrements ESI by
            //2. Register ECX is always decremented by 1. If ECX after decrement is
            //zero, operation stops; otherwise, processor repeats the whole cycle again
            //and again, until count exhausts.
            BOOL bHandled = FALSE;
            if(!VirtualProtect(target, 10, PAGE_READWRITE, &oldprot)) return EXCEPTION_CONTINUE_SEARCH; // error condition
            cmdlen = (*pDisasm)((BYTE *)target, 10, 0, &da, 0, NULL, NULL);
            if((cmdlen == 2) && (*(BYTE *)target == 0xF3) && (ExceptionInfo->ContextRecord->Edx == 0x3C9)) {
                bHandled = TRUE;
                DWORD iCount = ExceptionInfo->ContextRecord->Ecx % 0x300; // how many bytes of data to be transferred
                ExceptionInfo->ContextRecord->Ecx = 0; // clear ECX
                if(iCount == 0) iCount = 768; // ref. ...
                LPBYTE lpESI = (LPBYTE)ExceptionInfo->ContextRecord->Esi;
                ExceptionInfo->ContextRecord->Esi += iCount; // increment ESI
                OutTrace("DxWExceptionHandler: DAC WRITE count=%#x(#%d)\n", iCount, iCount);
                OutHexDW(lpESI, iCount);
                if(iCount + iStartPaletteIndex <= 0x300)   // bounds protection
                    memcpy(VGAPalette + (iStartPaletteIndex), lpESI, iCount);
                BYTE pal[256 * 4];
                LPBYTE q = pal;
                LPBYTE p = VGAPalette;
                for (int i = 0; i < 256; i++) {
                    *q++ = ((*p++) << 2);
                    *q++ = ((*p++) << 2);
                    *q++ = ((*p++) << 2);
                    *q++ = 0;
                }
                mySetPalette(0, 256, (LPPALETTEENTRY)pal);
            } else if((cmdlen == 1) && (*(BYTE *)target == 0xEE) && (ExceptionInfo->ContextRecord->Edx == 0x3C8)) {
                //Command:  OUT DX,AL
                //Hex dump: EE
                //
                //Writes contents of 8-bit register AL to the I/O port DX. Flags are not
                //affected.
                // n.b. AL = EAX & 0xFF
                bHandled = TRUE;
                iStartPaletteIndex = (ExceptionInfo->ContextRecord->Eax & 0x000000FF) * 3;
                OutTrace("DxWExceptionHandler: DAC WRITE addr=%#x(#%d)\n", iStartPaletteIndex, iStartPaletteIndex);
            }
            // recover previous protections, do not NOP assembly
            VirtualProtect(target, 10, oldprot, &oldprot);
            ret = FlushInstructionCache(GetCurrentProcess(), target, cmdlen);
            _if(!ret) OutTrace("DxWExceptionHandler: FlushInstructionCache ERROR target=%#x, err=%#x\n", target, GetLastError());
            if(bHandled) {
                // if handle DAC WRITE operation, skip offending assembly
                ExceptionInfo->ContextRecord->Eip += cmdlen; // skip ahead op-code length
                return EXCEPTION_CONTINUE_EXECUTION;
            }
        }
        if(HANDLEHWSYNCWAIT) {
            // this pattern:
            // start: IN AL,DX ; I/O command
            // TEST AL,08
            // JNZ SHORT start or JZ SHORT start
            // is often used to sample the video card status and wait for a vSync status.
            // since the JNZ block is followed by a JZ block removing the IN instruction is not
            // enough to avoid an endless loop. You have to clear all 3 instructions.
            if(!VirtualProtect(target, 10, PAGE_READWRITE, &oldprot)) return EXCEPTION_CONTINUE_SEARCH; // error condition
            cmdlen = (*pDisasm)((BYTE *)target, 10, 0, &da, 0, NULL, NULL);
            if((cmdlen == 1) && (*(BYTE *)target == 0xEC) && (ExceptionInfo->ContextRecord->Edx == 0x3DA)) {
                LPBYTE p = (LPBYTE)target;
                if( (p[1] == 0xA8) &&
                        (p[2] == 0x08) && // A8 08 => TEST AL, 08
                        ((p[3] == 0x74) || (p[3] == 0x75)) && // 74 => JZ; 75 => JNZ
                        (p[4] == 0xFB)) { // FB => SHORT (-4)
                    memset((BYTE *)target, 0x90, 5);
                    VirtualProtect(target, 10, oldprot, &oldprot);
                    ret = FlushInstructionCache(GetCurrentProcess(), target, cmdlen);
                    _if(!ret) OutTrace("DxWExceptionHandler: FlushInstructionCache ERROR target=%#x, err=%#x\n", target, GetLastError());
                    // v2.03.10 skip replaced opcode
                    ExceptionInfo->ContextRecord->Eip += cmdlen; // skip ahead op-code length
                    return EXCEPTION_CONTINUE_EXECUTION;
                }
            }
        }
    // else fall-through ....
    case 0xc0000094: // IDIV reg (Ultim@te Race Pro)
    case 0xc0000095: // DIV by 0 (divide overflow) exception (SonicR)
#ifndef DXWNDEMULATE3DNOW
    case 0xc000001d: // 3DNow! instructions: FEMMS (eXpendable), FPADD etc. (Arthur's Knights I & II, the New Adventures of the Time Machine)
#endif
        //case 0xc0000005: // Memory exception (Tie Fighter) -- moved to SKIP processing
        if(!VirtualProtect(target, 10, PAGE_READWRITE, &oldprot)) return EXCEPTION_CONTINUE_SEARCH; // error condition
        cmdlen = (*pDisasm)((BYTE *)target, 10, 0, &da, 0, NULL, NULL);
        OutTrace("DxWExceptionHandler: NOP opcode=%s len=%d\n", hexdump((BYTE *)target, cmdlen), cmdlen);
        memset((BYTE *)target, 0x90, cmdlen);
        VirtualProtect(target, 10, oldprot, &oldprot);
        ret = FlushInstructionCache(GetCurrentProcess(), target, cmdlen);
        _if(!ret) OutTrace("DxWExceptionHandler: FlushInstructionCache ERROR target=%#x, err=%#x\n", target, GetLastError());
        // v2.03.10 skip replaced opcode
        ExceptionInfo->ContextRecord->Eip += cmdlen; // skip ahead op-code length
        return EXCEPTION_CONTINUE_EXECUTION;
        break;
#ifdef DXWNDEMULATE3DNOW
    case 0xc000001d: // 3DNOW instructions
        if(!VirtualProtect(target, 10, PAGE_READWRITE, &oldprot)) return EXCEPTION_CONTINUE_SEARCH; // error condition
        cmdlen = (*pDisasm)((BYTE *)target, 10, 0, &da, 0, NULL, NULL);
        if(*(BYTE *)target == 0x0F) {
            OutTrace("DxWExceptionHandler: EMULATE opcode=%s len=%d\n", hexdump((BYTE *)target, cmdlen), cmdlen);
            extern void Emulate3DNow(BYTE *, int, PCONTEXT);
            Emulate3DNow((BYTE *)target, cmdlen, ExceptionInfo->ContextRecord);
        } else {
            OutTrace("DxWExceptionHandler: NOP opcode=%s len=%d\n", hexdump((BYTE *)target, cmdlen), cmdlen);
            memset((BYTE *)target, 0x90, cmdlen);
        }
        VirtualProtect(target, 10, oldprot, &oldprot);
        if(!FlushInstructionCache(GetCurrentProcess(), target, cmdlen))
            OutTrace("DxWExceptionHandler: FlushInstructionCache ERROR target=%#x, err=%#x\n", target, GetLastError());
        // v2.03.10 skip replaced opcode
        ExceptionInfo->ContextRecord->Eip += cmdlen; // skip ahead op-code length
        return EXCEPTION_CONTINUE_EXECUTION;
        break;
#endif
    default:
        if(!VirtualProtect(target, 10, PAGE_READONLY, &oldprot)) return EXCEPTION_CONTINUE_SEARCH; // error condition
        cmdlen = (*pDisasm)((BYTE *)target, 10, 0, &da, 0, NULL, NULL);
        OutTrace("DxWExceptionHandler: SKIP opcode=%s len=%d\n", hexdump((BYTE *)target, cmdlen), cmdlen); // v2.05.28 better logging
        ExceptionInfo->ContextRecord->Eip += cmdlen; // skip ahead op-code length
        return EXCEPTION_CONTINUE_EXECUTION;
        break;
    }
    if((target == pNext1) || (target == pNext2)) {
        iExceptionCounter ++;
        if(iExceptionCounter > DXW_MAX_CONSECUTIVE_EXCEPTIONS) {
            OutTrace("DxWExceptionHandler: MAX consecutive exceptions reached\n");
            exit(-1);
        } else
            iExceptionCounter = 0;
    }
    pNext1 = target;
    pNext2 = (PBYTE)target + cmdlen;
}

