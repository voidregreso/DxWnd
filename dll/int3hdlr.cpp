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

//#define USEPERFCOUNTERS

//#undef OutTraceDW
//#define OutTraceDW OutTrace

static void LogCPUFeatures(DWORD dwEAX, DWORD dwEBX, DWORD dwECX, DWORD dwEDX) {
    OutTraceDW("Int3Handler: [EAX] SteppingId=%#x Model=%#x Family=%#x ProcType=%#x ExtModel=%#x ExtFamily=%#x\n",
               (dwEAX >> 0) & 0xF,
               (dwEAX >> 4) & 0xF,
               (dwEAX >> 8) & 0xF,
               (dwEAX >> 12) & 0x3,
               (dwEAX >> 16) & 0xF,
               (dwEAX >> 20) & 0xF);
    OutTraceDW("Int3Handler: [EBX] BrandIndex=%#x CLFLUSHLineSize=%#x InitialAPICID=%#x\n",
               (dwEBX >> 0) & 0xFF,
               (dwEBX >> 8) & 0xFF,
               (dwEBX >> 24) & 0xFF);
    OutTraceDW("Int3Handler: [ECX] SSE3=%#x Monitor=%#x DS-CPL=%#x EST=%#x TM2=%#x CNTX-ID=%#x\n",
               (dwECX >> 0) & 0x01,
               (dwECX >> 3) & 0x01,
               (dwECX >> 4) & 0x01,
               (dwECX >> 7) & 0x01,
               (dwECX >> 8) & 0x01,
               (dwECX >> 10) & 0x01);
    OutTraceDW("Int3Handler: [EDX] FPU=%#x VME=%#x DE=%#x PSE=%#x TSC=%#x MSR=%#x PAE=%#x MCE=%#x "
               "CX8=%#x APIC=%#x SEP=%#x PGE=%#x MCA=%#x CMOV=%#x PAT=%#x PSE-36=%#x PSN=%#x CFLSH=%#x DS=%#x "
               "ACPI=%#x MMX=%#x FXSR=%#x SSE=%#x SSE2=%#x SS=%#x HTT=%#x TM=%#x PBE=%#x\n",
               (dwEDX >> 0) & 0x01,
               (dwEDX >> 1) & 0x01,
               (dwEDX >> 2) & 0x01,
               (dwEDX >> 3) & 0x01,
               (dwEDX >> 4) & 0x01,
               (dwEDX >> 5) & 0x01,
               (dwEDX >> 6) & 0x01,
               (dwEDX >> 7) & 0x01,
               (dwEDX >> 8) & 0x01,
               (dwEDX >> 9) & 0x01,
               (dwEDX >> 11) & 0x03, // ???
               (dwEDX >> 13) & 0x01,
               (dwEDX >> 14) & 0x01,
               (dwEDX >> 15) & 0x01,
               (dwEDX >> 16) & 0x01,
               (dwEDX >> 17) & 0x01,
               (dwEDX >> 18) & 0x01,
               (dwEDX >> 19) & 0x03, // ???
               (dwEDX >> 21) & 0x01,
               (dwEDX >> 22) & 0x01,
               (dwEDX >> 23) & 0x01,
               (dwEDX >> 24) & 0x01,
               (dwEDX >> 25) & 0x01,
               (dwEDX >> 26) & 0x01,
               (dwEDX >> 27) & 0x01,
               (dwEDX >> 28) & 0x01,
               (dwEDX >> 29) & 0x01,
               (dwEDX >> 31) & 0x01);
}

static void LogCPUExtendedFeatures(DWORD dwEAX, DWORD dwEBX, DWORD dwECX, DWORD dwEDX) {
    OutTraceDW("Int3Handler: [EDX] mmext=%#x mmx=%#x\n",
               (dwEDX >> 22) & 0x01,
               (dwEDX >> 23) & 0x01);
}

