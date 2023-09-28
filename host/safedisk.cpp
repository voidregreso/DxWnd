// from https://www.winehq.org/pipermail/wine-users/2002-April/007910.html
//
// There is no publicaly available version numbering for SafeDisc. However, it
// seems that the version number is stored in the executable as 3 unsigned 32-bit
// integers. Using an hexadecimal editor, locate the following byte pattern in
// the wrapper (game.exe)
//
// > 426f475f 202a3930 2e302621 21202059   BoG_ *90.0&!!  Y
// > 793e0000                              y>..
//
// There should be 3 unsigned integers right after that, which are respectively
// the version, subversion an revision number.
//
// On some versions of SafeDisc there are 3 null integers following the pattern,
// before the version number. You'll then have to look at the 3 unsigned 32-bit
// integers right after
//
// > 426f475f 202a3930 2e302621 21202059   BoG_ *90.0&!!  Y
// > 793e0000 00000000 00000000 00000000   y>..............

#include "stdafx.h"

void CheckSafeDiscVersion(char *path) {
    unsigned char opcode[44 + 1];
    char sMessage[81];
    DWORD dwVersion, dwSubversion, dwRevision;
    FILE *exe;
    long seekpos;
    exe = fopen(path, "rb");
    if(!exe) return;
    while(TRUE) {
        seekpos = ftell(exe);
        if(fread(opcode, 1, 44, exe) != 44) break;
        //MessageBox(0, "check", "debug", 0);
        // fast way to make 20 char comparisons .....
        if(*(DWORD *)opcode     == 0x5F476F42)
            if(*(DWORD *)(opcode + 4) == 0x30392A20)
                if(*(DWORD *)(opcode + 8) == 0x2126302E)
                    if(*(DWORD *)(opcode + 12) == 0x59202021)
                        if(*(DWORD *)(opcode + 16) == 0x00003E79) {
                            dwVersion = *(DWORD *)(opcode + 20);
                            dwSubversion = *(DWORD *)(opcode + 24);
                            dwRevision = *(DWORD *)(opcode + 28);
                            if(dwVersion == 0) {
                                dwVersion = *(DWORD *)(opcode + 32);
                                dwSubversion = *(DWORD *)(opcode + 36);
                                dwRevision = *(DWORD *)(opcode + 40);
                            }
                            sprintf(sMessage,  "Safedisk version %d.%d.%d detected", dwVersion, dwSubversion, dwRevision);
                            MessageBox(NULL, sMessage, "DxWnd hint", MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST);
                            break;
                        }
        if(fseek(exe, seekpos + 4, SEEK_SET)) break;
    }
    fclose(exe);
}