LONG CALLBACK Int3Handler(PEXCEPTION_POINTERS ExceptionInfo) {
    // Vectored Handler routine to intercept INT3 opcodes replacing RDTSC or CPUID
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == 0x80000003) {
        LARGE_INTEGER myPerfCount;
        switch(*(BYTE *)(ExceptionInfo->ContextRecord->Eip + 1)) {
        case 0x90: { // Replace RTDSC
#ifdef USEPERFCOUNTERS
            if(!pQueryPerformanceCounter) pQueryPerformanceCounter = QueryPerformanceCounter;
            (*pQueryPerformanceCounter)(&myPerfCount);
#else
            __asm {
                ;
                Flush the pipeline
                XOR eax, eax
                CPUID
                ;
                Get RDTSC counter in edx:eax
                RDTSC
                mov myPerfCount.LowPart, eax
                mov myPerfCount.HighPart, edx
            }
#endif
            //myPerfCount = dxw.StretchCounter(myPerfCount);
            myPerfCount = dxw.StretchLargeCounter(myPerfCount);
            OutTraceDW("Int3Handler: INT3 at=%#x tick=%#x RDTSC=%#x:%#x\n",
                       ExceptionInfo->ExceptionRecord->ExceptionAddress, (*pGetTickCount)(), myPerfCount.HighPart, myPerfCount.LowPart);
            ExceptionInfo->ContextRecord->Edx = myPerfCount.HighPart;
            ExceptionInfo->ContextRecord->Eax = myPerfCount.LowPart;
            ExceptionInfo->ContextRecord->Eip++; // skip ahead one-byte ( jump over 0xCC op-code )
            ExceptionInfo->ContextRecord->Eip++; // skip ahead one-byte ( jump over 0x90 op-code )
            return EXCEPTION_CONTINUE_EXECUTION;
        }
        break;
        case 0xCC: { // Replace CPUID
            DWORD dwEAX0, dwEAX, dwEBX, dwECX, dwEDX;
            dwEAX0 = ExceptionInfo->ContextRecord->Eax;
            OutTraceDW("Int3Handler: CPUID at=%#x eax=%#x\n", ExceptionInfo->ExceptionRecord->ExceptionAddress, dwEAX0);
            __asm {
                ;
                Flush the pipeline
                XOR eax, eax
                mov eax, dwEAX0
                CPUID
                mov dwEAX, eax
                mov dwEBX, ebx
                mov dwECX, ecx
                mov dwEDX, edx
            }
            OutTraceDW("Int3Handler: eax=%#x ebx=%#x ecx=%#x edx=%#x\n", dwEAX, dwEBX, dwECX, dwEDX);
            switch(dwEAX0) {
            case 0x00000000: // CPU Vendor-Id
                OutTraceDW("Int3Handler: vendor=\"%04.4s%04.4s%04.4s\" MaxCPUIDInput=%d\n",
                           (char *)&dwEBX, (char *)&dwEDX, (char *)&dwECX, dwEAX);
                if(dxw.dwDFlags & DUMPCPUID) break;
                switch (dxw.dwDFlags & CPUVENDORMASK) {
                case CPUFAKEDXWNDCPU:
                    dwEBX = *(DWORD *)"Fake";
                    dwEDX = *(DWORD *)"DxWn";
                    dwECX = *(DWORD *)"dCPU";
                    dwEAX = 0;
                    break;
                case CPUGENUINEINTEL:
                    dwEBX = *(DWORD *)"Genu";
                    dwEDX = *(DWORD *)"ineI";
                    dwECX = *(DWORD *)"ntel";
                    dwEAX = 13;
                    break;
                case CPUAUTHENTICAMD:
                    dwEBX = *(DWORD *)"Auth";
                    dwEDX = *(DWORD *)"enti";
                    dwECX = *(DWORD *)"cAMD";
                    dwEAX = 13;
                    break;
                case CPUCYRIXINSTEAD:
                    dwEBX = *(DWORD *)"Cyri";
                    dwEDX = *(DWORD *)"xIns";
                    dwECX = *(DWORD *)"tead";
                    dwEAX = 13;
                    break;
                }
                OutTraceDW("Int3Handler: FAKE vendor=\"%04.4s%04.4s%04.4s\" MaxCPUIDInput=%d\n",
                           (char *)&dwEBX, (char *)&dwEDX, (char *)&dwECX, dwEAX);
                break;
            case 0x00000001: // CPU features
                LogCPUFeatures(dwEAX, dwEBX, dwECX, dwEDX);
                if(dxw.dwDFlags & DUMPCPUID) break;
                if((dxw.dwDFlags & CPUDISABLEMMX) || (dxw.dwFlags5 & DISABLEMMX)) {
                    OutTraceDW("Int3Handler: disable MMX & PBE capability\n");
                    // Note: PB doesn't seem related to MMX, but it is necessary to turn it off
                    // to make the Cryo "Arthur's Knight" games pick the correct rendering path
                    // avoiding 3DNow! instructions. Thank to Dege patch that I disassembled and
                    // emulated here.
                    dwEDX &= ~(0x1 << 31); // PBE = 31th bit
                    dwEDX &= ~(0x1 << 23); // MMX = 23th bit
                }
                if(dxw.dwDFlags & CPUDISABLESSE) {
                    OutTraceDW("Int3Handler: disable SSE capability\n");
                    dwEDX &= ~(0x1 << 25); // SSE = 25th bit
                }
                if(dxw.dwDFlags & CPUDISABLESSE2) {
                    OutTraceDW("Int3Handler: disable SSE2 capability\n");
                    dwEDX &= ~(0x1 << 26); // SSE2= 26th bit
                }
                if(dxw.dwDFlags & CPUDISABLEPBE) {
                    // from https://c9x.me/x86/html/file_module_x86_id_45.html
                    // Pending Break Enable. The processor supports the use of the FERR#/PBE# pin when the processor
                    // is in the stop-clock state (STPCLK# is asserted) to signal the processor that an interrupt is
                    // pending and that the processor should return to normal operation to handle the interrupt.
                    // Bit 10 (PBE enable) in the IA32_MISC_ENABLE MSR enables this capability.
                    //
                    // for some reason (possibly even some mistake?) the Cryo "Arthur's Knight" first and second episode
                    // require that this flag is cleared or they enable a 3DNow! renderer with instructions not supported
                    // by modern OSs, from Win7 upward.
                    OutTraceDW("Int3Handler: disable PBE capability\n");
                    dwEDX &= ~(0x1 << 31);
                }
                //dwEAX = 0;
                //dwEBX = 0;
                //dwECX = 0;
#ifdef FAKEVIRTUALBOX
                dwEBX = 0x00000800;
                dwEDX = 0x078bf9bf;
                dwECX = 0x56d8220b;
                dwEAX = 0x000306c3;
#endif
                OutTraceDW("Int3Handler: FAKE eax=%08.8X ebx=%08.8X ecx=%08.8X edx=%0.8X\n",
                           dwEAX, dwEBX, dwECX, dwEDX);
                break;
            case 0x80000001: // CPU extended features
                LogCPUExtendedFeatures(dwEAX, dwEBX, dwECX, dwEDX);
                if(dxw.dwDFlags & DUMPCPUID) break;
                if((dxw.dwDFlags & CPUDISABLEMMX) || (dxw.dwFlags5 & DISABLEMMX)) {
                    OutTraceDW("Int3Handler: disable MMX capability\n");
                    dwEDX &= ~(0x1 << 22); // MMXext = 22th bit
                    dwEDX &= ~(0x1 << 23); // MMX = 23th bit
                    dwEDX &= ~(0x1 << 30); // 3DNow!ext = 30th bit
                    dwEDX &= ~(0x1 << 31); // 3DNow! = 31th bit
                }
                OutTraceDW("Int3Handler: FAKE eax=%08.8X ebx=%08.8X ecx=%08.8X edx=%0.8X\n",
                           dwEAX, dwEBX, dwECX, dwEDX);
                break;
            default:
                break;
            }
            ExceptionInfo->ContextRecord->Eax = dwEAX;
            ExceptionInfo->ContextRecord->Ebx = dwEBX;
            ExceptionInfo->ContextRecord->Ecx = dwECX;
            ExceptionInfo->ContextRecord->Edx = dwEDX;
            ExceptionInfo->ContextRecord->Eip++; // skip ahead one-byte ( jump over 0xCC op-code )
            ExceptionInfo->ContextRecord->Eip++; // skip ahead one-byte ( jump over 0xCC op-code )
            return EXCEPTION_CONTINUE_EXECUTION;
        }
        break;
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

/*  --------------------------------------------------------------------------------------------------
	CPUID - from http://x86.renejeschke.de/html/file_module_x86_id_45.html
    /* --------------------------------------------------------------------------------------------------
    switch(EAX) {
	case 0:
		EAX = HighestCPUIDInput(); //highest basic function input value understood by CPUID
		EBX = VendorIdentificationString[0..3];
		EDX = VendorIdentificationString[4..7];
		ECX = VendorIdentificationString[8..11];
		break;
	case 1:
		EAX[0..3] = SteppingID;
		EAX[4..7] = Model;
		EAX[8..11] = Family;
		EAX[12..13] = ProcessorType;
		EAX[14..15] = Reserved;
		EAX[16..19] = ExtendedModel;
		EAX[20..23] = ExtendedFamily;
		EAX[24..31] = Reserved;
		EBX[0..7] = BrandIndex;
		EBX[8..15] = CLFLUSHLineSize;
		EBX[16..23] = Reserved;
		EBX[24..31] = InitialAPICID;
		ECX = ExtendedFeatureFlags;
		EDX = FeatureFlags;
		break;
	case 2:
		EAX = CacheAndTLBInformation();
		EBX = CacheAndTLBInformation(); //0
		ECX = CacheAndTLBInformation(); //0
		EDX = CacheAndTLBInformation();
		break;
	case 3:
		EAX = Reserved;
		EBX = Reserved;
    #ifdef PentiumIII
		ECX = ProcessorSerialNumber[0..31];
		EDX = ProcessorSerialNumber[32..63];
    #else
		ECX = Reserved;
		EDX = Reserved;
    #endif
		break;
	case 4:
		EAX = DeterministicCacheParametersLeaf();
		EBX = DeterministicCacheParametersLeaf();
		ECX = DeterministicCacheParametersLeaf();
		EDX = DeterministicCacheParametersLeaf();
		break;
	case 5:
		EAX = MONITORMWAITLeaf();
		EBX = MONITORMWAITLeaf();
		ECX = MONITORMWAITLeaf();
		EDX = MONITORMWAITLeaf();
		break;
	case 0x80000000:
		EAX = HighestExtendedCPUIDInput(); //highest extended function input value understood by CPUID
		EBX = Reserved;
		ECX = Reserved;
		EDX = Reserved;
		break;
	case 0x80000001:
		EAX = ExtendedProcessorFeatureSignature; //Currently Reserved
		EBX = Reserved;
		ECX = Reserved;
		EDX = Reserved;
		break;
	case 0x80000002:
		EAX = ProcessorBrandString[0..3];
		EBX = ProcessorBrandString[4..7];
		ECX = ProcessorBrandString[8..11];
		EDX = ProcessorBrandString[12..15];
		break;
	case 0x80000003:
		EAX = ProcessorBrandString[16..19];
		EBX = ProcessorBrandString[20..23];
		ECX = ProcessorBrandString[24..27];
		EDX = ProcessorBrandString[28..31];
		break;
	case 0x80000004:
		EAX = Reserved;
		EBX = Reserved;
		ECX = Reserved;
		EDX = Reserved;
		break;
	case 0x80000006:
		EAX = Reserved;
		EBX = Reserved;
		ECX = CacheInformation;
		EDX = Reserved;
		break;
	case 0x80000007:
		EAX = Reserved;
		EBX = Reserved;
		ECX = Reserved;
		EDX = Reserved;
		break;
	case 0x80000008:
		EAX = Reserved;
		EBX = Reserved;
		ECX = Reserved;
		EDX = Reserved;
		break;
	default: //EAX > highest value recognized by CPUID
		//undefined
		EAX = Reserved;
		EBX = Reserved;
		ECX = Reserved;
		EDX = Reserved;
		break;
    }
    /* ----------------------------------------------------------------------------------------------- */
