////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// OllyDbg Disassembling Engine v2.01                                         //
//                                                                            //
// Copyright (c) 2007-2013 Oleh Yuschuk, ollydbg@t-online.de                  //
//                                                                            //
// This code is part of the OllyDbg Disassembler v2.01                        //
//                                                                            //
// Disassembling engine is free software; you can redistribute it and/or      //
// modify it under the terms of the GNU General Public License as published   //
// by the Free Software Foundation; either version 3 of the License, or (at   //
// your option) any later version.                                            //
//                                                                            //
// This code is distributed in the hope that it will be useful, but WITHOUT   //
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      //
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   //
// more details.                                                              //
//                                                                            //
// You should have received a copy of the GNU General Public License along    //
// with this program. If not, see <http://www.gnu.org/licenses/>.             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// This is a fast disassembler that can be used to determine the length of    //
// the binary 80x86 32-bit command and its attributes, to convert it to the   //
// human-readable text form, highlight its operands, and create hexadecimal   //
// dump of the binary command.                                                //
//                                                                            //
// It is a stripped down version of the disassembler used by OllyDbg 2.01.    //
// It can't analyse and comment the contents of the operands, or predict the  //
// results of the command execution. Analysis-dependent features are not      //
// included, too. Most other features are kept.                               //
//                                                                            //
// Disassembler supports integer, FPU, MMX, 3DNow, SSE1-SSE4.1 and AVX        //
// instructions. 64-bit mode, AVX2, FMA and XOP are not (yet) supported.      //
//                                                                            //
// This code can be compiled either in ASCII or UNICODE mode. It is reentrant //
// (thread-safe, feature not available in the original OllyDbg code).         //
//                                                                            //
// Typical operation speed on 3-GHz Phenom II in MASM mode is:                //
//                                                                            //
//   Command length and info:          130 ns/command (7,700,000 commands/s)  //
//   Disassembly:                      290 ns/command (3,400,000 commands/s)  //
//   Disassembly, dump, highlighting:  350 ns/command (2,800,000 commands/s)  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include <stddef.h>
#pragma hdrstop

#include "disasm.h"

const t_bincmd bincmd[] = {

    {
        T("PAUSE"),
        D_SSE | D_MUSTF3, 0,
        1, 0x000000FF, 0x00000090, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("NOP"),
        D_CMD, DX_NOP,
        1, 0x000000FF, 0x00000090, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("NOP"),
        D_CMD | D_UNDOC, DX_NOP,
        2, 0x0000FFFF, 0x0000190F, 0x00,
        B_INT,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("NOP"),
        D_CMD | D_UNDOC, DX_NOP,
        2, 0x0000FFFF, 0x00001A0F, 0x00,
        B_INT,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("NOP"),
        D_CMD | D_UNDOC, DX_NOP,
        2, 0x0000FFFF, 0x00001B0F, 0x00,
        B_INT,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("NOP"),
        D_CMD | D_UNDOC, DX_NOP,
        2, 0x0000FFFF, 0x00001C0F, 0x00,
        B_INT,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("NOP"),
        D_CMD | D_UNDOC, DX_NOP,
        2, 0x0000FFFF, 0x00001D0F, 0x00,
        B_INT,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("NOP"),
        D_CMD | D_UNDOC, DX_NOP,
        2, 0x0000FFFF, 0x00001E0F, 0x00,
        B_INT,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("NOP"),
        D_CMD, DX_NOP,
        2, 0x0000FFFF, 0x00001F0F, 0x00,
        B_INT,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("MONITOR"),
        D_SYS | D_RARE, 0,
        3, 0x00FFFFFF, 0x00C8010F, 0x00,
        B_EAXMEM | B_PSEUDO,
        B_ECX | B_BINARY | B_PSEUDO,
        B_EDX | B_BINARY | B_PSEUDO,
        B_NONE
    },

    {
        T("MWAIT"),
        D_SYS | D_RARE, 0,
        3, 0x00FFFFFF, 0x00C9010F, 0x00,
        B_EAX | B_BINARY | B_PSEUDO,
        B_ECX | B_BINARY | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("CLAC"),
        D_SYS | D_RARE, 0,
        3, 0x00FFFFFF, 0x00CA010F, 0x00,
        B_EAX | B_BINARY | B_PSEUDO,
        B_ECX | B_BINARY | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("STAC"),
        D_SYS | D_RARE, 0,
        3, 0x00FFFFFF, 0x00CB010F, 0x00,
        B_EAX | B_BINARY | B_PSEUDO,
        B_ECX | B_BINARY | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("AAA"),
        D_CMD | D_ALLFLAGS | D_RARE, 0,
        1, 0x000000FF, 0x00000037, 0x00,
        B_AL | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("AAD"),
        D_CMD | D_ALLFLAGS | D_RARE, 0,
        2, 0x0000FFFF, 0x00000AD5, 0x00,
        B_AX | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("AAD"),
        D_CMD | D_ALLFLAGS | D_RARE, 0,
        1, 0x000000FF, 0x000000D5, 0x00,
        B_AX | B_UPD | B_PSEUDO,
        B_CONST8 | B_UNSIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("AAM"),
        D_CMD | D_ALLFLAGS | D_RARE, 0,
        2, 0x0000FFFF, 0x00000AD4, 0x00,
        B_AX | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("AAM"),
        D_CMD | D_ALLFLAGS | D_RARE, 0,
        1, 0x000000FF, 0x000000D4, 0x00,
        B_AX | B_UPD | B_PSEUDO,
        B_CONST8 | B_UNSIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("AAS"),
        D_CMD | D_ALLFLAGS | D_RARE, 0,
        1, 0x000000FF, 0x0000003F, 0x00,
        B_AL | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("ADC"),
        D_CMD | D_SIZE01 | D_ALLFLAGS | D_USESCARRY, DX_JZ | DX_JB,
        1, 0x000000FE, 0x00000014, 0x00,
        B_ACC | B_UPD,
        B_CONST | B_NOADDR,
        B_NONE,
        B_NONE
    },

    {
        T("ADC"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS | D_USESCARRY, DX_JZ | DX_JB,
        1, 0x000038FE, 0x00001080, 0x00,
        B_INT | B_SHOWSIZE | B_UPD,
        B_CONST | B_NOADDR,
        B_NONE,
        B_NONE
    },

    {
        T("ADC"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS | D_USESCARRY, DX_JZ | DX_JB,
        1, 0x000038FE, 0x00001082, 0x00,
        B_INT | B_SHOWSIZE | B_UPD,
        B_SXTCONST,
        B_NONE,
        B_NONE
    },

    {
        T("ADC"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS | D_USESCARRY, DX_JZ | DX_JB,
        1, 0x000000FE, 0x00000010, 0x00,
        B_INT | B_UPD,
        B_REG,
        B_NONE,
        B_NONE
    },

    {
        T("ADC"),
        D_CMD | D_SIZE01 | D_ALLFLAGS | D_USESCARRY, DX_JZ | DX_JB,
        1, 0x000000FE, 0x00000012, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("ADD"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JB | DX_ADD,
        1, 0x000000FE, 0x00000004, 0x00,
        B_ACC | B_UPD,
        B_CONST,
        B_NONE,
        B_NONE
    },

    {
        T("ADD"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ | DX_JB | DX_ADD,
        1, 0x000038FE, 0x00000080, 0x00,
        B_INT | B_SHOWSIZE | B_UPD,
        B_CONST,
        B_NONE,
        B_NONE
    },

    {
        T("ADD"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ | DX_JB | DX_ADD,
        1, 0x000038FE, 0x00000082, 0x00,
        B_INT | B_SHOWSIZE | B_UPD,
        B_SXTCONST,
        B_NONE,
        B_NONE
    },

    {
        T("ADD"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ | DX_JB | DX_ADD,
        1, 0x000000FE, 0x00000000, 0x00,
        B_INT | B_UPD,
        B_REG,
        B_NONE,
        B_NONE
    },

    {
        T("ADD"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JB | DX_ADD,
        1, 0x000000FE, 0x00000002, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("AND"),
        D_TEST | D_SIZE01 | D_ALLFLAGS, DX_JZ,
        1, 0x000000FE, 0x00000024, 0x00,
        B_ACC | B_BINARY | B_UPD,
        B_CONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("AND"),
        D_TEST | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ,
        1, 0x000038FE, 0x00002080, 0x00,
        B_INT | B_BINARY | B_SHOWSIZE | B_UPD,
        B_CONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("AND"),
        D_TEST | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ,
        1, 0x000038FE, 0x00002082, 0x00,
        B_INT | B_BINARY | B_SHOWSIZE | B_UPD,
        B_SXTCONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("AND"),
        D_TEST | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ,
        1, 0x000000FE, 0x00000020, 0x00,
        B_INT | B_BINARY | B_UPD,
        B_REG | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("AND"),
        D_TEST | D_SIZE01 | D_ALLFLAGS, DX_JZ,
        1, 0x000000FE, 0x00000022, 0x00,
        B_REG | B_BINARY | B_UPD,
        B_INT | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("ARPL"),
        D_SYS | D_FLAGZ | D_RARE, 0,
        1, 0x000000FF, 0x00000063, 0x00,
        B_INT16 | B_UPD,
        B_REG16,
        B_NONE,
        B_NONE
    },

    {
        T("BOUND"),
        D_CMD | D_RARE, 0,
        1, 0x000000FF, 0x00000062, 0x00,
        B_REG | B_SIGNED,
        B_INTPAIR | B_MEMONLY,
        B_NONE,
        B_NONE
    },

    {
        T("BSF"),
        D_CMD | D_ALLFLAGS, DX_JZ,
        2, 0x0000FFFF, 0x0000BC0F, 0x00,
        B_REG | B_CHG,
        B_INT | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("BSR"),
        D_CMD | D_NOREP | D_ALLFLAGS, DX_JZ,
        2, 0x0000FFFF, 0x0000BD0F, 0x00,
        B_REG | B_CHG,
        B_INT | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("BSWAP"),
        D_CMD, 0,
        2, 0x0000F8FF, 0x0000C80F, 0x00,
        B_REGCMD | B_32BITONLY | B_NOESP | B_UPD,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("BT"),
        D_TEST | D_ALLFLAGS, DX_JC,
        2, 0x0000FFFF, 0x0000A30F, 0x00,
        B_INT | B_BINARY,
        B_REG | B_BITCNT,
        B_NONE,
        B_NONE
    },

    {
        T("BT"),
        D_TEST | D_ALLFLAGS, DX_JC,
        2, 0x0038FFFF, 0x0020BA0F, 0x00,
        B_INT | B_BINARY | B_SHOWSIZE,
        B_CONST8 | B_BITCNT,
        B_NONE,
        B_NONE
    },

    {
        T("BTC"),
        D_CMD | D_LOCKABLE | D_ALLFLAGS, DX_JC,
        2, 0x0000FFFF, 0x0000BB0F, 0x00,
        B_INT | B_BINARY | B_NOESP | B_UPD,
        B_REG | B_BITCNT,
        B_NONE,
        B_NONE
    },

    {
        T("BTC"),
        D_CMD | D_LOCKABLE | D_ALLFLAGS, DX_JC,
        2, 0x0038FFFF, 0x0038BA0F, 0x00,
        B_INT | B_BINARY | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_BITCNT,
        B_NONE,
        B_NONE
    },

    {
        T("BTR"),
        D_CMD | D_LOCKABLE | D_ALLFLAGS, DX_JC,
        2, 0x0000FFFF, 0x0000B30F, 0x00,
        B_INT | B_BINARY | B_NOESP | B_UPD,
        B_REG | B_BITCNT,
        B_NONE,
        B_NONE
    },

    {
        T("BTR"),
        D_CMD | D_LOCKABLE | D_ALLFLAGS, DX_JC,
        2, 0x0038FFFF, 0x0030BA0F, 0x00,
        B_INT | B_BINARY | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_BITCNT,
        B_NONE,
        B_NONE
    },

    {
        T("BTS"),
        D_CMD | D_LOCKABLE | D_ALLFLAGS, DX_JC,
        2, 0x0000FFFF, 0x0000AB0F, 0x00,
        B_INT | B_BINARY | B_NOESP | B_UPD,
        B_REG | B_BITCNT,
        B_NONE,
        B_NONE
    },

    {
        T("BTS"),
        D_CMD | D_LOCKABLE | D_ALLFLAGS, DX_JC,
        2, 0x0038FFFF, 0x0028BA0F, 0x00,
        B_INT | B_BINARY | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_BITCNT,
        B_NONE,
        B_NONE
    },

    {
        T("CALL"),
        D_CALL | D_CHGESP, 0,
        1, 0x000000FF, 0x000000E8, 0x00,
        B_OFFSET | B_JMPCALL,
        B_PUSHRET | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("CALL"),
        D_CALL | D_CHGESP, 0,
        1, 0x000038FF, 0x000010FF, 0x00,
        B_INT | B_JMPCALL,
        B_PUSHRET | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("CALL"),
        D_CALLFAR | D_CHGESP | D_RARE, 0,
        1, 0x000000FF, 0x0000009A, 0x00,
        B_FARCONST | B_JMPCALLFAR,
        B_PUSHRETF | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("CALL"),
        D_CALLFAR | D_CHGESP | D_RARE, 0,
        1, 0x000038FF, 0x000018FF, 0x00,
        B_SEGOFFS | B_JMPCALLFAR | B_MEMONLY,
        B_PUSHRETF | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("CBW"),
        D_CMD | D_DATA16, 0,
        1, 0x000000FF, 0x00000098, 0x00,
        B_AX | B_UPD | B_PSEUDO,
        B_AL | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("CBW"),
        D_CMD | D_DATA16, 0,
        1, 0x000000FF, 0x00000098, 0x00,
        B_AX | B_UPD | B_PSEUDO,
        B_AL,
        B_NONE,
        B_NONE
    },

    {
        T("CDQ"),
        D_CMD | D_DATA32, 0,
        1, 0x000000FF, 0x00000099, 0x00,
        B_EDX | B_CHG | B_PSEUDO,
        B_EAX | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("CDQ"),
        D_CMD | D_DATA32, 0,
        1, 0x000000FF, 0x00000099, 0x00,
        B_EDX | B_CHG | B_PSEUDO,
        B_EAX,
        B_NONE,
        B_NONE
    },

    {
        T("CLC"),
        D_CMD | D_FLAGC, 0,
        1, 0x000000FF, 0x000000F8, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("CLD"),
        D_CMD | D_FLAGD, 0,
        1, 0x000000FF, 0x000000FC, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("CLFLUSH"),
        D_CMD | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0038AE0F, 0x00,
        B_ANYMEM | B_MEMONLY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("CLI"),
        D_CMD | D_RARE, 0,
        1, 0x000000FF, 0x000000FA, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("CLTS"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000060F, 0x00,
        B_CR0 | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("CMC"),
        D_CMD | D_FLAGC, 0,
        1, 0x000000FF, 0x000000F5, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVO"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x0000400F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNO"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x0000410F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVB"),
        D_MOVC | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000420F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVC"),
        D_MOVC | D_COND | D_USESCARRY, DX_JC,
        2, 0x0000FFFF, 0x0000420F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNAE"),
        D_MOVC | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000420F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVAE"),
        D_MOVC | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000430F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNB"),
        D_MOVC | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000430F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNC"),
        D_MOVC | D_COND | D_USESCARRY, DX_JC,
        2, 0x0000FFFF, 0x0000430F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVE"),
        D_MOVC | D_COND, DX_JE,
        2, 0x0000FFFF, 0x0000440F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVZ"),
        D_MOVC | D_COND, DX_JZ,
        2, 0x0000FFFF, 0x0000440F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNE"),
        D_MOVC | D_COND, DX_JE,
        2, 0x0000FFFF, 0x0000450F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNZ"),
        D_MOVC | D_COND, DX_JZ,
        2, 0x0000FFFF, 0x0000450F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVBE"),
        D_MOVC | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000460F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNA"),
        D_MOVC | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000460F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVA"),
        D_MOVC | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000470F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNBE"),
        D_MOVC | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000470F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVS"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x0000480F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNS"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x0000490F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVPE"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004A0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVP"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004A0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVPO"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004B0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNP"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004B0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVL"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004C0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNGE"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004C0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVGE"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004D0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNL"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004D0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVLE"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004E0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNG"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004E0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVG"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004F0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVNLE"),
        D_MOVC | D_COND, 0,
        2, 0x0000FFFF, 0x00004F0F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMP"),
        D_TEST | D_SIZE01 | D_ALLFLAGS | D_HLADIR, DX_JE | DX_JB,
        1, 0x000000FE, 0x0000003C, 0x00,
        B_ACC,
        B_CONST,
        B_NONE,
        B_NONE
    },

    {
        T("CMP"),
        D_TEST | D_SIZE01 | D_ALLFLAGS | D_HLADIR, DX_JE | DX_JB,
        1, 0x000038FE, 0x00003880, 0x00,
        B_INT | B_SHOWSIZE,
        B_CONST,
        B_NONE,
        B_NONE
    },

    {
        T("CMP"),
        D_TEST | D_SIZE01 | D_ALLFLAGS | D_HLADIR, DX_JE | DX_JB,
        1, 0x000038FE, 0x00003882, 0x00,
        B_INT | B_SHOWSIZE,
        B_SXTCONST,
        B_NONE,
        B_NONE
    },

    {
        T("CMP"),
        D_TEST | D_SIZE01 | D_ALLFLAGS | D_HLADIR, DX_JE | DX_JB,
        1, 0x000000FE, 0x00000038, 0x00,
        B_INT,
        B_REG,
        B_NONE,
        B_NONE
    },

    {
        T("CMP"),
        D_TEST | D_SIZE01 | D_ALLFLAGS | D_HLADIR, DX_JE | DX_JB,
        1, 0x000000FE, 0x0000003A, 0x00,
        B_REG,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("CMPXCHG"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS | D_HLADIR, DX_JE | DX_JB,
        2, 0x0000FEFF, 0x0000B00F, 0x00,
        B_INT | B_UPD,
        B_REG,
        B_ACC | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("CMPXCHG8B"),
        D_CMD | D_LOCKABLE | D_MEMORY | D_ALLFLAGS, DX_JE | DX_JB,
        2, 0x0038FFFF, 0x0008C70F, 0x00,
        B_INT64 | B_MEMONLY | B_UPD,
        B_EAX | B_UPD | B_PSEUDO,
        B_EDX | B_UPD | B_PSEUDO,
        B_EBX | B_PSEUDO
    },

    {
        T("CPUID"),
        D_CMD, 0,
        2, 0x0000FFFF, 0x0000A20F, 0x00,
        B_EAX | B_CHG | B_PSEUDO,
        B_EBX | B_CHG | B_PSEUDO,
        B_ECX | B_CHG | B_PSEUDO,
        B_EDX | B_CHG | B_PSEUDO
    },

    {
        T("CWD"),
        D_CMD | D_DATA16, 0,
        1, 0x000000FF, 0x00000099, 0x00,
        B_DX | B_CHG | B_PSEUDO,
        B_AX | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("CWD"),
        D_CMD | D_DATA16, 0,
        1, 0x000000FF, 0x00000099, 0x00,
        B_DX | B_CHG | B_PSEUDO,
        B_AX,
        B_NONE,
        B_NONE
    },

    {
        T("CWDE"),
        D_CMD | D_DATA32, 0,
        1, 0x000000FF, 0x00000098, 0x00,
        B_EAX | B_UPD | B_PSEUDO,
        B_AX | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("CWDE"),
        D_CMD | D_DATA32, 0,
        1, 0x000000FF, 0x00000098, 0x00,
        B_EAX | B_UPD | B_PSEUDO,
        B_AX,
        B_NONE,
        B_NONE
    },

    {
        T("DAA"),
        D_CMD | D_ALLFLAGS | D_USESCARRY | D_RARE, DX_JC,
        1, 0x000000FF, 0x00000027, 0x00,
        B_AL | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("DAS"),
        D_CMD | D_ALLFLAGS | D_USESCARRY | D_RARE, DX_JC,
        1, 0x000000FF, 0x0000002F, 0x00,
        B_AL | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("DEC"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_NOCFLAG, DX_JZ,
        1, 0x000038FE, 0x000008FE, 0x00,
        B_INT | B_SHOWSIZE | B_UPD,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("DEC"),
        D_CMD | D_NOCFLAG, DX_JZ,
        1, 0x000000F8, 0x00000048, 0x00,
        B_REGCMD | B_UPD,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("DIV"),
        D_CMD | D_ALLFLAGS, 0,
        1, 0x000038FF, 0x000030F6, 0x00,
        B_INT8 | B_SHOWSIZE,
        B_AX | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("DIV"),
        D_CMD | D_ALLFLAGS, 0,
        1, 0x000038FF, 0x000030F7, 0x00,
        B_INT1632 | B_UNSIGNED | B_NOESP | B_SHOWSIZE,
        B_DXEDX | B_UPD | B_PSEUDO,
        B_ACC | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("EMMS"),
        D_CMD, 0,
        2, 0x0000FFFF, 0x0000770F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("ENTER"),
        D_CMD | D_CHGESP, 0,
        1, 0x000000FF, 0x000000C8, 0x00,
        B_CONST16 | B_STACKINC,
        B_CONST8_2 | B_UNSIGNED,
        B_PUSH | B_CHG | B_PSEUDO,
        B_BPEBP | B_CHG | B_PSEUDO
    },

    {
        T("WAIT"),
        D_CMD, 0,
        1, 0x000000FF, 0x0000009B, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FWAIT"),
        D_CMD, 0,
        1, 0x000000FF, 0x0000009B, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("HLT"),
        D_PRIVILEGED | D_RARE, 0,
        1, 0x000000FF, 0x000000F4, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("IDIV"),
        D_CMD | D_ALLFLAGS, 0,
        1, 0x000038FF, 0x000038F6, 0x00,
        B_INT8 | B_SIGNED | B_SHOWSIZE,
        B_AX | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("IDIV"),
        D_CMD | D_ALLFLAGS, 0,
        1, 0x000038FF, 0x000038F7, 0x00,
        B_INT1632 | B_SIGNED | B_NOESP | B_SHOWSIZE,
        B_DXEDX | B_UPD | B_PSEUDO,
        B_ACC | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("IMUL"),
        D_CMD | D_ALLFLAGS, 0,
        1, 0x000038FF, 0x000028F6, 0x00,
        B_AX | B_UPD | B_PSEUDO,
        B_INT8 | B_SIGNED | B_SHOWSIZE,
        B_AL | B_SIGNED | B_PSEUDO,
        B_NONE
    },

    {
        T("IMUL"),
        D_CMD | D_ALLFLAGS, 0,
        1, 0x000038FF, 0x000028F7, 0x00,
        B_DXEDX | B_CHG | B_PSEUDO,
        B_ACC | B_UPD | B_PSEUDO,
        B_INT1632 | B_SIGNED | B_NOESP | B_SHOWSIZE,
        B_NONE
    },

    {
        T("IMUL"),
        D_CMD | D_ALLFLAGS, 0,
        2, 0x0000FFFF, 0x0000AF0F, 0x00,
        B_REG | B_UPD,
        B_INT | B_NOESP,
        B_NONE,
        B_NONE
    },

    {
        T("IMUL"),
        D_CMD | D_ALLFLAGS, 0,
        1, 0x000000FF, 0x0000006B, 0x00,
        B_REG | B_CHG,
        B_INT | B_NOESP,
        B_SXTCONST,
        B_NONE
    },

    {
        T("IMUL"),
        D_CMD | D_ALLFLAGS, 0,
        1, 0x000000FF, 0x00000069, 0x00,
        B_REG | B_CHG,
        B_INT | B_NOESP,
        B_CONST | B_SIGNED,
        B_NONE
    },

    {
        T("IN"),
        D_IO | D_SIZE01 | D_RARE, 0,
        1, 0x000000FE, 0x000000E4, 0x00,
        B_ACC | B_CHG,
        B_CONST8 | B_PORT,
        B_NONE,
        B_NONE
    },

    {
        T("IN"),
        D_IO | D_SIZE01 | D_RARE, 0,
        1, 0x000000FE, 0x000000EC, 0x00,
        B_ACC | B_CHG,
        B_DXPORT | B_PORT,
        B_NONE,
        B_NONE
    },

    {
        T("INC"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_NOCFLAG, DX_JZ,
        1, 0x000038FE, 0x000000FE, 0x00,
        B_INT | B_SHOWSIZE | B_UPD,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("INC"),
        D_CMD | D_NOCFLAG, DX_JZ,
        1, 0x000000F8, 0x00000040, 0x00,
        B_REGCMD | B_UPD,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("INT"),
        D_INT, 0,
        1, 0x000000FF, 0x000000CD, 0x00,
        B_CONST8,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("INT3"),
        D_INT | D_RARE, 0,
        1, 0x000000FF, 0x000000CC, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("INTO"),
        D_INT | D_RARE, 0,
        1, 0x000000FF, 0x000000CE, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("INT1"),
        D_INT | D_UNDOC, 0,
        1, 0x000000FF, 0x000000F1, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("ICEBP"),
        D_INT | D_UNDOC, 0,
        1, 0x000000FF, 0x000000F1, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("INVD"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000080F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("INVLPG"),
        D_PRIVILEGED | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0038010F, 0x00,
        B_ANYMEM | B_MEMONLY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("IRET*"),
        D_RETFAR | D_ALLFLAGS | D_CHGESP | D_WILDCARD | D_RARE, 0,
        1, 0x000000FF, 0x000000CF, 0x00,
        B_STKTOPFAR | B_JMPCALLFAR | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JO"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x00000070, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JO"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x0000800F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNO"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x00000071, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNO"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x0000810F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JB"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JB,
        1, 0x000000FF, 0x00000072, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JC"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JC,
        1, 0x000000FF, 0x00000072, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNAE"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JB,
        1, 0x000000FF, 0x00000072, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JB"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000820F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JC"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JC,
        2, 0x0000FFFF, 0x0000820F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNAE"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000820F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JAE"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JB,
        1, 0x000000FF, 0x00000073, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNB"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JB,
        1, 0x000000FF, 0x00000073, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNC"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JC,
        1, 0x000000FF, 0x00000073, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JAE"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000830F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNB"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000830F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNC"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, DX_JC,
        2, 0x0000FFFF, 0x0000830F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JE"),
        D_JMC | D_BHINT | D_COND, DX_JE,
        1, 0x000000FF, 0x00000074, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JZ"),
        D_JMC | D_BHINT | D_COND, DX_JZ,
        1, 0x000000FF, 0x00000074, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JE"),
        D_JMC | D_BHINT | D_COND, DX_JE,
        2, 0x0000FFFF, 0x0000840F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JZ"),
        D_JMC | D_BHINT | D_COND, DX_JZ,
        2, 0x0000FFFF, 0x0000840F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNE"),
        D_JMC | D_BHINT | D_COND, DX_JE,
        1, 0x000000FF, 0x00000075, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNZ"),
        D_JMC | D_BHINT | D_COND, DX_JZ,
        1, 0x000000FF, 0x00000075, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNE"),
        D_JMC | D_BHINT | D_COND, DX_JE,
        2, 0x0000FFFF, 0x0000850F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNZ"),
        D_JMC | D_BHINT | D_COND, DX_JZ,
        2, 0x0000FFFF, 0x0000850F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JBE"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, 0,
        1, 0x000000FF, 0x00000076, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNA"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, 0,
        1, 0x000000FF, 0x00000076, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JBE"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000860F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNA"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000860F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JA"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, 0,
        1, 0x000000FF, 0x00000077, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNBE"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, 0,
        1, 0x000000FF, 0x00000077, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JA"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000870F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNBE"),
        D_JMC | D_BHINT | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000870F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JS"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x00000078, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JS"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x0000880F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNS"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x00000079, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNS"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x0000890F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JPE"),
        D_JMC | D_BHINT | D_COND | D_RARE, 0,
        1, 0x000000FF, 0x0000007A, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JP"),
        D_JMC | D_BHINT | D_COND | D_RARE, 0,
        1, 0x000000FF, 0x0000007A, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JPE"),
        D_JMC | D_BHINT | D_COND | D_RARE, 0,
        2, 0x0000FFFF, 0x00008A0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JP"),
        D_JMC | D_BHINT | D_COND | D_RARE, 0,
        2, 0x0000FFFF, 0x00008A0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JPO"),
        D_JMC | D_BHINT | D_COND | D_RARE, 0,
        1, 0x000000FF, 0x0000007B, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNP"),
        D_JMC | D_BHINT | D_COND | D_RARE, 0,
        1, 0x000000FF, 0x0000007B, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JPO"),
        D_JMC | D_BHINT | D_COND | D_RARE, 0,
        2, 0x0000FFFF, 0x00008B0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNP"),
        D_JMC | D_BHINT | D_COND | D_RARE, 0,
        2, 0x0000FFFF, 0x00008B0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JL"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x0000007C, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNGE"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x0000007C, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JL"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x00008C0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNGE"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x00008C0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JGE"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x0000007D, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNL"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x0000007D, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JGE"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x00008D0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNL"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x00008D0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JLE"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x0000007E, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNG"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x0000007E, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JLE"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x00008E0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNG"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x00008E0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JG"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x0000007F, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNLE"),
        D_JMC | D_BHINT | D_COND, 0,
        1, 0x000000FF, 0x0000007F, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JG"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x00008F0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JNLE"),
        D_JMC | D_BHINT | D_COND, 0,
        2, 0x0000FFFF, 0x00008F0F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JCXZ"),
        D_JMCX | D_ADDR16 | D_BHINT, 0,
        1, 0x000000FF, 0x000000E3, 0x00,
        B_CX | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("JECXZ"),
        D_JMCX | D_ADDR32 | D_BHINT, 0,
        1, 0x000000FF, 0x000000E3, 0x00,
        B_ECX | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("JMP"),
        D_JMP, 0,
        1, 0x000000FF, 0x000000EB, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JMP"),
        D_JMP, 0,
        1, 0x000000FF, 0x000000E9, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JMP"),
        D_JMP, 0,
        1, 0x000038FF, 0x000020FF, 0x00,
        B_INT | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JMP"),
        D_JMPFAR | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x000000EA, 0x00,
        B_FARCONST | B_JMPCALLFAR,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JMP"),
        D_JMPFAR | D_RARE, 0,
        1, 0x000038FF, 0x000028FF, 0x00,
        B_SEGOFFS | B_JMPCALLFAR | B_MEMONLY | B_SHOWSIZE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("LAHF"),
        D_CMD, 0,
        1, 0x000000FF, 0x0000009F, 0x00,
        B_AH | B_CHG | B_PSEUDO,
        B_FLAGS8 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("LAR"),
        D_CMD | D_FLAGZ | D_RARE, DX_JZ,
        2, 0x0000FFFF, 0x0000020F, 0x00,
        B_REG | B_BINARY | B_NOESP | B_CHG,
        B_INT | B_BINARY | B_NOESP,
        B_NONE,
        B_NONE
    },

    {
        T("LDS"),
        D_CMD | D_RARE, 0,
        1, 0x000000FF, 0x000000C5, 0x00,
        B_SEGDS | B_CHG | B_PSEUDO,
        B_REG | B_BINARY | B_CHG,
        B_SEGOFFS | B_MEMONLY,
        B_NONE
    },

    {
        T("LES"),
        D_CMD | D_RARE, 0,
        1, 0x000000FF, 0x000000C4, 0x00,
        B_SEGES | B_CHG | B_PSEUDO,
        B_REG | B_BINARY | B_CHG,
        B_SEGOFFS | B_MEMONLY,
        B_NONE
    },

    {
        T("LFS"),
        D_CMD | D_RARE, 0,
        2, 0x0000FFFF, 0x0000B40F, 0x00,
        B_SEGFS | B_CHG | B_PSEUDO,
        B_REG | B_BINARY | B_CHG,
        B_SEGOFFS | B_MEMONLY,
        B_NONE
    },

    {
        T("LGS"),
        D_CMD | D_RARE, 0,
        2, 0x0000FFFF, 0x0000B50F, 0x00,
        B_SEGGS | B_CHG | B_PSEUDO,
        B_REG | B_BINARY | B_CHG,
        B_SEGOFFS | B_MEMONLY,
        B_NONE
    },

    {
        T("LSS"),
        D_CMD | D_RARE, 0,
        2, 0x0000FFFF, 0x0000B20F, 0x00,
        B_SEGSS | B_CHG | B_PSEUDO,
        B_REG | B_BINARY | B_CHG,
        B_SEGOFFS | B_MEMONLY,
        B_NONE
    },

    {
        T("LEA"),
        D_CMD | D_HLADIR, DX_LEA,
        1, 0x000000FF, 0x0000008D, 0x00,
        B_REG | B_BINARY | B_CHG,
        B_ANYMEM | B_MEMONLY | B_NOSEG,
        B_NONE,
        B_NONE
    },

    {
        T("LEAVE"),
        D_CMD | D_CHGESP, 0,
        1, 0x000000FF, 0x000000C9, 0x00,
        B_BPEBP | B_CHG | B_PSEUDO,
        B_EBPMEM | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("LGDT"),
        D_PRIVILEGED | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0010010F, 0x00,
        B_DESCR | B_MEMONLY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("LIDT"),
        D_PRIVILEGED | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0018010F, 0x00,
        B_DESCR | B_MEMONLY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("LLDT"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0038FFFF, 0x0010000F, 0x00,
        B_INT16 | B_NOESP,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("LMSW"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0038FFFF, 0x0030010F, 0x00,
        B_CR0 | B_UPD | B_PSEUDO,
        B_INT16 | B_NOESP,
        B_NONE,
        B_NONE
    },

    {
        T("LOOP"),
        D_JMCX | D_ADDR32, 0,
        1, 0x000000FF, 0x000000E2, 0x00,
        B_ECX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPD"),
        D_JMCX | D_ADDR32, 0,
        1, 0x000000FF, 0x000000E2, 0x00,
        B_ECX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPW"),
        D_JMCX | D_ADDR16, 0,
        1, 0x000000FF, 0x000000E2, 0x00,
        B_CX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPZ"),
        D_JMCX | D_ADDR32 | D_COND, 0,
        1, 0x000000FF, 0x000000E1, 0x00,
        B_ECX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPDZ"),
        D_JMCX | D_ADDR32 | D_COND, 0,
        1, 0x000000FF, 0x000000E1, 0x00,
        B_ECX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPWZ"),
        D_JMCX | D_ADDR16 | D_COND, 0,
        1, 0x000000FF, 0x000000E1, 0x00,
        B_CX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPE"),
        D_JMCX | D_ADDR32 | D_COND, 0,
        1, 0x000000FF, 0x000000E1, 0x00,
        B_ECX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPDE"),
        D_JMCX | D_ADDR32 | D_COND, 0,
        1, 0x000000FF, 0x000000E1, 0x00,
        B_ECX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPWE"),
        D_JMCX | D_ADDR16 | D_COND, 0,
        1, 0x000000FF, 0x000000E1, 0x00,
        B_CX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPNZ"),
        D_JMCX | D_ADDR32 | D_COND, 0,
        1, 0x000000FF, 0x000000E0, 0x00,
        B_ECX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPDNZ"),
        D_JMCX | D_ADDR32 | D_COND, 0,
        1, 0x000000FF, 0x000000E0, 0x00,
        B_ECX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPWNZ"),
        D_JMCX | D_ADDR16 | D_COND, 0,
        1, 0x000000FF, 0x000000E0, 0x00,
        B_CX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPNE"),
        D_JMCX | D_ADDR32 | D_COND, 0,
        1, 0x000000FF, 0x000000E0, 0x00,
        B_ECX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPDNE"),
        D_JMCX | D_ADDR32 | D_COND, 0,
        1, 0x000000FF, 0x000000E0, 0x00,
        B_ECX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LOOPWNE"),
        D_JMCX | D_ADDR16 | D_COND, 0,
        1, 0x000000FF, 0x000000E0, 0x00,
        B_CX | B_UPD | B_PSEUDO,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE
    },

    {
        T("LSL"),
        D_SYS | D_FLAGZ | D_RARE, 0,
        2, 0x0000FFFF, 0x0000030F, 0x00,
        B_REG | B_NOESP | B_CHG,
        B_INT | B_BINARY | B_NOESP,
        B_NONE,
        B_NONE
    },

    {
        T("LTR"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0038FFFF, 0x0018000F, 0x00,
        B_INT16 | B_NOESP,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_MOV | D_SIZE01, 0,
        1, 0x000000FE, 0x00000088, 0x00,
        B_INT | B_CHG,
        B_REG,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_MOV | D_SIZE01, 0,
        1, 0x000000FE, 0x0000008A, 0x00,
        B_REG | B_CHG,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_CMD | D_REGISTER | D_RARE, 0,
        1, 0x0000C0FF, 0x0000C08C, 0x00,
        B_INT | B_REGISTER | B_NOESP | B_CHG,
        B_SEG,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_CMD | D_MEMORY | D_RARE, 0,
        1, 0x000000FF, 0x0000008C, 0x00,
        B_INT16 | B_MEMORY | B_CHG,
        B_SEG,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_CMD | D_RARE, 0,
        1, 0x000000FF, 0x0000008E, 0x00,
        B_SEGNOCS | B_CHG,
        B_INT | B_REGISTER | B_NOESP,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_CMD | D_RARE, 0,
        1, 0x000000FF, 0x0000008E, 0x00,
        B_SEGNOCS | B_CHG,
        B_INT16 | B_MEMORY | B_NOESP,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_MOV | D_SIZE01, 0,
        1, 0x000000FE, 0x000000A0, 0x00,
        B_ACC | B_CHG,
        B_IMMINT,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_MOV | D_SIZE01, 0,
        1, 0x000000FE, 0x000000A2, 0x00,
        B_IMMINT | B_CHG,
        B_ACC,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_MOV, 0,
        1, 0x000000F8, 0x000000B0, 0x00,
        B_REGCMD8 | B_CHG,
        B_CONST8,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_MOV, 0,
        1, 0x000000F8, 0x000000B8, 0x00,
        B_REGCMD | B_NOESP | B_CHG,
        B_CONST,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_MOV | D_SIZE01, 0,
        1, 0x000038FE, 0x000000C6, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_CHG,
        B_CONST,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000220F, 0x00,
        B_CR | B_CHG,
        B_INT32 | B_BINARY | B_REGONLY | B_NOESP,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000200F, 0x00,
        B_INT32 | B_BINARY | B_REGONLY | B_NOESP | B_CHG,
        B_CR,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000230F, 0x00,
        B_DR | B_CHG,
        B_INT32 | B_BINARY | B_REGONLY | B_NOESP,
        B_NONE,
        B_NONE
    },

    {
        T("MOV"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000210F, 0x00,
        B_INT32 | B_BINARY | B_REGONLY | B_NOESP | B_CHG,
        B_DR,
        B_NONE,
        B_NONE
    },

    {
        T("MOVSX"),
        D_MOV, 0,
        2, 0x0000FFFF, 0x0000BE0F, 0x00,
        B_REG | B_NOESP | B_CHG,
        B_INT8 | B_SIGNED | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("MOVSX"),
        D_MOV, 0,
        2, 0x0000FFFF, 0x0000BF0F, 0x00,
        B_REG32 | B_NOESP | B_CHG,
        B_INT16 | B_SIGNED | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("MOVZX"),
        D_MOV, 0,
        2, 0x0000FFFF, 0x0000B60F, 0x00,
        B_REG | B_NOESP | B_CHG,
        B_INT8 | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("MOVZX"),
        D_MOV, 0,
        2, 0x0000FFFF, 0x0000B70F, 0x00,
        B_REG32 | B_NOESP | B_CHG,
        B_INT16 | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("MUL"),
        D_CMD | D_ALLFLAGS, 0,
        1, 0x000038FF, 0x000020F6, 0x00,
        B_AX | B_UPD | B_PSEUDO,
        B_AL | B_PSEUDO,
        B_INT8 | B_UNSIGNED | B_SHOWSIZE,
        B_NONE
    },

    {
        T("MUL"),
        D_CMD | D_ALLFLAGS, 0,
        1, 0x000038FF, 0x000020F7, 0x00,
        B_DXEDX | B_CHG | B_PSEUDO,
        B_ACC | B_UPD | B_PSEUDO,
        B_INT1632 | B_UNSIGNED | B_NOESP | B_SHOWSIZE,
        B_NONE
    },

    {
        T("NEG"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000018F6, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("NOT"),
        D_CMD | D_SIZE01 | D_LOCKABLE, 0,
        1, 0x000038FE, 0x000010F6, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("OR"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JB,
        1, 0x000000FE, 0x0000000C, 0x00,
        B_ACC | B_BINARY | B_UPD,
        B_CONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("OR"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ | DX_JB,
        1, 0x000038FE, 0x00000880, 0x00,
        B_INT | B_BINARY | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("OR"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ | DX_JB,
        1, 0x000038FE, 0x00000882, 0x00,
        B_INT | B_BINARY | B_NOESP | B_SHOWSIZE | B_UPD,
        B_SXTCONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("OR"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ | DX_JB,
        1, 0x000000FE, 0x00000008, 0x00,
        B_INT | B_BINARY | B_NOESP | B_UPD,
        B_REG | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("OR"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JB,
        1, 0x000000FE, 0x0000000A, 0x00,
        B_REG | B_BINARY | B_NOESP | B_UPD,
        B_INT | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("OUT"),
        D_IO | D_SIZE01 | D_RARE, 0,
        1, 0x000000FE, 0x000000E6, 0x00,
        B_CONST8 | B_PORT,
        B_ACC,
        B_NONE,
        B_NONE
    },

    {
        T("OUT"),
        D_IO | D_SIZE01 | D_RARE, 0,
        1, 0x000000FE, 0x000000EE, 0x00,
        B_DXPORT | B_PORT,
        B_ACC,
        B_NONE,
        B_NONE
    },

    {
        T("POP"),
        D_POP | D_CHGESP, 0,
        1, 0x000038FF, 0x0000008F, 0x00,
        B_INT | B_SHOWSIZE | B_CHG,
        B_STKTOP | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("POP"),
        D_POP | D_CHGESP, 0,
        1, 0x000000F8, 0x00000058, 0x00,
        B_REGCMD | B_CHG,
        B_STKTOP | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("POP"),
        D_POP | D_CHGESP | D_RARE, 0,
        1, 0x000000FF, 0x0000001F, 0x00,
        B_SEGDS | B_CHG,
        B_STKTOP | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("POP"),
        D_POP | D_CHGESP | D_RARE, 0,
        1, 0x000000FF, 0x00000007, 0x00,
        B_SEGES | B_CHG,
        B_STKTOP | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("POP"),
        D_POP | D_CHGESP | D_RARE, DX_JB,
        1, 0x000000FF, 0x00000017, 0x00,
        B_SEGSS | B_CHG,
        B_STKTOP | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("POP"),
        D_POP | D_CHGESP | D_RARE, 0,
        2, 0x0000FFFF, 0x0000A10F, 0x00,
        B_SEGFS | B_CHG,
        B_STKTOP | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("POP"),
        D_POP | D_CHGESP | D_RARE, 0,
        2, 0x0000FFFF, 0x0000A90F, 0x00,
        B_SEGGS | B_CHG,
        B_STKTOP | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("POPA*"),
        D_CMD | D_CHGESP | D_WILDCARD, 0,
        1, 0x000000FF, 0x00000061, 0x00,
        B_STKTOPA | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("POPF*"),
        D_POP | D_ALLFLAGS | D_CHGESP | D_WILDCARD, 0,
        1, 0x000000FF, 0x0000009D, 0x00,
        B_EFL | B_CHG | B_PSEUDO,
        B_STKTOPEFL | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("PUSH"),
        D_PUSH | D_CHGESP, 0,
        1, 0x000038FF, 0x000030FF, 0x00,
        B_INT | B_SHOWSIZE,
        B_PUSH | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("PUSH"),
        D_PUSH | D_CHGESP, 0,
        1, 0x000000F8, 0x00000050, 0x00,
        B_REGCMD,
        B_PUSH | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("PUSH"),
        D_PUSH | D_CHGESP, 0,
        1, 0x000000FF, 0x0000006A, 0x00,
        B_SXTCONST | B_SHOWSIZE,
        B_PUSH | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("PUSH"),
        D_PUSH | D_CHGESP, 0,
        1, 0x000000FF, 0x00000068, 0x00,
        B_CONSTL | B_SHOWSIZE,
        B_PUSH | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("PUSH"),
        D_PUSH | D_CHGESP | D_RARE, 0,
        1, 0x000000FF, 0x0000000E, 0x00,
        B_SEGCS,
        B_PUSH | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("PUSH"),
        D_PUSH | D_CHGESP | D_RARE, 0,
        1, 0x000000FF, 0x00000016, 0x00,
        B_SEGSS,
        B_PUSH | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("PUSH"),
        D_PUSH | D_CHGESP | D_RARE, 0,
        1, 0x000000FF, 0x0000001E, 0x00,
        B_SEGDS,
        B_PUSH | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("PUSH"),
        D_PUSH | D_CHGESP | D_RARE, 0,
        1, 0x000000FF, 0x00000006, 0x00,
        B_SEGES,
        B_PUSH | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("PUSH"),
        D_PUSH | D_CHGESP | D_RARE, 0,
        2, 0x0000FFFF, 0x0000A00F, 0x00,
        B_SEGFS,
        B_PUSH | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("PUSH"),
        D_PUSH | D_CHGESP | D_RARE, 0,
        2, 0x0000FFFF, 0x0000A80F, 0x00,
        B_SEGGS,
        B_PUSH | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("PUSHA*"),
        D_CMD | D_CHGESP | D_WILDCARD, 0,
        1, 0x000000FF, 0x00000060, 0x00,
        B_PUSHA | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("PUSHF*"),
        D_PUSH | D_CHGESP | D_WILDCARD, DX_JB,
        1, 0x000000FF, 0x0000009C, 0x00,
        B_EFL | B_PSEUDO,
        B_PUSH | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("RCL"),
        D_CMD | D_SIZE01 | D_FLAGSCO | D_USESCARRY, DX_JC,
        1, 0x000038FE, 0x000010D0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_1 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("RCL"),
        D_CMD | D_SIZE01 | D_FLAGSCO | D_USESCARRY, DX_JC,
        1, 0x000038FE, 0x000010D2, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CL | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("RCL"),
        D_CMD | D_SIZE01 | D_FLAGSCO | D_USESCARRY, DX_JC,
        1, 0x000038FE, 0x000010C0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("RCR"),
        D_CMD | D_SIZE01 | D_FLAGSCO | D_USESCARRY, DX_JC,
        1, 0x000038FE, 0x000018D0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_1 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("RCR"),
        D_CMD | D_SIZE01 | D_FLAGSCO | D_USESCARRY, DX_JC,
        1, 0x000038FE, 0x000018D2, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CL | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("RCR"),
        D_CMD | D_SIZE01 | D_FLAGSCO | D_USESCARRY, DX_JC,
        1, 0x000038FE, 0x000018C0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("ROL"),
        D_CMD | D_SIZE01 | D_FLAGSCO, DX_JC,
        1, 0x000038FE, 0x000000D0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_1 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("ROL"),
        D_CMD | D_SIZE01 | D_FLAGSCO, DX_JC,
        1, 0x000038FE, 0x000000D2, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CL | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("ROL"),
        D_CMD | D_SIZE01 | D_FLAGSCO, DX_JC,
        1, 0x000038FE, 0x000000C0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("ROR"),
        D_CMD | D_SIZE01 | D_FLAGSCO, DX_JC,
        1, 0x000038FE, 0x000008D0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_1 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("ROR"),
        D_CMD | D_SIZE01 | D_FLAGSCO, DX_JC,
        1, 0x000038FE, 0x000008D2, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CL | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("ROR"),
        D_CMD | D_SIZE01 | D_FLAGSCO, DX_JC,
        1, 0x000038FE, 0x000008C0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("RDMSR"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000320F, 0x00,
        B_EDX | B_BINARY | B_CHG | B_PSEUDO,
        B_EAX | B_BINARY | B_CHG | B_PSEUDO,
        B_ECX | B_PSEUDO,
        B_NONE
    },

    {
        T("RDPMC"),
        D_SYS | D_RARE, 0,
        2, 0x0000FFFF, 0x0000330F, 0x00,
        B_EDX | B_BINARY | B_CHG | B_PSEUDO,
        B_EAX | B_BINARY | B_CHG | B_PSEUDO,
        B_ECX | B_PSEUDO,
        B_NONE
    },

    {
        T("RDTSC"),
        D_SYS | D_RARE, 0,
        2, 0x0000FFFF, 0x0000310F, 0x00,
        B_EDX | B_BINARY | B_CHG | B_PSEUDO,
        B_EAX | B_BINARY | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("RDTSCP"),
        D_SYS | D_RARE, 0,
        3, 0x00FFFFFF, 0x00F9010F, 0x00,
        B_EDX | B_BINARY | B_CHG | B_PSEUDO,
        B_EAX | B_BINARY | B_CHG | B_PSEUDO,
        B_ECX | B_BINARY | B_CHG | B_PSEUDO,
        B_NONE
    },

    {
        T("RETN"),
        D_RET | D_NOREP | D_CHGESP, DX_RETN,
        1, 0x000000FF, 0x000000C3, 0x00,
        B_STKTOP | B_JMPCALL | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("RET"),
        D_RET | D_NOREP | D_CHGESP, 0,
        1, 0x000000FF, 0x000000C3, 0x00,
        B_STKTOP | B_JMPCALL | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("RETN"),
        D_RET | D_MUSTREP | D_CHGESP, DX_RETN,
        1, 0x000000FF, 0x000000C3, 0x00,
        B_STKTOP | B_JMPCALL | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("RET"),
        D_RET | D_MUSTREP | D_CHGESP, 0,
        1, 0x000000FF, 0x000000C3, 0x00,
        B_STKTOP | B_JMPCALL | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("RETN"),
        D_RET | D_CHGESP, DX_RETN,
        1, 0x000000FF, 0x000000C2, 0x00,
        B_STKTOP | B_JMPCALL | B_PSEUDO,
        B_CONST16 | B_STACKINC,
        B_NONE,
        B_NONE
    },

    {
        T("RET"),
        D_RET | D_CHGESP, 0,
        1, 0x000000FF, 0x000000C2, 0x00,
        B_STKTOP | B_JMPCALL | B_PSEUDO,
        B_CONST16 | B_STACKINC,
        B_NONE,
        B_NONE
    },

    {
        T("RETF"),
        D_RETFAR | D_CHGESP | D_RARE, 0,
        1, 0x000000FF, 0x000000CB, 0x00,
        B_STKTOPFAR | B_JMPCALLFAR | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("RETF"),
        D_RETFAR | D_CHGESP | D_RARE, 0,
        1, 0x000000FF, 0x000000CA, 0x00,
        B_STKTOPFAR | B_JMPCALLFAR | B_PSEUDO,
        B_CONST16 | B_STACKINC,
        B_NONE,
        B_NONE
    },

    {
        T("RSM"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000AA0F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SAHF"),
        D_CMD | D_ALLFLAGS, 0,
        1, 0x000000FF, 0x0000009E, 0x00,
        B_AH | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SHL"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000020D0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_1 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SHL"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000020D2, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CL | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SHL"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000020C0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SAL"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000020D0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_1 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SAL"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000020D2, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CL | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SAL"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000020C0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SAL"),
        D_CMD | D_SIZE01 | D_ALLFLAGS | D_UNDOC, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000030D0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_1 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SAL"),
        D_CMD | D_SIZE01 | D_ALLFLAGS | D_UNDOC, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000030D2, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CL | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SAL"),
        D_CMD | D_SIZE01 | D_ALLFLAGS | D_UNDOC, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000030C0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SALC"),
        D_CMD | D_ALLFLAGS | D_UNDOC, DX_JZ | DX_JC,
        1, 0x000000FF, 0x000000D6, 0x00,
        B_AL | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SHR"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000028D0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_1 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SHR"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000028D2, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CL | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SHR"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000028C0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SAR"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000038D0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_1 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SAR"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000038D2, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CL | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SAR"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JC,
        1, 0x000038FE, 0x000038C0, 0x00,
        B_INT | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST8 | B_SHIFTCNT,
        B_NONE,
        B_NONE
    },

    {
        T("SBB"),
        D_CMD | D_SIZE01 | D_ALLFLAGS | D_USESCARRY, DX_JZ | DX_JB,
        1, 0x000000FE, 0x0000001C, 0x00,
        B_ACC | B_UPD,
        B_CONST,
        B_NONE,
        B_NONE
    },

    {
        T("SBB"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS | D_USESCARRY, DX_JZ | DX_JB,
        1, 0x000038FE, 0x00001880, 0x00,
        B_INT | B_SHOWSIZE | B_UPD,
        B_CONST,
        B_NONE,
        B_NONE
    },

    {
        T("SBB"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS | D_USESCARRY, DX_JZ | DX_JB,
        1, 0x000038FE, 0x00001882, 0x00,
        B_INT | B_SHOWSIZE | B_UPD,
        B_SXTCONST,
        B_NONE,
        B_NONE
    },

    {
        T("SBB"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS | D_USESCARRY, DX_JZ | DX_JB,
        1, 0x000000FE, 0x00000018, 0x00,
        B_INT | B_UPD,
        B_REG,
        B_NONE,
        B_NONE
    },

    {
        T("SBB"),
        D_CMD | D_SIZE01 | D_ALLFLAGS | D_USESCARRY, DX_JZ | DX_JB,
        1, 0x000000FE, 0x0000001A, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("SETO"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x0000900F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNO"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x0000910F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETB"),
        D_SETC | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000920F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETC"),
        D_SETC | D_COND | D_USESCARRY, DX_JC,
        2, 0x0000FFFF, 0x0000920F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNAE"),
        D_SETC | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000920F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETAE"),
        D_SETC | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000930F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNB"),
        D_SETC | D_COND | D_USESCARRY, DX_JB,
        2, 0x0000FFFF, 0x0000930F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNC"),
        D_SETC | D_COND | D_USESCARRY, DX_JC,
        2, 0x0000FFFF, 0x0000930F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETE"),
        D_SETC | D_COND, DX_JE,
        2, 0x0000FFFF, 0x0000940F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETZ"),
        D_SETC | D_COND, DX_JZ,
        2, 0x0000FFFF, 0x0000940F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNE"),
        D_SETC | D_COND, DX_JE,
        2, 0x0000FFFF, 0x0000950F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNZ"),
        D_SETC | D_COND, DX_JZ,
        2, 0x0000FFFF, 0x0000950F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETBE"),
        D_SETC | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000960F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNA"),
        D_SETC | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000960F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETA"),
        D_SETC | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000970F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNBE"),
        D_SETC | D_COND | D_USESCARRY, 0,
        2, 0x0000FFFF, 0x0000970F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETS"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x0000980F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNS"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x0000990F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETPE"),
        D_SETC | D_COND | D_RARE, 0,
        2, 0x0000FFFF, 0x00009A0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETP"),
        D_SETC | D_COND | D_RARE, 0,
        2, 0x0000FFFF, 0x00009A0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETPO"),
        D_SETC | D_COND | D_RARE, 0,
        2, 0x0000FFFF, 0x00009B0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNP"),
        D_SETC | D_COND | D_RARE, 0,
        2, 0x0000FFFF, 0x00009B0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETL"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x00009C0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNGE"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x00009C0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETGE"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x00009D0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNL"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x00009D0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETLE"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x00009E0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNG"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x00009E0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETG"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x00009F0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SETNLE"),
        D_SETC | D_COND, 0,
        2, 0x0000FFFF, 0x00009F0F, 0x00,
        B_INT8 | B_CHG,
        B_ANYREG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SGDT"),
        D_SYS | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0000010F, 0x00,
        B_DESCR | B_MEMONLY | B_CHG,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SIDT"),
        D_SYS | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0008010F, 0x00,
        B_DESCR | B_MEMONLY | B_CHG,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SHLD"),
        D_CMD | D_ALLFLAGS, DX_JZ | DX_JC,
        2, 0x0000FFFF, 0x0000A40F, 0x00,
        B_INT | B_NOESP | B_UPD,
        B_REG,
        B_CONST8 | B_SHIFTCNT,
        B_NONE
    },

    {
        T("SHLD"),
        D_CMD | D_ALLFLAGS, DX_JZ | DX_JC,
        2, 0x0000FFFF, 0x0000A50F, 0x00,
        B_INT | B_NOESP | B_UPD,
        B_REG,
        B_CL | B_SHIFTCNT,
        B_NONE
    },

    {
        T("SHRD"),
        D_CMD | D_ALLFLAGS, DX_JZ | DX_JC,
        2, 0x0000FFFF, 0x0000AC0F, 0x00,
        B_INT | B_NOESP | B_UPD,
        B_REG,
        B_CONST8 | B_SHIFTCNT,
        B_NONE
    },

    {
        T("SHRD"),
        D_CMD | D_ALLFLAGS, DX_JZ | DX_JC,
        2, 0x0000FFFF, 0x0000AD0F, 0x00,
        B_INT | B_NOESP | B_UPD,
        B_REG,
        B_CL | B_SHIFTCNT,
        B_NONE
    },

    {
        T("SLDT"),
        D_SYS | D_RARE, 0,
        2, 0x0038FFFF, 0x0000000F, 0x00,
        B_INT | B_NOESP | B_CHG,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SMSW"),
        D_SYS | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0020010F, 0x00,
        B_INT16 | B_MEMONLY | B_CHG,
        B_CR0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SMSW"),
        D_SYS | D_REGISTER | D_RARE, 0,
        2, 0x0038FFFF, 0x0020010F, 0x00,
        B_INT | B_REGONLY | B_NOESP | B_CHG,
        B_CR0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("STC"),
        D_CMD | D_FLAGC, 0,
        1, 0x000000FF, 0x000000F9, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("STD"),
        D_CMD | D_FLAGD, 0,
        1, 0x000000FF, 0x000000FD, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("STI"),
        D_CMD | D_RARE, 0,
        1, 0x000000FF, 0x000000FB, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("STMXCSR"),
        D_CMD | D_MEMORY, 0,
        2, 0x0038FFFF, 0x0018AE0F, 0x00,
        B_INT32 | B_BINARY | B_MEMONLY | B_NOESP | B_SHOWSIZE | B_CHG,
        B_MXCSR | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("STR"),
        D_SYS | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0008000F, 0x00,
        B_INT16 | B_MEMONLY | B_CHG,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("STR"),
        D_SYS | D_REGISTER | D_RARE, 0,
        2, 0x0038FFFF, 0x0008000F, 0x00,
        B_INT | B_REGONLY | B_NOESP | B_CHG,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SUB"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JB | DX_SUB,
        1, 0x000000FE, 0x0000002C, 0x00,
        B_ACC | B_UPD,
        B_CONST,
        B_NONE,
        B_NONE
    },

    {
        T("SUB"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ | DX_JB | DX_SUB,
        1, 0x000038FE, 0x00002880, 0x00,
        B_INT | B_SHOWSIZE | B_UPD,
        B_CONST,
        B_NONE,
        B_NONE
    },

    {
        T("SUB"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ | DX_JB | DX_SUB,
        1, 0x000038FE, 0x00002882, 0x00,
        B_INT | B_SHOWSIZE | B_UPD,
        B_SXTCONST,
        B_NONE,
        B_NONE
    },

    {
        T("SUB"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ | DX_JB | DX_SUB,
        1, 0x000000FE, 0x00000028, 0x00,
        B_INT | B_UPD,
        B_REG,
        B_NONE,
        B_NONE
    },

    {
        T("SUB"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ | DX_JB | DX_SUB,
        1, 0x000000FE, 0x0000002A, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("SYSENTER"),
        D_SYS | D_RARE, 0,
        2, 0x0000FFFF, 0x0000340F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SYSEXIT"),
        D_SYS | D_ALLFLAGS | D_SUSPICIOUS, 0,
        2, 0x0000FFFF, 0x0000350F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("TEST"),
        D_TEST | D_SIZE01 | D_ALLFLAGS, DX_JZ,
        1, 0x000000FE, 0x000000A8, 0x00,
        B_ACC | B_BINARY,
        B_CONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("TEST"),
        D_TEST | D_SIZE01 | D_ALLFLAGS, DX_JZ,
        1, 0x000038FE, 0x000000F6, 0x00,
        B_INT | B_BINARY | B_SHOWSIZE,
        B_CONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("TEST"),
        D_TEST | D_SIZE01 | D_ALLFLAGS | D_UNDOC, DX_JZ,
        1, 0x000038FE, 0x000008F6, 0x00,
        B_INT | B_BINARY | B_SHOWSIZE,
        B_CONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("TEST"),
        D_TEST | D_SIZE01 | D_ALLFLAGS, DX_JZ,
        1, 0x000000FE, 0x00000084, 0x00,
        B_INT | B_BINARY,
        B_REG | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("TEST"),
        D_TEST | D_SIZE01 | D_ALLFLAGS, DX_JZ,
        1, 0x000000FE, 0x00000084, 0x00,
        B_REG | B_BINARY,
        B_INT | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("UD1"),
        D_CMD | D_UNDOC, 0,
        2, 0x0000FFFF, 0x0000B90F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("UD2"),
        D_CMD, 0,
        2, 0x0000FFFF, 0x00000B0F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VERR"),
        D_CMD | D_FLAGZ | D_RARE, 0,
        2, 0x0038FFFF, 0x0020000F, 0x00,
        B_INT16 | B_NOESP,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VERW"),
        D_CMD | D_FLAGZ | D_RARE, 0,
        2, 0x0038FFFF, 0x0028000F, 0x00,
        B_INT16 | B_NOESP,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("WBINVD"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000090F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("WRMSR"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000300F, 0x00,
        B_EDX | B_BINARY | B_PSEUDO,
        B_EAX | B_BINARY | B_PSEUDO,
        B_ECX | B_PSEUDO,
        B_NONE
    },

    {
        T("XADD"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JE | DX_JB,
        2, 0x0000FEFF, 0x0000C00F, 0x00,
        B_INT | B_UPD,
        B_REG | B_CHG,
        B_NONE,
        B_NONE
    },

    {
        T("XCHG"),
        D_MOV | D_LOCKABLE, 0,
        1, 0x000000F8, 0x00000090, 0x00,
        B_ACC | B_CHG,
        B_REGCMD | B_CHG,
        B_NONE,
        B_NONE
    },

    {
        T("XCHG"),
        D_MOV, 0,
        1, 0x000000F8, 0x00000090, 0x00,
        B_REGCMD | B_CHG,
        B_ACC | B_CHG,
        B_NONE,
        B_NONE
    },

    {
        T("XCHG"),
        D_MOV | D_SIZE01 | D_LOCKABLE, 0,
        1, 0x000000FE, 0x00000086, 0x00,
        B_INT | B_CHG,
        B_REG | B_CHG,
        B_NONE,
        B_NONE
    },

    {
        T("XCHG"),
        D_MOV | D_SIZE01 | D_LOCKABLE, 0,
        1, 0x000000FE, 0x00000086, 0x00,
        B_REG | B_CHG,
        B_INT | B_CHG,
        B_NONE,
        B_NONE
    },

    {
        T("XLAT"),
        D_CMD, 0,
        1, 0x000000FF, 0x000000D7, 0x00,
        B_AL | B_CHG | B_PSEUDO,
        B_XLATMEM,
        B_NONE,
        B_NONE
    },

    {
        T("XLATB"),
        D_CMD, 0,
        1, 0x000000FF, 0x000000D7, 0x00,
        B_AL | B_UPD | B_PSEUDO,
        B_XLATMEM | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("XOR"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ,
        1, 0x000000FE, 0x00000034, 0x00,
        B_ACC | B_BINARY | B_UPD,
        B_CONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("XOR"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ,
        1, 0x000038FE, 0x00003080, 0x00,
        B_INT | B_BINARY | B_NOESP | B_SHOWSIZE | B_UPD,
        B_CONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("XOR"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ,
        1, 0x000038FE, 0x00003082, 0x00,
        B_INT | B_BINARY | B_NOESP | B_SHOWSIZE | B_UPD,
        B_SXTCONST | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("XOR"),
        D_CMD | D_SIZE01 | D_LOCKABLE | D_ALLFLAGS, DX_JZ,
        1, 0x000000FE, 0x00000030, 0x00,
        B_INT | B_BINARY | B_UPD,
        B_REG | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("XOR"),
        D_CMD | D_SIZE01 | D_ALLFLAGS, DX_JZ,
        1, 0x000000FE, 0x00000032, 0x00,
        B_REG | B_BINARY | B_UPD,
        B_INT | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("CMPS"),
        D_CMD | D_SIZE01 | D_LONGFORM | D_NOREP | D_ALLFLAGS | D_HLADIR, DX_JE | DX_JB,
        1, 0x000000FE, 0x000000A6, 0x00,
        B_STRSRC | B_SHOWSIZE,
        B_STRDEST | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("CMPSB"),
        D_CMD | D_NOREP | D_ALLFLAGS, DX_JE | DX_JB,
        1, 0x000000FF, 0x000000A6, 0x00,
        B_STRSRC8 | B_PSEUDO,
        B_STRDEST8 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("CMPS*"),
        D_CMD | D_NOREP | D_ALLFLAGS | D_WILDCARD, DX_JE | DX_JB,
        1, 0x000000FF, 0x000000A7, 0x00,
        B_STRSRC | B_PSEUDO,
        B_STRDEST | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("CMPS"),
        D_STRING | D_SIZE01 | D_LONGFORM | D_MUSTREPE | D_ALLFLAGS | D_HLADIR, 0,
        1, 0x000000FE, 0x000000A6, 0x00,
        B_STRSRC | B_SHOWSIZE,
        B_STRDEST | B_SHOWSIZE,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("CMPSB"),
        D_STRING | D_MUSTREPE | D_ALLFLAGS, 0,
        1, 0x000000FF, 0x000000A6, 0x00,
        B_STRSRC8 | B_PSEUDO,
        B_STRDEST8 | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("CMPS*"),
        D_STRING | D_MUSTREPE | D_ALLFLAGS | D_WILDCARD, 0,
        1, 0x000000FF, 0x000000A7, 0x00,
        B_STRSRC | B_PSEUDO,
        B_STRDEST | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("CMPS"),
        D_STRING | D_SIZE01 | D_LONGFORM | D_MUSTREPNE | D_ALLFLAGS | D_HLADIR, 0,
        1, 0x000000FE, 0x000000A6, 0x00,
        B_STRSRC | B_SHOWSIZE,
        B_STRDEST | B_SHOWSIZE,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("CMPSB"),
        D_STRING | D_MUSTREPNE | D_ALLFLAGS, 0,
        1, 0x000000FF, 0x000000A6, 0x00,
        B_STRSRC8 | B_PSEUDO,
        B_STRDEST8 | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("CMPS*"),
        D_STRING | D_MUSTREPNE | D_ALLFLAGS | D_WILDCARD, 0,
        1, 0x000000FF, 0x000000A7, 0x00,
        B_STRSRC | B_PSEUDO,
        B_STRDEST | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("LODS"),
        D_CMD | D_SIZE01 | D_LONGFORM | D_NOREP, 0,
        1, 0x000000FE, 0x000000AC, 0x00,
        B_ACC | B_CHG | B_PSEUDO,
        B_STRSRC | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("LODSB"),
        D_CMD | D_NOREP, 0,
        1, 0x000000FF, 0x000000AC, 0x00,
        B_AL | B_CHG | B_PSEUDO,
        B_STRSRC8 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("LODS*"),
        D_CMD | D_NOREP | D_WILDCARD, 0,
        1, 0x000000FF, 0x000000AD, 0x00,
        B_ACC | B_CHG | B_PSEUDO,
        B_STRSRC | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("LODS"),
        D_STRING | D_SIZE01 | D_LONGFORM | D_MUSTREP | D_RARE, 0,
        1, 0x000000FE, 0x000000AC, 0x00,
        B_ACC | B_CHG | B_PSEUDO,
        B_STRSRC | B_SHOWSIZE,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("LODSB"),
        D_STRING | D_MUSTREP | D_RARE, 0,
        1, 0x000000FF, 0x000000AC, 0x00,
        B_AL | B_CHG | B_PSEUDO,
        B_STRSRC8 | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("LODS*"),
        D_STRING | D_MUSTREP | D_WILDCARD | D_RARE, 0,
        1, 0x000000FF, 0x000000AD, 0x00,
        B_ACC | B_CHG | B_PSEUDO,
        B_STRSRC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("LODS"),
        D_STRING | D_SIZE01 | D_LONGFORM | D_MUSTREPNE | D_UNDOC, 0,
        1, 0x000000FE, 0x000000AC, 0x00,
        B_ACC | B_CHG | B_PSEUDO,
        B_STRSRC | B_SHOWSIZE,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("LODSB"),
        D_STRING | D_MUSTREPNE | D_UNDOC, 0,
        1, 0x000000FF, 0x000000AC, 0x00,
        B_AL | B_CHG | B_PSEUDO,
        B_STRSRC8 | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("LODS*"),
        D_STRING | D_MUSTREPNE | D_WILDCARD | D_UNDOC, 0,
        1, 0x000000FF, 0x000000AD, 0x00,
        B_ACC | B_CHG | B_PSEUDO,
        B_STRSRC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("MOVS"),
        D_CMD | D_SIZE01 | D_LONGFORM | D_NOREP, 0,
        1, 0x000000FE, 0x000000A4, 0x00,
        B_STRDEST | B_SHOWSIZE | B_CHG,
        B_STRSRC | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("MOVSB"),
        D_CMD | D_NOREP, 0,
        1, 0x000000FF, 0x000000A4, 0x00,
        B_STRDEST8 | B_CHG | B_PSEUDO,
        B_STRSRC8 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("MOVS*"),
        D_CMD | D_NOREP | D_WILDCARD, 0,
        1, 0x000000FF, 0x000000A5, 0x00,
        B_STRDEST | B_CHG | B_PSEUDO,
        B_STRSRC | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("MOVS"),
        D_STRING | D_SIZE01 | D_LONGFORM | D_MUSTREP, 0,
        1, 0x000000FE, 0x000000A4, 0x00,
        B_STRDEST | B_SHOWSIZE | B_CHG,
        B_STRSRC | B_SHOWSIZE,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("MOVSB"),
        D_STRING | D_MUSTREP, 0,
        1, 0x000000FF, 0x000000A4, 0x00,
        B_STRDEST8 | B_CHG | B_PSEUDO,
        B_STRSRC8 | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("MOVS*"),
        D_STRING | D_MUSTREP | D_WILDCARD, 0,
        1, 0x000000FF, 0x000000A5, 0x00,
        B_STRDEST | B_CHG | B_PSEUDO,
        B_STRSRC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("MOVS"),
        D_STRING | D_SIZE01 | D_LONGFORM | D_MUSTREPNE | D_UNDOC, 0,
        1, 0x000000FE, 0x000000A4, 0x00,
        B_STRDEST | B_SHOWSIZE | B_CHG,
        B_STRSRC | B_SHOWSIZE,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("MOVSB"),
        D_STRING | D_MUSTREPNE | D_UNDOC, 0,
        1, 0x000000FF, 0x000000A4, 0x00,
        B_STRDEST8 | B_CHG | B_PSEUDO,
        B_STRSRC8 | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("MOVS*"),
        D_STRING | D_MUSTREPNE | D_WILDCARD | D_UNDOC, 0,
        1, 0x000000FF, 0x000000A5, 0x00,
        B_STRDEST | B_CHG | B_PSEUDO,
        B_STRSRC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("SCAS"),
        D_CMD | D_SIZE01 | D_LONGFORM | D_NOREP | D_ALLFLAGS, DX_JE | DX_JB,
        1, 0x000000FE, 0x000000AE, 0x00,
        B_STRDEST | B_SHOWSIZE,
        B_ACC | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SCASB"),
        D_CMD | D_NOREP | D_ALLFLAGS, DX_JE | DX_JB,
        1, 0x000000FF, 0x000000AE, 0x00,
        B_STRDEST8 | B_PSEUDO,
        B_AL | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SCAS*"),
        D_CMD | D_NOREP | D_ALLFLAGS | D_WILDCARD, DX_JE | DX_JB,
        1, 0x000000FF, 0x000000AF, 0x00,
        B_STRDEST | B_PSEUDO,
        B_ACC | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SCAS"),
        D_STRING | D_SIZE01 | D_LONGFORM | D_MUSTREPE | D_ALLFLAGS, 0,
        1, 0x000000FE, 0x000000AE, 0x00,
        B_STRDEST | B_SHOWSIZE,
        B_ACC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("SCASB"),
        D_STRING | D_MUSTREPE | D_ALLFLAGS, 0,
        1, 0x000000FF, 0x000000AE, 0x00,
        B_STRDEST8 | B_PSEUDO,
        B_AL | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("SCAS*"),
        D_STRING | D_MUSTREPE | D_ALLFLAGS | D_WILDCARD, 0,
        1, 0x000000FF, 0x000000AF, 0x00,
        B_STRDEST | B_PSEUDO,
        B_ACC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("SCAS"),
        D_STRING | D_SIZE01 | D_LONGFORM | D_MUSTREPNE | D_ALLFLAGS, 0,
        1, 0x000000FE, 0x000000AE, 0x00,
        B_STRDEST | B_SHOWSIZE,
        B_ACC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("SCASB"),
        D_STRING | D_MUSTREPNE | D_ALLFLAGS, 0,
        1, 0x000000FF, 0x000000AE, 0x00,
        B_STRDEST8 | B_PSEUDO,
        B_AL | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("SCAS*"),
        D_STRING | D_MUSTREPNE | D_ALLFLAGS | D_WILDCARD, 0,
        1, 0x000000FF, 0x000000AF, 0x00,
        B_STRDEST | B_PSEUDO,
        B_ACC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("STOS"),
        D_CMD | D_SIZE01 | D_LONGFORM | D_NOREP, 0,
        1, 0x000000FE, 0x000000AA, 0x00,
        B_STRDEST | B_SHOWSIZE | B_CHG,
        B_ACC | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("STOSB"),
        D_CMD | D_NOREP, 0,
        1, 0x000000FF, 0x000000AA, 0x00,
        B_STRDEST8 | B_CHG | B_PSEUDO,
        B_AL | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("STOS*"),
        D_CMD | D_NOREP | D_WILDCARD, 0,
        1, 0x000000FF, 0x000000AB, 0x00,
        B_STRDEST | B_CHG | B_PSEUDO,
        B_ACC | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("STOS"),
        D_STRING | D_SIZE01 | D_LONGFORM | D_MUSTREP, 0,
        1, 0x000000FE, 0x000000AA, 0x00,
        B_STRDEST | B_SHOWSIZE | B_CHG,
        B_ACC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("STOSB"),
        D_STRING | D_MUSTREP, 0,
        1, 0x000000FF, 0x000000AA, 0x00,
        B_STRDEST8 | B_CHG | B_PSEUDO,
        B_AL | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("STOS*"),
        D_STRING | D_MUSTREP | D_WILDCARD, 0,
        1, 0x000000FF, 0x000000AB, 0x00,
        B_STRDEST | B_CHG | B_PSEUDO,
        B_ACC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("STOS"),
        D_STRING | D_SIZE01 | D_LONGFORM | D_MUSTREPNE | D_UNDOC, 0,
        1, 0x000000FE, 0x000000AA, 0x00,
        B_STRDEST | B_SHOWSIZE | B_CHG,
        B_ACC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("STOSB"),
        D_STRING | D_MUSTREPNE | D_UNDOC, 0,
        1, 0x000000FF, 0x000000AA, 0x00,
        B_STRDEST8 | B_CHG | B_PSEUDO,
        B_AL | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("STOS*"),
        D_STRING | D_MUSTREPNE | D_WILDCARD | D_UNDOC, 0,
        1, 0x000000FF, 0x000000AB, 0x00,
        B_STRDEST | B_CHG | B_PSEUDO,
        B_ACC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("INS"),
        D_IO | D_SIZE01 | D_LONGFORM | D_NOREP | D_RARE, 0,
        1, 0x000000FE, 0x0000006C, 0x00,
        B_STRDEST | B_SHOWSIZE | B_CHG,
        B_DXPORT | B_PORT,
        B_NONE,
        B_NONE
    },

    {
        T("INSB"),
        D_IO | D_NOREP | D_RARE, 0,
        1, 0x000000FF, 0x0000006C, 0x00,
        B_STRDEST8 | B_CHG | B_PSEUDO,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("INS*"),
        D_IO | D_NOREP | D_WILDCARD | D_RARE, 0,
        1, 0x000000FF, 0x0000006D, 0x00,
        B_STRDEST | B_CHG | B_PSEUDO,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("INS"),
        D_IO | D_SIZE01 | D_LONGFORM | D_MUSTREP | D_RARE, 0,
        1, 0x000000FE, 0x0000006C, 0x00,
        B_STRDEST | B_SHOWSIZE | B_CHG,
        B_DXPORT | B_PORT,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("INSB"),
        D_IO | D_MUSTREP | D_RARE, 0,
        1, 0x000000FF, 0x0000006C, 0x00,
        B_STRDEST8 | B_CHG | B_PSEUDO,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("INS*"),
        D_IO | D_MUSTREP | D_WILDCARD | D_RARE, 0,
        1, 0x000000FF, 0x0000006D, 0x00,
        B_STRDEST | B_CHG | B_PSEUDO,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("INS"),
        D_IO | D_SIZE01 | D_LONGFORM | D_MUSTREPNE | D_UNDOC, 0,
        1, 0x000000FE, 0x0000006C, 0x00,
        B_STRDEST | B_SHOWSIZE | B_CHG,
        B_DXPORT | B_PORT,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("INSB"),
        D_IO | D_MUSTREPNE | D_UNDOC, 0,
        1, 0x000000FF, 0x0000006C, 0x00,
        B_STRDEST8 | B_CHG | B_PSEUDO,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("INS*"),
        D_IO | D_MUSTREPNE | D_WILDCARD | D_UNDOC, 0,
        1, 0x000000FF, 0x0000006D, 0x00,
        B_STRDEST | B_CHG | B_PSEUDO,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("OUTS"),
        D_IO | D_SIZE01 | D_LONGFORM | D_NOREP | D_RARE, 0,
        1, 0x000000FE, 0x0000006E, 0x00,
        B_DXPORT | B_PORT,
        B_STRSRC | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("OUTSB"),
        D_IO | D_NOREP | D_RARE, 0,
        1, 0x000000FF, 0x0000006E, 0x00,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_STRSRC8 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("OUTS*"),
        D_IO | D_NOREP | D_WILDCARD | D_RARE, 0,
        1, 0x000000FF, 0x0000006F, 0x00,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_STRSRC | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("OUTS"),
        D_IO | D_SIZE01 | D_LONGFORM | D_MUSTREP | D_RARE, 0,
        1, 0x000000FE, 0x0000006E, 0x00,
        B_DXPORT | B_PORT,
        B_STRSRC | B_SHOWSIZE,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("OUTSB"),
        D_IO | D_MUSTREP | D_RARE, 0,
        1, 0x000000FF, 0x0000006E, 0x00,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_STRSRC8 | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("OUTS*"),
        D_IO | D_MUSTREP | D_WILDCARD | D_RARE, 0,
        1, 0x000000FF, 0x0000006F, 0x00,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_STRSRC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("OUTS"),
        D_IO | D_SIZE01 | D_LONGFORM | D_MUSTREPNE | D_UNDOC, 0,
        1, 0x000000FE, 0x0000006E, 0x00,
        B_DXPORT | B_PORT,
        B_STRSRC | B_SHOWSIZE,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("OUTSB"),
        D_IO | D_MUSTREPNE | D_UNDOC, 0,
        1, 0x000000FF, 0x0000006E, 0x00,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_STRSRC8 | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("OUTS*"),
        D_IO | D_MUSTREPNE | D_WILDCARD | D_UNDOC, 0,
        1, 0x000000FF, 0x0000006F, 0x00,
        B_DXPORT | B_PORT | B_PSEUDO,
        B_STRSRC | B_PSEUDO,
        B_STRCNT | B_UPD | B_PSEUDO,
        B_NONE
    },

    {
        T("MOVBE"),
        D_CMD | D_NOREP, 0,
        3, 0x00FFFFFF, 0x00F0380F, 0x00,
        B_REG | B_CHG,
        B_INT | B_MEMONLY,
        B_NONE,
        B_NONE
    },

    {
        T("MOVBE"),
        D_CMD | D_NOREP, 0,
        3, 0x00FFFFFF, 0x00F1380F, 0x00,
        B_INT | B_MEMONLY | B_CHG,
        B_REG,
        B_NONE,
        B_NONE
    },

    {
        T("XGETBV"),
        D_SYS | D_MUSTNONE | D_RARE, 0,
        3, 0x00FFFFFF, 0x00D0010F, 0x00,
        B_EAX | B_CHG | B_PSEUDO,
        B_EDX | B_CHG | B_PSEUDO,
        B_ECX | B_PSEUDO,
        B_NONE
    },

    {
        T("XSETBV"),
        D_PRIVILEGED | D_MUSTNONE | D_RARE, 0,
        3, 0x00FFFFFF, 0x00D1010F, 0x00,
        B_EAX | B_PSEUDO,
        B_EDX | B_PSEUDO,
        B_ECX | B_PSEUDO,
        B_NONE
    },

    {
        T("XRSTOR"),
        D_SYS | D_MUSTNONE | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0028AE0F, 0x00,
        B_ANYMEM | B_MEMONLY,
        B_EAX | B_PSEUDO,
        B_EDX | B_PSEUDO,
        B_NONE
    },

    {
        T("XSAVE"),
        D_SYS | D_MUSTNONE | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0020AE0F, 0x00,
        B_ANYMEM | B_MEMONLY | B_CHG,
        B_EAX | B_PSEUDO,
        B_EDX | B_PSEUDO,
        B_NONE
    },

    {
        T("F2XM1"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000F0D9, 0x00,
        B_ST0 | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FABS"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000E1D9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FCHS"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000E0D9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FCLEX"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000E2DB, 0x00,
        B_FST | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FCOMPP"),
        D_FPU | D_FPUPOP2, 0,
        2, 0x0000FFFF, 0x0000D9DE, 0x00,
        B_ST0 | B_PSEUDO,
        B_ST1 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FCOS"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000FFD9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FDECSTP"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000FFFF, 0x0000F6D9, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FINCSTP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000F7D9, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FINIT"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000E3DB, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FLD1"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000FFFF, 0x0000E8D9, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FLDL2T"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000FFFF, 0x0000E9D9, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FLDL2E"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000FFFF, 0x0000EAD9, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FLDPI"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000FFFF, 0x0000EBD9, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FLDLG2"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000FFFF, 0x0000ECD9, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FLDLN2"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000FFFF, 0x0000EDD9, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FLDZ"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000FFFF, 0x0000EED9, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FNOP"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000D0D9, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FPATAN"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000F3D9, 0x00,
        B_ST1 | B_UPD | B_PSEUDO,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FPREM"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000F8D9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_ST1 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FPREM1"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000F5D9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_ST1 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FPTAN"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000FFFF, 0x0000F2D9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FRNDINT"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000FCD9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FSCALE"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000FDD9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_ST1 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FSIN"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000FED9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FSINCOS"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000FFFF, 0x0000FBD9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FSQRT"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000FAD9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FSTSW"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000E0DF, 0x00,
        B_AX | B_CHG,
        B_FST | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FTST"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000E4D9, 0x00,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FUCOMPP"),
        D_FPU | D_FPUPOP2, 0,
        2, 0x0000FFFF, 0x0000E9DA, 0x00,
        B_ST0 | B_PSEUDO,
        B_ST1 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FXAM"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000E5D9, 0x00,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FXTRACT"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000FFFF, 0x0000F4D9, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FYL2X"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000F1D9, 0x00,
        B_ST1 | B_UPD | B_PSEUDO,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FYL2XP1"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000F9D9, 0x00,
        B_ST1 | B_UPD | B_PSEUDO,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FENI"),
        D_FPU | D_RARE, 0,
        2, 0x0000FFFF, 0x0000E0DB, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FDISI"),
        D_FPU | D_RARE, 0,
        2, 0x0000FFFF, 0x0000E1DB, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FADD"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000C0D8, 0x00,
        B_ST0 | B_UPD,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FADD"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000C0DC, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FADDP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000F8FF, 0x0000C0DE, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FADDP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000C1DE, 0x00,
        B_ST1 | B_UPD | B_PSEUDO,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FCMOVB"),
        D_FPU | D_COND, 0,
        2, 0x0000F8FF, 0x0000C0DA, 0x00,
        B_ST0 | B_CHG,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCMOVE"),
        D_FPU | D_COND, 0,
        2, 0x0000F8FF, 0x0000C8DA, 0x00,
        B_ST0 | B_CHG,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCMOVBE"),
        D_FPU | D_COND, 0,
        2, 0x0000F8FF, 0x0000D0DA, 0x00,
        B_ST0 | B_CHG,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCMOVU"),
        D_FPU | D_COND, 0,
        2, 0x0000F8FF, 0x0000D8DA, 0x00,
        B_ST0 | B_CHG,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCMOVNB"),
        D_FPU | D_COND, 0,
        2, 0x0000F8FF, 0x0000C0DB, 0x00,
        B_ST0 | B_CHG,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCMOVNE"),
        D_FPU | D_COND, 0,
        2, 0x0000F8FF, 0x0000C8DB, 0x00,
        B_ST0 | B_CHG,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCMOVNBE"),
        D_FPU | D_COND, 0,
        2, 0x0000F8FF, 0x0000D0DB, 0x00,
        B_ST0 | B_CHG,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCMOVNU"),
        D_FPU | D_COND, 0,
        2, 0x0000F8FF, 0x0000D8DB, 0x00,
        B_ST0 | B_CHG,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCOM"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000D0D8, 0x00,
        B_ST0 | B_PSEUDO,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCOM"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000D1D8, 0x00,
        B_ST0 | B_PSEUDO,
        B_ST1 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FCOMP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000F8FF, 0x0000D8D8, 0x00,
        B_ST0 | B_PSEUDO,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCOMP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000D9D8, 0x00,
        B_ST0 | B_PSEUDO,
        B_ST1 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FCOMI"),
        D_FPU | D_FLAGSZPC, 0,
        2, 0x0000F8FF, 0x0000F0DB, 0x00,
        B_ST0,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCOMIP"),
        D_FPU | D_FLAGSZPC | D_FPUPOP, 0,
        2, 0x0000F8FF, 0x0000F0DF, 0x00,
        B_ST0,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FUCOMI"),
        D_FPU | D_FLAGSZPC, 0,
        2, 0x0000F8FF, 0x0000E8DB, 0x00,
        B_ST0,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FUCOMIP"),
        D_FPU | D_FLAGSZPC | D_FPUPOP, 0,
        2, 0x0000F8FF, 0x0000E8DF, 0x00,
        B_ST0,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FDIV"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000F0D8, 0x00,
        B_ST0 | B_UPD,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FDIV"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000F8DC, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FDIVP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000F8FF, 0x0000F8DE, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FDIVP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000F9DE, 0x00,
        B_ST1 | B_UPD | B_PSEUDO,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FDIVR"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000F8D8, 0x00,
        B_ST0 | B_UPD,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FDIVR"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000F0DC, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FDIVRP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000F8FF, 0x0000F0DE, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FDIVRP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000F1DE, 0x00,
        B_ST1 | B_UPD | B_PSEUDO,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FFREE"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000C0DD, 0x00,
        B_ST,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FFREEP"),
        D_FPU | D_FPUPOP | D_UNDOC, 0,
        2, 0x0000F8FF, 0x0000C0DF, 0x00,
        B_ST,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FLD"),
        D_FPU | D_FPUPUSH, 0,
        2, 0x0000F8FF, 0x0000C0D9, 0x00,
        B_ST,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FMUL"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000C8D8, 0x00,
        B_ST0 | B_UPD,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FMUL"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000C8DC, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FMULP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000F8FF, 0x0000C8DE, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FMULP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000C9DE, 0x00,
        B_ST1 | B_UPD | B_PSEUDO,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FST"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000D0DD, 0x00,
        B_ST | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FSTP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000F8FF, 0x0000D8DD, 0x00,
        B_ST | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FSUB"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000E0D8, 0x00,
        B_ST0 | B_UPD,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FSUB"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000E8DC, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FSUBP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000F8FF, 0x0000E8DE, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FSUBP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000E9DE, 0x00,
        B_ST1 | B_UPD | B_PSEUDO,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FSUBR"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000E8D8, 0x00,
        B_ST0 | B_UPD,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FSUBR"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000E0DC, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FSUBRP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000F8FF, 0x0000E0DE, 0x00,
        B_ST | B_UPD,
        B_ST0,
        B_NONE,
        B_NONE
    },

    {
        T("FSUBRP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000E1DE, 0x00,
        B_ST1 | B_UPD | B_PSEUDO,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FUCOM"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000E0DD, 0x00,
        B_ST0 | B_PSEUDO,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FUCOM"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000E1DD, 0x00,
        B_ST0 | B_PSEUDO,
        B_ST1 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FUCOMP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000F8FF, 0x0000E8DD, 0x00,
        B_ST0 | B_PSEUDO,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FUCOMP"),
        D_FPU | D_FPUPOP, 0,
        2, 0x0000FFFF, 0x0000E9DD, 0x00,
        B_ST0 | B_PSEUDO,
        B_ST1 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FXCH"),
        D_FPU, 0,
        2, 0x0000F8FF, 0x0000C8D9, 0x00,
        B_ST0 | B_CHG | B_PSEUDO,
        B_ST | B_CHG,
        B_NONE,
        B_NONE
    },

    {
        T("FXCH"),
        D_FPU, 0,
        2, 0x0000FFFF, 0x0000C9D9, 0x00,
        B_ST0 | B_CHG | B_PSEUDO,
        B_ST1 | B_CHG | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FADD"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000000D8, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT32 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FADD"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000000DC, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT64 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FIADD"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000000DA, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FIADD"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000000DE, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FBLD"),
        D_FPU | D_MEMORY | D_FPUPUSH | D_RARE, 0,
        1, 0x000038FF, 0x000020DF, 0x00,
        B_BCD | B_MEMORY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FBSTP"),
        D_FPU | D_MEMORY | D_FPUPOP | D_RARE, 0,
        1, 0x000038FF, 0x000030DF, 0x00,
        B_BCD | B_MEMORY | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FCOM"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000010D8, 0x00,
        B_ST0 | B_PSEUDO,
        B_FLOAT32 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FCOM"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000010DC, 0x00,
        B_ST0 | B_PSEUDO,
        B_FLOAT64 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FCOMP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000018D8, 0x00,
        B_ST0 | B_PSEUDO,
        B_FLOAT32 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FCOMP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000018DC, 0x00,
        B_ST0 | B_PSEUDO,
        B_FLOAT64 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FDIV"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000030D8, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT32 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FDIV"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000030DC, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT64 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FIDIV"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000030DA, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FIDIV"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000030DE, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FDIVR"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000038D8, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT32 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FDIVR"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000038DC, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT64 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FIDIVR"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000038DA, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FIDIVR"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000038DE, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FICOM"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000010DE, 0x00,
        B_ST0 | B_PSEUDO,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FICOM"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000010DA, 0x00,
        B_ST0 | B_PSEUDO,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FICOMP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000018DE, 0x00,
        B_ST0 | B_PSEUDO,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FICOMP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000018DA, 0x00,
        B_ST0 | B_PSEUDO,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FILD"),
        D_FPU | D_MEMORY | D_FPUPUSH, 0,
        1, 0x000038FF, 0x000000DF, 0x00,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FILD"),
        D_FPU | D_MEMORY | D_FPUPUSH, 0,
        1, 0x000038FF, 0x000000DB, 0x00,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FILD"),
        D_FPU | D_MEMORY | D_FPUPUSH, 0,
        1, 0x000038FF, 0x000028DF, 0x00,
        B_INT64 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FIST"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000010DF, 0x00,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FIST"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000010DB, 0x00,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FISTP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000018DF, 0x00,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FISTP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000018DB, 0x00,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FISTP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000038DF, 0x00,
        B_INT64 | B_SIGNED | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FISTTP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000008DF, 0x00,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FISTTP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000008DB, 0x00,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FISTTP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000008DD, 0x00,
        B_INT64 | B_SIGNED | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FLD"),
        D_FPU | D_MEMORY | D_FPUPUSH, 0,
        1, 0x000038FF, 0x000000D9, 0x00,
        B_FLOAT32 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FLD"),
        D_FPU | D_MEMORY | D_FPUPUSH, 0,
        1, 0x000038FF, 0x000000DD, 0x00,
        B_FLOAT64 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FLD"),
        D_FPU | D_MEMORY | D_FPUPUSH, 0,
        1, 0x000038FF, 0x000028DB, 0x00,
        B_FLOAT80 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FLDCW"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000028D9, 0x00,
        B_FCW | B_CHG | B_PSEUDO,
        B_INT16 | B_BINARY | B_MEMORY,
        B_NONE,
        B_NONE
    },

    {
        T("FLDENV"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000020D9, 0x00,
        B_LONGDATA | B_MEMORY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FMUL"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000008D8, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT32 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FMUL"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000008DC, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT64 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FIMUL"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000008DA, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FIMUL"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000008DE, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FRSTOR"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000020DD, 0x00,
        B_LONGDATA | B_MEMORY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FSAVE"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000030DD, 0x00,
        B_LONGDATA | B_MEMORY | B_CHG,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FST"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000010D9, 0x00,
        B_FLOAT32 | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FST"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000010DD, 0x00,
        B_FLOAT64 | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FSTP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000018D9, 0x00,
        B_FLOAT32 | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FSTP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000018DD, 0x00,
        B_FLOAT64 | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FSTP"),
        D_FPU | D_MEMORY | D_FPUPOP, 0,
        1, 0x000038FF, 0x000038DB, 0x00,
        B_FLOAT80 | B_MEMORY | B_SHOWSIZE | B_CHG,
        B_ST0 | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FSTCW"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000038D9, 0x00,
        B_INT16 | B_BINARY | B_MEMORY | B_CHG,
        B_FCW | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FSTENV"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000030D9, 0x00,
        B_LONGDATA | B_MEMORY | B_CHG,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FSTSW"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000038DD, 0x00,
        B_INT16 | B_BINARY | B_MEMORY | B_CHG,
        B_FST | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("FSUB"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000020D8, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT32 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FSUB"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000020DC, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT64 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FISUB"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000020DA, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FISUB"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000020DE, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FSUBR"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000028D8, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT32 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FSUBR"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000028DC, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_FLOAT64 | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FISUBR"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000028DA, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT32 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FISUBR"),
        D_FPU | D_MEMORY, 0,
        1, 0x000038FF, 0x000028DE, 0x00,
        B_ST0 | B_UPD | B_PSEUDO,
        B_INT16 | B_SIGNED | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("FSETPM"),
        D_FPU | D_UNDOC, 0,
        2, 0x0000FFFF, 0x0000E4DB, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("ADDPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000580F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VADDPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000058, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("ADDPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000580F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VADDPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000058, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("ADDSD"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000580F, 0x00,
        B_SREGF64L | B_UPD,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VADDSD"),
        D_AVX | D_MUSTF2, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000058, 0x00,
        B_SREGF64L | B_CHG,
        B_SVEXF64L,
        B_SSEF64L,
        B_NONE
    },

    {
        T("ADDSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x0000580F, 0x00,
        B_SREGF32L | B_UPD,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VADDSS"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000058, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L,
        B_SSEF32L,
        B_NONE
    },

    {
        T("ADDSUBPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000D00F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VADDSUBPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x000000D0, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("ADDSUBPS"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000D00F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VADDSUBPS"),
        D_AVX | D_MUSTF2, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x000000D0, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("ANDPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000540F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VANDPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000054, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("ANDPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000540F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VANDPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000054, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("ANDNPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000550F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VANDNPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000055, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("ANDNPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000550F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VANDNPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000055, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("CMP*PD"),
        D_SSE | D_POSTBYTE | D_MUST66 | D_WILDCARD, 0,
        2, 0x0000FFFF, 0x0000C20F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VCMP*PD"),
        D_AVX | D_POSTBYTE | D_MUST66 | D_WILDCARD, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x000000C2, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("CMPPD"),
        D_SSE | D_MUST66 | D_SUSPICIOUS, 0,
        2, 0x0000FFFF, 0x0000C20F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_CONST8,
        B_NONE
    },

    {
        T("VCMPPD"),
        D_AVX | D_MUST66 | D_SUSPICIOUS, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x000000C2, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_CONST8
    },

    {
        T("CMP*PS"),
        D_SSE | D_POSTBYTE | D_MUSTNONE | D_WILDCARD, 0,
        2, 0x0000FFFF, 0x0000C20F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VCMP*PS"),
        D_AVX | D_POSTBYTE | D_MUSTNONE | D_WILDCARD, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x000000C2, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("CMPPS"),
        D_SSE | D_MUSTNONE | D_SUSPICIOUS, 0,
        2, 0x0000FFFF, 0x0000C20F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_CONST8,
        B_NONE
    },

    {
        T("VCMPPS"),
        D_AVX | D_MUSTNONE | D_SUSPICIOUS, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x000000C2, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_CONST8
    },

    {
        T("CMP*SD"),
        D_SSE | D_POSTBYTE | D_MUSTF2 | D_WILDCARD, 0,
        2, 0x0000FFFF, 0x0000C20F, 0x00,
        B_SREGF64L | B_UPD,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VCMP*SD"),
        D_AVX | D_POSTBYTE | D_MUSTF2 | D_WILDCARD, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x000000C2, 0x00,
        B_SREGF64L | B_CHG,
        B_SVEXF64L,
        B_SSEF64L,
        B_NONE
    },

    {
        T("CMPSD"),
        D_SSE | D_MUSTF2 | D_SUSPICIOUS, 0,
        2, 0x0000FFFF, 0x0000C20F, 0x00,
        B_SREGF64L | B_UPD,
        B_SVEXF64L,
        B_CONST8,
        B_NONE
    },

    {
        T("VCMPSD"),
        D_AVX | D_MUSTF2 | D_SUSPICIOUS, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x000000C2, 0x00,
        B_SREGF64L | B_CHG,
        B_SVEXF64L,
        B_SSEF64L,
        B_CONST8
    },

    {
        T("CMP*SS"),
        D_SSE | D_POSTBYTE | D_MUSTF3 | D_WILDCARD, 0,
        2, 0x0000FFFF, 0x0000C20F, 0x00,
        B_SREGF32L | B_UPD,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VCMP*SS"),
        D_AVX | D_POSTBYTE | D_MUSTF3 | D_WILDCARD, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x000000C2, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L,
        B_SSEF32L,
        B_NONE
    },

    {
        T("CMPSS"),
        D_SSE | D_MUSTF3 | D_SUSPICIOUS, 0,
        2, 0x0000FFFF, 0x0000C20F, 0x00,
        B_SREGF32L | B_UPD,
        B_SSEF32L,
        B_CONST8,
        B_NONE
    },

    {
        T("VCMPSS"),
        D_AVX | D_MUSTF3 | D_SUSPICIOUS, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x000000C2, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L,
        B_SSEF32L,
        B_CONST8
    },

    {
        T("COMISD"),
        D_SSE | D_MUST66 | D_ALLFLAGS, 0,
        2, 0x0000FFFF, 0x00002F0F, 0x00,
        B_SREGF64L,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VCOMISD"),
        D_AVX | D_MUST66 | D_ALLFLAGS, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x0000002F, 0x00,
        B_SREGF64L,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("COMISS"),
        D_SSE | D_MUSTNONE | D_ALLFLAGS, 0,
        2, 0x0000FFFF, 0x00002F0F, 0x00,
        B_SREGF32L,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VCOMISS"),
        D_AVX | D_MUSTNONE | D_ALLFLAGS, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x0000002F, 0x00,
        B_SREGF32L,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("CVTDQ2PD"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x0000E60F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEI32x2L,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTDQ2PD"),
        D_AVX | D_MUSTF3 | D_REGISTER, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x000000E6, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEI32x2L | B_REGISTER | B_NOVEXSIZE | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTDQ2PD"),
        D_AVX | D_MUSTF3 | D_MEMORY, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x000000E6, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEI32x2L | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("CVTDQ2PS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00005B0F, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTDQ2PS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000005B, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("CVTPD2DQ"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000E60F, 0x00,
        B_SREGI32x2L | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTPD2DQ"),
        D_AVX | D_MUSTF2, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x000000E6, 0x00,
        B_SREGI32x2L | B_NOVEXSIZE | B_CHG,
        B_SSEF64x2 | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("CVTPD2PI"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00002D0F, 0x00,
        B_MREG32x2 | B_SIGNED | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("CVTPD2PS"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00005A0F, 0x00,
        B_SREGF32x2L | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTPD2PS"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000005A, 0x00,
        B_SREGF32x2L | B_NOVEXSIZE | B_CHG,
        B_SSEF64x2 | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("CVTPI2PD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00002A0F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_MMX32x2 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("CVTPI2PS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00002A0F, 0x00,
        B_SREGF32x2L | B_CHG,
        B_MMX32x2 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("CVTPS2DQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00005B0F, 0x00,
        B_SREGI32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTPS2DQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000005B, 0x00,
        B_SREGI32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("CVTPS2PD"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00005A0F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF32x2L,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTPS2PD"),
        D_AVX | D_MUSTNONE | D_REGISTER, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000005A, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF32x2L | B_REGISTER | B_NOVEXSIZE | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTPS2PD"),
        D_AVX | D_MUSTNONE | D_MEMORY, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000005A, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF32x2L | B_MEMORY | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("CVTPS2PI"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00002D0F, 0x00,
        B_MREG32x2 | B_CHG,
        B_SSEF32x2L,
        B_NONE,
        B_NONE
    },

    {
        T("CVTSD2SI"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x00002D0F, 0x00,
        B_REG32 | B_CHG,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTSD2SI"),
        D_AVX | D_MUSTF2, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x0000002D, 0x00,
        B_REG32 | B_CHG,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("CVTSD2SS"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x00005A0F, 0x00,
        B_SREGF32L | B_CHG,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTSD2SS"),
        D_AVX | D_MUSTF2, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000005A, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L,
        B_SSEF64L,
        B_NONE
    },

    {
        T("CVTSI2SD"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x00002A0F, 0x00,
        B_SREGF64L | B_CHG,
        B_INT32 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTSI2SD"),
        D_AVX | D_MUSTF2, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000002A, 0x00,
        B_SREGF64L | B_CHG,
        B_SVEXF64L,
        B_INT32 | B_SIGNED,
        B_NONE
    },

    {
        T("CVTSI2SS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00002A0F, 0x00,
        B_SREGF32L | B_CHG,
        B_INT32 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTSI2SS"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000002A, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L,
        B_INT32 | B_SIGNED,
        B_NONE
    },

    {
        T("CVTSS2SD"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00005A0F, 0x00,
        B_SREGF64L | B_CHG,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTSS2SD"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000005A, 0x00,
        B_SREGF64L | B_CHG,
        B_SVEXF64L,
        B_SSEF32L,
        B_NONE
    },

    {
        T("CVTSS2SI"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00002D0F, 0x00,
        B_REG32 | B_CHG,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTSS2SI"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x0000002D, 0x00,
        B_REG32 | B_CHG,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("CVTTPD2PI"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00002C0F, 0x00,
        B_MREG32x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("CVTTPD2DQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000E60F, 0x00,
        B_SREGI32x2L | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTTPD2DQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x000000E6, 0x00,
        B_SREGI32x2L | B_NOVEXSIZE | B_CHG,
        B_SSEF64x2 | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("CVTTPS2DQ"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00005B0F, 0x00,
        B_SREGI32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTTPS2DQ"),
        D_AVX | D_MUSTF3, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000005B, 0x00,
        B_SREGI32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("CVTTPS2PI"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00002C0F, 0x00,
        B_MREG32x2 | B_CHG,
        B_SSEF32x2L,
        B_NONE,
        B_NONE
    },

    {
        T("CVTTSD2SI"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x00002C0F, 0x00,
        B_REG32 | B_CHG,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTTSD2SI"),
        D_AVX | D_MUSTF2, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x0000002C, 0x00,
        B_REG32 | B_CHG,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("CVTTSS2SI"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00002C0F, 0x00,
        B_REG32 | B_CHG,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTTSS2SI"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x0000002C, 0x00,
        B_REG32 | B_CHG,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("DIVPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00005E0F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VDIVPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000005E, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("DIVPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00005E0F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VDIVPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000005E, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("DIVSD"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x00005E0F, 0x00,
        B_SREGF64L | B_UPD,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VDIVSD"),
        D_AVX | D_MUSTF2, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000005E, 0x00,
        B_SREGF64L | B_CHG,
        B_SVEXF64L,
        B_SSEF64L,
        B_NONE
    },

    {
        T("DIVSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00005E0F, 0x00,
        B_SREGF32L | B_UPD,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VDIVSS"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000005E, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L,
        B_SSEF32L,
        B_NONE
    },

    {
        T("HADDPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00007C0F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VHADDPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000007C, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("HADDPS"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x00007C0F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VHADDPS"),
        D_AVX | D_MUSTF2, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000007C, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("HSUBPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00007D0F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VHSUBPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000007D, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("HSUBPS"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x00007D0F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VHSUBPS"),
        D_AVX | D_MUSTF2, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000007D, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("LDDQU"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000F00F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VLDDQU"),
        D_AVX | D_MUSTF2, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x000000F0, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("LDMXCSR"),
        D_CMD | D_MEMORY, 0,
        2, 0x0038FFFF, 0x0010AE0F, 0x00,
        B_MXCSR | B_CHG | B_PSEUDO,
        B_INT32 | B_BINARY | B_MEMORY,
        B_NONE,
        B_NONE
    },

    {
        T("VLDMXCSR"),
        D_CMD | D_MEMORY, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000038FF, 0x000010AE, 0x00,
        B_MXCSR | B_CHG | B_PSEUDO,
        B_INT32 | B_BINARY | B_MEMORY,
        B_NONE,
        B_NONE
    },

    {
        T("VSTMXCSR"),
        D_CMD | D_MEMORY, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000038FF, 0x000018AE, 0x00,
        B_INT32 | B_BINARY | B_MEMONLY | B_NOESP | B_SHOWSIZE | B_CHG,
        B_MXCSR | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("MASKMOVDQU"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0F70F, 0x00,
        B_SSEI8x16DI | B_UPD | B_PSEUDO,
        B_SREGI8x16 | B_BINARY,
        B_SSEI8x16 | B_REGISTER,
        B_NONE
    },

    {
        T("VMASKMOVDQU"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x0000C0FF, 0x0000C0F7, 0x00,
        B_SSEI8x16DI | B_UPD | B_PSEUDO,
        B_SREGI8x16 | B_BINARY,
        B_SSEI8x16 | B_REGISTER,
        B_NONE
    },

    {
        T("MASKMOVQ"),
        D_MMX | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0F70F, 0x00,
        B_MMX8x8DI | B_UPD | B_PSEUDO,
        B_MREG8x8,
        B_MMX8x8 | B_REGISTER,
        B_NONE
    },

    {
        T("MAXPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00005F0F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMAXPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000005F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("MAXPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00005F0F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VMAXPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000005F, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("MAXSD"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x00005F0F, 0x00,
        B_SREGF64L | B_UPD,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VMAXSD"),
        D_AVX | D_MUSTF2, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000005F, 0x00,
        B_SREGF64L | B_CHG,
        B_SVEXF64L,
        B_SSEF64L,
        B_NONE
    },

    {
        T("MAXSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00005F0F, 0x00,
        B_SREGF32L | B_UPD,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VMAXSS"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000005F, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L,
        B_SSEF32L,
        B_NONE
    },

    {
        T("MFENCE"),
        D_SSE, 0,
        3, 0x00FFFFFF, 0x00F0AE0F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("MINPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00005D0F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMINPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000005D, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("MINPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00005D0F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VMINPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000005D, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("MINSD"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x00005D0F, 0x00,
        B_SREGF64L | B_UPD,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VMINSD"),
        D_AVX | D_MUSTF2, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000005D, 0x00,
        B_SREGF64L | B_CHG,
        B_SVEXF64L,
        B_SSEF64L,
        B_NONE
    },

    {
        T("MINSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00005D0F, 0x00,
        B_SREGF32L | B_UPD,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VMINSS"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000005D, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L,
        B_SSEF32L,
        B_NONE
    },

    {
        T("MOVAPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000280F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVAPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000028, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVAPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000290F, 0x00,
        B_SSEF64x2 | B_CHG,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVAPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000029, 0x00,
        B_SSEF64x2 | B_CHG,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVAPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000280F, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVAPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000028, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("MOVAPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000290F, 0x00,
        B_SSEF32x4 | B_CHG,
        B_SREGF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVAPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000029, 0x00,
        B_SSEF32x4 | B_CHG,
        B_SREGF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("MOVD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00006E0F, 0x00,
        B_MREG32x2 | B_CHG,
        B_INT32,
        B_NONE,
        B_NONE
    },

    {
        T("MOVD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00007E0F, 0x00,
        B_INT32 | B_CHG,
        B_MREG32x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00006E0F, 0x00,
        B_SREGI32x2L | B_CHG,
        B_INT32,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x0000006E, 0x00,
        B_SREGI32x2L | B_CHG,
        B_INT32,
        B_NONE,
        B_NONE
    },

    {
        T("MOVD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00007E0F, 0x00,
        B_INT32 | B_CHG,
        B_SREGI32L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x0000007E, 0x00,
        B_INT32 | B_CHG,
        B_SREGI32L,
        B_NONE,
        B_NONE
    },

    {
        T("MOVDDUP"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000120F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVDDUP"),
        D_AVX | D_MUSTF2, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000012, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVDQA"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00006F0F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVDQA"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000006F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVDQA"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00007F0F, 0x00,
        B_SSEF64x2 | B_CHG,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVDQA"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000007F, 0x00,
        B_SSEF64x2 | B_CHG,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVDQU"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00006F0F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVDQU"),
        D_AVX | D_MUSTF3, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000006F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVDQU"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00007F0F, 0x00,
        B_SSEF64x2 | B_CHG,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVDQU"),
        D_AVX | D_MUSTF3, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000007F, 0x00,
        B_SSEF64x2 | B_CHG,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVDQ2Q"),
        D_MMX | D_MUSTF2 | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0D60F, 0x00,
        B_MREG32x2 | B_CHG,
        B_SSEI32x2L | B_REGISTER,
        B_NONE,
        B_NONE
    },

    {
        T("MOVHLPS"),
        D_SSE | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0120F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4 | B_REGISTER,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVHLPS"),
        D_AVX | D_MUSTNONE | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000C0FF, 0x0000C012, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4 | B_REGISTER,
        B_NONE
    },

    {
        T("MOVHPD"),
        D_SSE | D_MUST66 | D_MEMORY, 0,
        2, 0x0000FFFF, 0x0000160F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64L | B_MEMORY,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVHPD"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000016, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64L | B_MEMORY,
        B_NONE
    },

    {
        T("MOVHPD"),
        D_SSE | D_MUST66 | D_MEMORY, 0,
        2, 0x0000FFFF, 0x0000170F, 0x00,
        B_SSEF64L | B_MEMORY | B_UPD,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVHPD"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x00000017, 0x00,
        B_SSEF64L | B_MEMORY | B_UPD,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVHPS"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0000FFFF, 0x0000160F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x2L | B_MEMORY,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVHPS"),
        D_AVX | D_MUSTNONE | D_MEMORY, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000016, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x2L | B_MEMORY,
        B_NONE
    },

    {
        T("MOVHPS"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0000FFFF, 0x0000170F, 0x00,
        B_SSEF32x2L | B_MEMORY | B_UPD,
        B_SREGF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVHPS"),
        D_AVX | D_MUSTNONE | D_MEMORY, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x00000017, 0x00,
        B_SSEF32x2L | B_MEMORY | B_UPD,
        B_SREGF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("MOVLHPS"),
        D_SSE | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0160F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x2L | B_REGISTER,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVLHPS"),
        D_AVX | D_MUSTNONE | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000C0FF, 0x0000C016, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x2L | B_REGISTER,
        B_NONE
    },

    {
        T("MOVLPD"),
        D_SSE | D_MUST66 | D_MEMORY, 0,
        2, 0x0000FFFF, 0x0000120F, 0x00,
        B_SREGF64L | B_UPD,
        B_SSEF64L | B_MEMORY,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVLPD"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000012, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SVEXF64x2,
        B_SSEF64L | B_MEMORY,
        B_NONE
    },

    {
        T("MOVLPD"),
        D_SSE | D_MUST66 | D_MEMORY, 0,
        2, 0x0000FFFF, 0x0000130F, 0x00,
        B_SSEF64L | B_MEMORY | B_UPD,
        B_SREGF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVLPD"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x00000013, 0x00,
        B_SSEF64L | B_MEMORY | B_UPD,
        B_SREGF64L,
        B_NONE,
        B_NONE
    },

    {
        T("MOVLPS"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0000FFFF, 0x0000120F, 0x00,
        B_SREGF32x2L | B_UPD,
        B_SSEF32x2L | B_MEMORY,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVLPS"),
        D_AVX | D_MUSTNONE | D_MEMORY, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000012, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x2L | B_MEMORY,
        B_NONE
    },

    {
        T("MOVLPS"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0000FFFF, 0x0000130F, 0x00,
        B_SSEF32x2L | B_MEMORY | B_UPD,
        B_SREGF32x2L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVLPS"),
        D_AVX | D_MUSTNONE | D_MEMORY, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x00000013, 0x00,
        B_SSEF32x2L | B_MEMORY | B_UPD,
        B_SREGF32x2L,
        B_NONE,
        B_NONE
    },

    {
        T("MOVMSKPD"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0500F, 0x00,
        B_REG32 | B_CHG,
        B_SSEF64x2 | B_REGONLY,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVMSKPD"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x0000C0FF, 0x0000C050, 0x00,
        B_REG32 | B_CHG,
        B_SSEF64x2 | B_REGONLY,
        B_NONE,
        B_NONE
    },

    {
        T("MOVMSKPS"),
        D_SSE | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0500F, 0x00,
        B_REG32 | B_CHG,
        B_SSEF32x4 | B_REGONLY,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVMSKPS"),
        D_AVX | D_MUSTNONE | D_REGISTER, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x0000C0FF, 0x0000C050, 0x00,
        B_REG32 | B_CHG,
        B_SSEF32x4 | B_REGONLY,
        B_NONE,
        B_NONE
    },

    {
        T("MOVNTDQ"),
        D_SSE | D_MUST66 | D_MEMORY, 0,
        2, 0x0000FFFF, 0x0000E70F, 0x00,
        B_SSEI8x16 | B_MEMORY | B_CHG,
        B_SREGI8x16 | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVNTDQ"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x000000E7, 0x00,
        B_SSEI8x16 | B_MEMORY | B_CHG,
        B_SREGI8x16 | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("MOVNTI"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0000FFFF, 0x0000C30F, 0x00,
        B_INT32 | B_MEMORY | B_CHG,
        B_REG32,
        B_NONE,
        B_NONE
    },

    {
        T("MOVNTPD"),
        D_SSE | D_MUST66 | D_MEMORY, 0,
        2, 0x0000FFFF, 0x00002B0F, 0x00,
        B_SSEF64x2 | B_MEMORY | B_CHG,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVNTPD"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000002B, 0x00,
        B_SSEF64x2 | B_MEMORY | B_CHG,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVNTPS"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0000FFFF, 0x00002B0F, 0x00,
        B_SSEF32x4 | B_MEMORY | B_CHG,
        B_SREGF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVNTPS"),
        D_AVX | D_MUSTNONE | D_MEMORY, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x0000002B, 0x00,
        B_SSEF32x4 | B_MEMORY | B_CHG,
        B_SREGF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("MOVNTQ"),
        D_MMX | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0000FFFF, 0x0000E70F, 0x00,
        B_MMX64 | B_MEMORY | B_CHG,
        B_MREG64,
        B_NONE,
        B_NONE
    },

    {
        T("MOVQ"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00006F0F, 0x00,
        B_MREG64 | B_CHG,
        B_MMX64,
        B_NONE,
        B_NONE
    },

    {
        T("MOVQ"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00007F0F, 0x00,
        B_MMX64 | B_CHG,
        B_MREG64,
        B_NONE,
        B_NONE
    },

    {
        T("MOVQ"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00007E0F, 0x00,
        B_SREGF64L | B_CHG,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVQ"),
        D_AVX | D_MUSTF3, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x0000007E, 0x00,
        B_SREGF64L | B_CHG,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("MOVQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000D60F, 0x00,
        B_SSEF64L | B_CHG,
        B_SREGF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x000000D6, 0x00,
        B_SSEF64L | B_CHG,
        B_SREGF64L,
        B_NONE,
        B_NONE
    },

    {
        T("MOVQ2DQ"),
        D_MMX | D_MUSTF3 | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0D60F, 0x00,
        B_SREGF64L | B_UPD,
        B_MMX8x8 | B_REGISTER,
        B_NONE,
        B_NONE
    },

    {
        T("MOVSD"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000100F, 0x00,
        B_SREGF64L | B_UPD,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVSD"),
        D_AVX | D_MUSTF2 | D_MEMORY, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x00000010, 0x00,
        B_SREGF64L | B_UPD,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVSD"),
        D_AVX | D_MUSTF2 | D_REGISTER, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000010, 0x00,
        B_SREGF64L | B_UPD,
        B_SVEXF64x2,
        B_SSEF64L,
        B_NONE
    },

    {
        T("MOVSD"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000110F, 0x00,
        B_SSEF64L | B_UPD,
        B_SREGF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVSD"),
        D_AVX | D_MUSTF2 | D_MEMORY, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x00000011, 0x00,
        B_SSEF64L | B_UPD,
        B_SREGF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVSD"),
        D_AVX | D_MUSTF2 | D_REGISTER, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000011, 0x00,
        B_SSEF64L | B_UPD,
        B_SVEXF64x2,
        B_SREGF64L,
        B_NONE
    },

    {
        T("MOVSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x0000100F, 0x00,
        B_SREGF32L | B_UPD,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVSS"),
        D_AVX | D_MUSTF3 | D_MEMORY, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x00000010, 0x00,
        B_SREGF32L | B_UPD,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVSS"),
        D_AVX | D_MUSTF3 | D_REGISTER, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000010, 0x00,
        B_SREGF32L | B_UPD,
        B_SVEXF32x4,
        B_SSEF32L,
        B_NONE
    },

    {
        T("MOVSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x0000110F, 0x00,
        B_SSEF32L | B_UPD,
        B_SREGF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVSS"),
        D_AVX | D_MUSTF3 | D_MEMORY, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x00000011, 0x00,
        B_SSEF32L | B_UPD,
        B_SREGF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVSS"),
        D_AVX | D_MUSTF3 | D_REGISTER, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000011, 0x00,
        B_SSEF32L | B_UPD,
        B_SVEXF32x4,
        B_SREGF32L,
        B_NONE
    },

    {
        T("MOVSHDUP"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x0000160F, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVSHDUP"),
        D_AVX | D_MUSTF3, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000016, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("MOVSLDUP"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x0000120F, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVSLDUP"),
        D_AVX | D_MUSTF3, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000012, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("MOVUPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000100F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVUPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000010, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVUPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000110F, 0x00,
        B_SSEF64x2 | B_CHG,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVUPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000011, 0x00,
        B_SSEF64x2 | B_CHG,
        B_SREGF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("MOVUPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000100F, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVUPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000010, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("MOVUPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000110F, 0x00,
        B_SSEF32x4 | B_CHG,
        B_SREGF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVUPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000011, 0x00,
        B_SSEF32x4 | B_CHG,
        B_SREGF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("MULPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000590F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VMULPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000059, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("MULPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000590F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VMULPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000059, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("MULSD"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000590F, 0x00,
        B_SREGF64L | B_UPD,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VMULSD"),
        D_AVX | D_MUSTF2, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000059, 0x00,
        B_SREGF64L | B_UPD,
        B_SVEXF64L,
        B_SSEF64L,
        B_NONE
    },

    {
        T("MULSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x0000590F, 0x00,
        B_SREGF32L | B_UPD,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VMULSS"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000059, 0x00,
        B_SREGF32L | B_UPD,
        B_SVEXF32L,
        B_SSEF32L,
        B_NONE
    },

    {
        T("ORPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000560F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VORPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000056, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("ORPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000560F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VORPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000056, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("PACKSSWB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000630F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PACKSSWB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000630F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPACKSSWB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000063, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PACKSSDW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00006B0F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PACKSSDW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00006B0F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPACKSSDW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x0000006B, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PACKUSWB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000670F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PACKUSWB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000670F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPACKUSWB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000067, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PADDB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000FC0F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PADDW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000FD0F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PADDD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000FE0F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PADDB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000FC0F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPADDB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000FC, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PADDW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000FD0F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPADDW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000FD, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PADDD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000FE0F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPADDD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000FE, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PADDQ"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000D40F, 0x00,
        B_MREG64 | B_UPD,
        B_MMX64,
        B_NONE,
        B_NONE
    },

    {
        T("PADDQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000D40F, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SSEI64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VPADDQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000D4, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SVEXI64x2,
        B_SSEI64x2,
        B_NONE
    },

    {
        T("PADDSB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000EC0F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PADDSW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000ED0F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PADDSB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000EC0F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPADDSB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000EC, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PADDSW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000ED0F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPADDSW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000ED, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PADDUSB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000DC0F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PADDUSW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000DD0F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PADDUSB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000DC0F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPADDUSB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000DC, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PADDUSW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000DD0F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPADDUSW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000DD, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PAND"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000DB0F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PAND"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000DB0F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPAND"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000DB, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PANDN"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000DF0F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PANDN"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000DF0F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPANDN"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000DF, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PAVGB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000E00F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PAVGW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000E30F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PAVGB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000E00F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPAVGB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000E0, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PAVGW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000E30F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPAVGW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000E3, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PCMPEQB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000740F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PCMPEQW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000750F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PCMPEQD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000760F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PCMPEQB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000740F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPCMPEQB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000074, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PCMPEQW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000750F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPCMPEQW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000075, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PCMPEQD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000760F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPCMPEQD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000076, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PCMPGTB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000640F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PCMPGTW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000650F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PCMPGTD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000660F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PCMPGTB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000640F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPCMPGTB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000064, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PCMPGTW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000650F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPCMPGTW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000065, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PCMPGTD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000660F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPCMPGTD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000066, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PEXTRW"),
        D_MMX | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0C50F, 0x00,
        B_REG32 | B_CHG,
        B_MMX16x4 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PEXTRW"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0C50F, 0x00,
        B_REG32 | B_CHG,
        B_SSEI16x8 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("VPEXTRW"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x0000C0FF, 0x0000C0C5, 0x00,
        B_REG32 | B_CHG,
        B_SSEI16x8 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PINSRW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000C40F, 0x00,
        B_MREG16x4 | B_UPD,
        B_INT16,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PINSRW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x00C0FFFF, 0x00C0C40F, 0x00,
        B_MREG16x4 | B_UPD,
        B_INT32 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PINSRW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000C40F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_INT16,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("VPINSRW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000C4, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_INT16,
        B_CONST8 | B_COUNT
    },

    {
        T("PINSRW"),
        D_SSE | D_MUST66, 0,
        2, 0x00C0FFFF, 0x00C0C40F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_INT32 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("VPINSRW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x0000C0FF, 0x0000C0C4, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_INT32 | B_REGISTER,
        B_CONST8 | B_COUNT
    },

    {
        T("PMADDWD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000F50F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PMADDWD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000F50F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPMADDWD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000F5, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PMAXSW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000EE0F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PMAXSW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000EE0F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPMAXSW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000EE, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PMAXUB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000DE0F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PMAXUB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000DE0F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPMAXUB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000DE, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PMINSW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000EA0F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PMINSW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000EA0F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPMINSW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000EA, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PMINUB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000DA0F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PMINUB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000DA0F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPMINUB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000DA, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PMOVMSKB"),
        D_MMX | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0D70F, 0x00,
        B_REG32 | B_CHG,
        B_MMX8x8 | B_REGISTER,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVMSKB"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00C0FFFF, 0x00C0D70F, 0x00,
        B_REG32 | B_CHG,
        B_SSEI8x16 | B_REGISTER,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVMSKB"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x0000C0FF, 0x0000C0D7, 0x00,
        B_REG32 | B_CHG,
        B_SSEI8x16 | B_REGISTER,
        B_NONE,
        B_NONE
    },

    {
        T("PMULHUW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000E40F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PMULHUW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000E40F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPMULHUW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000E4, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PMULHW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000E50F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PMULHW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000E50F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPMULHW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000E5, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PMULLW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000D50F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PMULLW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000D50F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPMULLW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000D5, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PMULUDQ"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000F40F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PMULUDQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000F40F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPMULUDQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000F4, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("POR"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000EB0F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("POR"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000EB0F, 0x00,
        B_SREGI8x16 | B_BINARY | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPOR"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000EB, 0x00,
        B_SREGI8x16 | B_BINARY | B_UPD,
        B_SVEXI8x16 | B_BINARY,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PSADBW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000F60F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PSADBW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000F60F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPSADBW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000F6, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PSHUFD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000700F, 0x00,
        B_SREGI32x4 | B_CHG,
        B_SSEI32x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPSHUFD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x00000070, 0x00,
        B_SREGI32x4 | B_CHG,
        B_SSEI32x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PSHUFHW"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x0000700F, 0x00,
        B_SREGI16x8 | B_CHG,
        B_SSEI16x8,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPSHUFHW"),
        D_AVX | D_MUSTF3, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x00000070, 0x00,
        B_SREGI16x8 | B_CHG,
        B_SSEI16x8,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PSHUFLW"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000700F, 0x00,
        B_SREGI16x8 | B_CHG,
        B_SSEI16x8,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPSHUFLW"),
        D_AVX | D_MUSTF2, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x00000070, 0x00,
        B_SREGI16x8 | B_CHG,
        B_SSEI16x8,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PSHUFW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000700F, 0x00,
        B_MREG16x4 | B_CHG,
        B_MMX16x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PSLLDQ"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00F8730F, 0x00,
        B_SSEI8x16 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("VPSLLDQ"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000F8FF, 0x0000F873, 0x00,
        B_SVEXI8x16 | B_UPD,
        B_SSEI8x16 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PSLLW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000F10F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PSLLW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000F10F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPSLLW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000F1, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PSLLW"),
        D_MMX | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00F0710F, 0x00,
        B_MMX16x4 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("PSLLW"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00F0710F, 0x00,
        B_SSEI16x8 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("VPSLLW"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000F8FF, 0x0000F071, 0x00,
        B_SVEXI16x8 | B_UPD,
        B_SSEI16x8 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PSLLD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000F20F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PSLLD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000F20F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPSLLD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000F2, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PSLLD"),
        D_MMX | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00F0720F, 0x00,
        B_MMX32x2 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("PSLLD"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00F0720F, 0x00,
        B_SSEI32x4 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("VPSLLD"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000F8FF, 0x0000F072, 0x00,
        B_SVEXI32x4 | B_UPD,
        B_SSEI32x4 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PSLLQ"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000F30F, 0x00,
        B_MREG64 | B_UPD,
        B_MMX64,
        B_NONE,
        B_NONE
    },

    {
        T("PSLLQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000F30F, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SSEI64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VPSLLQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000F3, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SVEXI64x2,
        B_SSEI64x2,
        B_NONE
    },

    {
        T("PSLLQ"),
        D_MMX | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00F0730F, 0x00,
        B_MMX64 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("PSLLQ"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00F0730F, 0x00,
        B_SSEI64x2 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("VPSLLQ"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000F8FF, 0x0000F073, 0x00,
        B_SVEXI64x2 | B_UPD,
        B_SSEI64x2 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PSRAW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000E10F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PSRAW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000E10F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPSRAW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000E1, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PSRAW"),
        D_MMX | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00E0710F, 0x00,
        B_MMX16x4 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("PSRAW"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00E0710F, 0x00,
        B_SSEI16x8 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("VPSRAW"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000F8FF, 0x0000E071, 0x00,
        B_SVEXI16x8 | B_UPD,
        B_SSEI16x8 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PSRAD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000E20F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PSRAD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000E20F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPSRAD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000E2, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PSRAD"),
        D_MMX | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00E0720F, 0x00,
        B_MMX32x2 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("PSRAD"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00E0720F, 0x00,
        B_SSEI32x4 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("VPSRAD"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000F8FF, 0x0000E072, 0x00,
        B_SVEXI32x4 | B_UPD,
        B_SSEI32x4 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PSRLDQ"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00D8730F, 0x00,
        B_SSEI8x16 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("VPSRLDQ"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000F8FF, 0x0000D873, 0x00,
        B_SVEXI8x16 | B_UPD,
        B_SSEI8x16 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PSRLW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000D10F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PSRLW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000D10F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPSRLW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000D1, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PSRLW"),
        D_MMX | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00D0710F, 0x00,
        B_MMX16x4 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("PSRLW"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00D0710F, 0x00,
        B_SSEI16x8 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("VPSRLW"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000F8FF, 0x0000D071, 0x00,
        B_SVEXI16x8 | B_UPD,
        B_SSEI16x8 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PSRLD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000D20F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PSRLD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000D20F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPSRLD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000D2, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PSRLD"),
        D_MMX | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00D0720F, 0x00,
        B_MMX32x2 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("PSRLD"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00D0720F, 0x00,
        B_SSEI32x4 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("VPSRLD"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000F8FF, 0x0000D072, 0x00,
        B_SVEXI32x4 | B_UPD,
        B_SSEI32x4 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PSRLQ"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000D30F, 0x00,
        B_MREG64 | B_UPD,
        B_MMX64,
        B_NONE,
        B_NONE
    },

    {
        T("PSRLQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000D30F, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SSEI64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VPSRLQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000D3, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SVEXI64x2,
        B_SSEI64x2,
        B_NONE
    },

    {
        T("PSRLQ"),
        D_MMX | D_MUSTNONE | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00D0730F, 0x00,
        B_MMX64 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("PSRLQ"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        2, 0x00F8FFFF, 0x00D0730F, 0x00,
        B_SSEI64x2 | B_REGISTER | B_UPD,
        B_CONST8 | B_COUNT,
        B_NONE,
        B_NONE
    },

    {
        T("VPSRLQ"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT,
        1, 0x0000F8FF, 0x0000D073, 0x00,
        B_SVEXI64x2 | B_UPD,
        B_SSEI64x2 | B_REGISTER,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("PSUBB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000F80F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PSUBW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000F90F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PSUBD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000FA0F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PSUBB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000F80F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPSUBB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000F8, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PSUBW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000F90F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPSUBW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000F9, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PSUBD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000FA0F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPSUBD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000FA, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PSUBQ"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000FB0F, 0x00,
        B_MREG64 | B_UPD,
        B_MMX64,
        B_NONE,
        B_NONE
    },

    {
        T("PSUBQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000FB0F, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SSEI64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VPSUBQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000FB, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SVEXI64x2,
        B_SSEI64x2,
        B_NONE
    },

    {
        T("PSUBSB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000E80F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PSUBSW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000E90F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PSUBSB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000E80F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPSUBSB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000E8, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PSUBSW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000E90F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPSUBSW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000E9, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PSUBUSB"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000D80F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PSUBUSW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000D90F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PSUBUSB"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000D80F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPSUBUSB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000D8, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PSUBUSW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000D90F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPSUBUSW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000D9, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PUNPCKHBW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000680F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PUNPCKHBW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000680F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPUNPCKHBW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000068, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PUNPCKHWD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000690F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PUNPCKHWD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000690F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPUNPCKHWD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000069, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PUNPCKHDQ"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00006A0F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PUNPCKHDQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00006A0F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPUNPCKHDQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x0000006A, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PUNPCKHQDQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00006D0F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPUNPCKHQDQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x0000006D, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PUNPCKLBW"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000600F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PUNPCKLBW"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000600F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPUNPCKLBW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000060, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("PUNPCKLWD"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000610F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PUNPCKLWD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000610F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPUNPCKLWD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000061, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PUNPCKLDQ"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000620F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PUNPCKLDQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000620F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPUNPCKLDQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x00000062, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PUNPCKLQDQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00006C0F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPUNPCKLQDQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x0000006C, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PXOR"),
        D_MMX | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000EF0F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PXOR"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000EF0F, 0x00,
        B_SREGI8x16 | B_BINARY | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VPXOR"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT,
        1, 0x000000FF, 0x000000EF, 0x00,
        B_SREGI8x16 | B_BINARY | B_UPD,
        B_SVEXI8x16 | B_BINARY,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("RCPPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000530F, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VRCPPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000053, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("RCPSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x0000530F, 0x00,
        B_SREGF32L | B_CHG,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VRCPSS"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000053, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L | B_CHG,
        B_SSEF32L,
        B_NONE
    },

    {
        T("RSQRTPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000520F, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VRSQRTPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000052, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("RSQRTSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x0000520F, 0x00,
        B_SREGF32L | B_CHG,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VRSQRTSS"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000052, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L,
        B_SSEF32L,
        B_NONE
    },

    {
        T("SHUFPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000C60F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VSHUFPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x000000C6, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_CONST8 | B_BINARY
    },

    {
        T("SHUFPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000C60F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VSHUFPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x000000C6, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_CONST8 | B_BINARY
    },

    {
        T("SQRTPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000510F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VSQRTPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000051, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("SQRTPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000510F, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VSQRTPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH | DX_NOVREG,
        1, 0x000000FF, 0x00000051, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("SQRTSD"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000510F, 0x00,
        B_SREGF64L | B_CHG,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VSQRTSD"),
        D_AVX | D_MUSTF2, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000051, 0x00,
        B_SREGF64L | B_CHG,
        B_SVEXF64L,
        B_SSEF64L,
        B_NONE
    },

    {
        T("SQRTSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x0000510F, 0x00,
        B_SREGF32L | B_CHG,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VSQRTSS"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x00000051, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L,
        B_SSEF32L,
        B_NONE
    },

    {
        T("SUBPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x00005C0F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VSUBPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000005C, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("SUBPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00005C0F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VSUBPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x0000005C, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("SUBSD"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x00005C0F, 0x00,
        B_SREGF64L | B_UPD,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VSUBSD"),
        D_AVX | D_MUSTF2, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000005C, 0x00,
        B_SREGF64L | B_UPD,
        B_SVEXF64L,
        B_SSEF64L,
        B_NONE
    },

    {
        T("SUBSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00005C0F, 0x00,
        B_SREGF32L | B_UPD,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VSUBSS"),
        D_AVX | D_MUSTF3, DX_VEX | DX_IGNOREL,
        1, 0x000000FF, 0x0000005C, 0x00,
        B_SREGF32L | B_UPD,
        B_SVEXF32L,
        B_SSEF32L,
        B_NONE
    },

    {
        T("UNPCKHPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000150F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VUNPCKHPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000015, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("UNPCKHPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000150F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VUNPCKHPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000015, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("UNPCKLPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000140F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VUNPCKLPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000014, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("UNPCKLPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000140F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VUNPCKLPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000014, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("UCOMISD"),
        D_SSE | D_MUST66 | D_ALLFLAGS, 0,
        2, 0x0000FFFF, 0x00002E0F, 0x00,
        B_SREGF64L,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("VUCOMISD"),
        D_AVX | D_MUST66 | D_ALLFLAGS, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x0000002E, 0x00,
        B_SREGF64L,
        B_SSEF64L,
        B_NONE,
        B_NONE
    },

    {
        T("UCOMISS"),
        D_SSE | D_MUSTNONE | D_ALLFLAGS, 0,
        2, 0x0000FFFF, 0x00002E0F, 0x00,
        B_SREGF32L,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("VUCOMISS"),
        D_AVX | D_MUSTNONE | D_ALLFLAGS, DX_VEX | DX_IGNOREL | DX_NOVREG,
        1, 0x000000FF, 0x0000002E, 0x00,
        B_SREGF32L,
        B_SSEF32L,
        B_NONE,
        B_NONE
    },

    {
        T("XORPD"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000570F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VXORPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000057, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_NONE
    },

    {
        T("XORPS"),
        D_SSE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x0000570F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VXORPS"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LBOTH,
        1, 0x000000FF, 0x00000057, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_NONE
    },

    {
        T("FXRSTOR"),
        D_SSE | D_MEMORY, 0,
        2, 0x0038FFFF, 0x0008AE0F, 0x00,
        B_LONGDATA | B_MEMORY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FXSAVE"),
        D_SSE | D_MEMORY, 0,
        2, 0x0038FFFF, 0x0000AE0F, 0x00,
        B_LONGDATA | B_MEMORY | B_CHG,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("LFENCE"),
        D_SSE, 0,
        3, 0x00FFFFFF, 0x00E8AE0F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("PREFETCHT0"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0038FFFF, 0x0008180F, 0x00,
        B_ANYMEM | B_MEMORY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("PREFETCHT1"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0038FFFF, 0x0010180F, 0x00,
        B_ANYMEM | B_MEMORY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("PREFETCHT2"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0038FFFF, 0x0018180F, 0x00,
        B_ANYMEM | B_MEMORY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("PREFETCHNTA"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0038FFFF, 0x0000180F, 0x00,
        B_ANYMEM | B_MEMORY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("NOP"),
        D_SSE | D_MUSTNONE | D_MEMORY | D_UNDOC, DX_NOP,
        2, 0x0020FFFF, 0x0020180F, 0x00,
        B_ANYMEM | B_MEMORY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("PREFETCH"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0038FFFF, 0x00000D0F, 0x00,
        B_ANYMEM | B_MEMORY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("PREFETCHW"),
        D_SSE | D_MUSTNONE | D_MEMORY, 0,
        2, 0x0038FFFF, 0x00080D0F, 0x00,
        B_ANYMEM | B_MEMORY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SFENCE"),
        D_SSE, 0,
        3, 0x00FFFFFF, 0x00F8AE0F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("BLENDPD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x000D3A0F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VBLENDPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_LEAD3A,
        1, 0x000000FF, 0x0000000D, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_CONST8 | B_BINARY
    },

    {
        T("BLENDPS"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x000C3A0F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VBLENDPS"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_LEAD3A,
        1, 0x000000FF, 0x0000000C, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_CONST8 | B_BINARY
    },

    {
        T("BLENDVPD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0015380F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_XMM0I64x2,
        B_NONE
    },

    {
        T("BLENDVPD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0015380F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_XMM0I64x2 | B_PSEUDO,
        B_NONE
    },

    {
        T("VBLENDVPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x0000004B, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_SIMMI8x16
    },

    {
        T("BLENDVPS"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0014380F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_XMM0I32x4,
        B_NONE
    },

    {
        T("BLENDVPS"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0014380F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_XMM0I32x4 | B_PSEUDO,
        B_NONE
    },

    {
        T("VBLENDVPS"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x0000004A, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_SIMMI8x16
    },

    {
        T("CRC32"),
        D_CMD | D_NEEDF2, 0,
        3, 0x00FFFFFF, 0x00F0380F, 0x00,
        B_REG32 | B_NOADDR | B_UPD,
        B_INT8 | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("CRC32"),
        D_CMD | D_NEEDF2, 0,
        3, 0x00FFFFFF, 0x00F1380F, 0x00,
        B_REG32 | B_NOADDR | B_UPD,
        B_INT1632 | B_NOADDR | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("DPPD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00413A0F, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SSEF64x2,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VDPPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD3A,
        1, 0x000000FF, 0x00000041, 0x00,
        B_SREGF64x2 | B_UPD,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_CONST8 | B_BINARY
    },

    {
        T("DPPS"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00403A0F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VDPPS"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_LEAD3A,
        1, 0x000000FF, 0x00000040, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32x4,
        B_CONST8 | B_BINARY
    },

    {
        T("EXTRACTPS"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00173A0F, 0x00,
        B_INT32 | B_CHG,
        B_SREGF32x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VEXTRACTPS"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD3A,
        1, 0x000000FF, 0x00000017, 0x00,
        B_INT32 | B_CHG,
        B_SREGF32x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("INSERTPS"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00213A0F, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SSEF32L,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VINSERTPS"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD3A,
        1, 0x000000FF, 0x00000021, 0x00,
        B_SREGF32x4 | B_UPD,
        B_SVEXF32x4,
        B_SSEF32L,
        B_CONST8 | B_BINARY
    },

    {
        T("MOVNTDQA"),
        D_SSE | D_MUST66 | D_MEMORY, 0,
        3, 0x00FFFFFF, 0x002A380F, 0x00,
        B_SREGI8x16 | B_BINARY | B_CHG,
        B_SSEI8x16 | B_MEMORY,
        B_NONE,
        B_NONE
    },

    {
        T("VMOVNTDQA"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x0000002A, 0x00,
        B_SREGI8x16 | B_BINARY | B_CHG,
        B_SSEI8x16 | B_MEMORY,
        B_NONE,
        B_NONE
    },

    {
        T("MPSADBW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00423A0F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VMPSADBW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD3A,
        1, 0x000000FF, 0x00000042, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_CONST8 | B_BINARY
    },

    {
        T("PACKUSDW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x002B380F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPACKUSDW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x0000002B, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PBLENDVB"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0010380F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_XMM0I8x16,
        B_NONE
    },

    {
        T("PBLENDVB"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0010380F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_XMM0I8x16 | B_PSEUDO,
        B_NONE
    },

    {
        T("VPBLENDVB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x0000004C, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_SIMMI8x16
    },

    {
        T("PBLENDW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x000E3A0F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPBLENDW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD3A,
        1, 0x000000FF, 0x0000000E, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_CONST8 | B_BINARY
    },

    {
        T("PCLMULLQLQDQ"),
        D_SSE | D_POSTBYTE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00443A0F, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SSEI64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VPCLMULLQLQDQ"),
        D_AVX | D_POSTBYTE | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD3A,
        1, 0x000000FF, 0x00000044, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SVEXI64x2,
        B_SSEI64x2,
        B_NONE
    },

    {
        T("PCLMULHQLQDQ"),
        D_SSE | D_POSTBYTE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00443A0F, 0x01,
        B_SREGI64x2 | B_UPD,
        B_SSEI64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VPCLMULHQLQDQ"),
        D_AVX | D_POSTBYTE | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD3A,
        1, 0x000000FF, 0x00000044, 0x01,
        B_SREGI64x2 | B_UPD,
        B_SVEXI64x2,
        B_SSEI64x2,
        B_NONE
    },

    {
        T("PCLMULLQHDQ"),
        D_SSE | D_POSTBYTE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00443A0F, 0x10,
        B_SREGI64x2 | B_UPD,
        B_SSEI64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VPCLMULLQHDQ"),
        D_AVX | D_POSTBYTE | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD3A,
        1, 0x000000FF, 0x00000044, 0x10,
        B_SREGI64x2 | B_UPD,
        B_SVEXI64x2,
        B_SSEI64x2,
        B_NONE
    },

    {
        T("PCLMULHQHDQ"),
        D_SSE | D_POSTBYTE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00443A0F, 0x11,
        B_SREGI64x2 | B_UPD,
        B_SSEI64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VPCLMULHQHDQ"),
        D_AVX | D_POSTBYTE | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD3A,
        1, 0x000000FF, 0x00000044, 0x11,
        B_SREGI64x2 | B_UPD,
        B_SVEXI64x2,
        B_SSEI64x2,
        B_NONE
    },

    {
        T("PCLMULQDQ"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00443A0F, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SSEI64x2,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPCLMULQDQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD3A,
        1, 0x000000FF, 0x00000044, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SVEXI64x2,
        B_SSEI64x2,
        B_CONST8 | B_BINARY
    },

    {
        T("PCMPEQQ"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0029380F, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SSEI64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VPCMPEQQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000029, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SVEXI64x2,
        B_SSEI64x2,
        B_NONE
    },

    {
        T("PCMPESTRI"),
        D_SSE | D_MUST66 | D_ALLFLAGS, 0,
        3, 0x00FFFFFF, 0x00613A0F, 0x00,
        B_SREGI8x16,
        B_SSEI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPCMPESTRI"),
        D_AVX | D_MUST66 | D_ALLFLAGS, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD3A,
        1, 0x000000FF, 0x00000061, 0x00,
        B_SREGI8x16,
        B_SSEI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PCMPESTRM"),
        D_SSE | D_MUST66 | D_ALLFLAGS, 0,
        3, 0x00FFFFFF, 0x00603A0F, 0x00,
        B_SREGI8x16,
        B_SSEI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPCMPESTRM"),
        D_AVX | D_MUST66 | D_ALLFLAGS, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD3A,
        1, 0x000000FF, 0x00000060, 0x00,
        B_SREGI8x16,
        B_SSEI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PCMPISTRI"),
        D_SSE | D_MUST66 | D_ALLFLAGS, 0,
        3, 0x00FFFFFF, 0x00633A0F, 0x00,
        B_SREGI8x16,
        B_SSEI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPCMPISTRI"),
        D_AVX | D_MUST66 | D_ALLFLAGS, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD3A,
        1, 0x000000FF, 0x00000063, 0x00,
        B_SREGI8x16,
        B_SSEI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PCMPISTRM"),
        D_SSE | D_MUST66 | D_ALLFLAGS, 0,
        3, 0x00FFFFFF, 0x00623A0F, 0x00,
        B_SREGI8x16,
        B_SSEI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPCMPISTRM"),
        D_AVX | D_MUST66 | D_ALLFLAGS, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD3A,
        1, 0x000000FF, 0x00000062, 0x00,
        B_SREGI8x16,
        B_SSEI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PCMPGTQ"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0037380F, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SSEI64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VPCMPGTQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000037, 0x00,
        B_SREGI64x2 | B_UPD,
        B_SVEXI64x2,
        B_SSEI64x2,
        B_NONE
    },

    {
        T("PEXTRB"),
        D_SSE | D_MUST66 | D_MEMORY, 0,
        3, 0x00FFFFFF, 0x00143A0F, 0x00,
        B_INT8 | B_MEMORY | B_CHG,
        B_SREGI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PEXTRB"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        3, 0x00FFFFFF, 0x00143A0F, 0x00,
        B_INT32 | B_REGISTER | B_CHG,
        B_SREGI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPEXTRB"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LSHORT | DX_NOVREG | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x00000014, 0x00,
        B_INT8 | B_MEMORY | B_CHG,
        B_SREGI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPEXTRB"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT | DX_NOVREG | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x00000014, 0x00,
        B_INT32 | B_REGISTER | B_CHG,
        B_SREGI8x16,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PEXTRD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00163A0F, 0x00,
        B_INT32 | B_CHG,
        B_SREGI32x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPEXTRD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD3A,
        1, 0x000000FF, 0x00000016, 0x00,
        B_INT32 | B_CHG,
        B_SREGI32x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PEXTRW"),
        D_SSE | D_MUST66 | D_MEMORY, 0,
        3, 0x00FFFFFF, 0x00153A0F, 0x00,
        B_INT16 | B_MEMORY | B_CHG,
        B_SREGI16x8,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PEXTRW"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        3, 0x00FFFFFF, 0x00153A0F, 0x00,
        B_INT32 | B_REGISTER | B_CHG,
        B_SREGI16x8,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPEXTRW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD3A,
        1, 0x000000FF, 0x00000015, 0x00,
        B_INT16 | B_CHG,
        B_SREGI16x8,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("PHMINPOSUW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0041380F, 0x00,
        B_SREGI16x8 | B_CHG,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPHMINPOSUW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000041, 0x00,
        B_SREGI16x8 | B_CHG,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("PINSRB"),
        D_SSE | D_MUST66 | D_MEMORY, 0,
        3, 0x00FFFFFF, 0x00203A0F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_INT8 | B_MEMORY,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPINSRB"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LSHORT | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x00000020, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_INT8 | B_MEMORY,
        B_CONST8 | B_BINARY
    },

    {
        T("PINSRB"),
        D_SSE | D_MUST66 | D_REGISTER, 0,
        3, 0x00FFFFFF, 0x00203A0F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_INT32 | B_REGISTER,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPINSRB"),
        D_AVX | D_MUST66 | D_REGISTER, DX_VEX | DX_LSHORT | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x00000020, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_INT32 | B_REGISTER,
        B_CONST8 | B_BINARY
    },

    {
        T("PINSRD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00223A0F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_INT32,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VPINSRD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x00000022, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_INT32,
        B_CONST8 | B_BINARY
    },

    {
        T("PMAXSB"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x003C380F, 0x00,
        B_SREGI8x16 | B_SIGNED | B_UPD,
        B_SSEI8x16 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPMAXSB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x0000003C, 0x00,
        B_SREGI8x16 | B_SIGNED | B_UPD,
        B_SVEXI8x16 | B_SIGNED,
        B_SSEI8x16 | B_SIGNED,
        B_NONE
    },

    {
        T("PMAXSD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x003D380F, 0x00,
        B_SREGI32x4 | B_SIGNED | B_UPD,
        B_SSEI32x4 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPMAXSD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x0000003D, 0x00,
        B_SREGI32x4 | B_SIGNED | B_UPD,
        B_SVEXI32x4 | B_SIGNED,
        B_SSEI32x4 | B_SIGNED,
        B_NONE
    },

    {
        T("PMAXUD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x003F380F, 0x00,
        B_SREGI32x4 | B_UNSIGNED | B_UPD,
        B_SSEI32x4 | B_UNSIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPMAXUD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x0000003F, 0x00,
        B_SREGI32x4 | B_UNSIGNED | B_UPD,
        B_SVEXI32x4 | B_UNSIGNED,
        B_SSEI32x4 | B_UNSIGNED,
        B_NONE
    },

    {
        T("PMAXUW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x003E380F, 0x00,
        B_SREGI16x8 | B_UNSIGNED | B_UPD,
        B_SSEI16x8 | B_UNSIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPMAXUW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x0000003E, 0x00,
        B_SREGI16x8 | B_UNSIGNED | B_UPD,
        B_SVEXI16x8 | B_UNSIGNED,
        B_SSEI16x8 | B_UNSIGNED,
        B_NONE
    },

    {
        T("PMINSB"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0038380F, 0x00,
        B_SREGI8x16 | B_SIGNED | B_UPD,
        B_SSEI8x16 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPMINSB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000038, 0x00,
        B_SREGI8x16 | B_SIGNED | B_UPD,
        B_SVEXI8x16 | B_SIGNED,
        B_SSEI8x16 | B_SIGNED,
        B_NONE
    },

    {
        T("PMINSD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0039380F, 0x00,
        B_SREGI32x4 | B_SIGNED | B_UPD,
        B_SSEI32x4 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPMINSD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000039, 0x00,
        B_SREGI32x4 | B_SIGNED | B_UPD,
        B_SVEXI32x4 | B_SIGNED,
        B_SSEI32x4 | B_SIGNED,
        B_NONE
    },

    {
        T("PMINUD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x003B380F, 0x00,
        B_SREGI32x4 | B_UNSIGNED | B_UPD,
        B_SSEI32x4 | B_UNSIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPMINUD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x0000003B, 0x00,
        B_SREGI32x4 | B_UNSIGNED | B_UPD,
        B_SVEXI32x4 | B_UNSIGNED,
        B_SSEI32x4 | B_UNSIGNED,
        B_NONE
    },

    {
        T("PMINUW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x003A380F, 0x00,
        B_SREGI16x8 | B_UNSIGNED | B_UPD,
        B_SSEI16x8 | B_UNSIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPMINUW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x0000003A, 0x00,
        B_SREGI16x8 | B_UNSIGNED | B_UPD,
        B_SVEXI16x8 | B_UNSIGNED,
        B_SSEI16x8 | B_UNSIGNED,
        B_NONE
    },

    {
        T("PMOVSXBW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0020380F, 0x00,
        B_SREGI16x8 | B_SIGNED | B_CHG,
        B_SSEI8x8L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVSXBW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000020, 0x00,
        B_SREGI16x8 | B_SIGNED | B_CHG,
        B_SSEI8x8L,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVSXBD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0021380F, 0x00,
        B_SREGI32x4 | B_SIGNED | B_CHG,
        B_SSEI8x4L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVSXBD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000021, 0x00,
        B_SREGI32x4 | B_SIGNED | B_CHG,
        B_SSEI8x4L,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVSXBQ"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0022380F, 0x00,
        B_SREGI64x2 | B_SIGNED | B_CHG,
        B_SSEI8x2L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVSXBQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000022, 0x00,
        B_SREGI64x2 | B_SIGNED | B_CHG,
        B_SSEI8x2L,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVSXWD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0023380F, 0x00,
        B_SREGI32x4 | B_SIGNED | B_CHG,
        B_SSEI16x4L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVSXWD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000023, 0x00,
        B_SREGI32x4 | B_SIGNED | B_CHG,
        B_SSEI16x4L,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVSXWQ"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0024380F, 0x00,
        B_SREGI64x2 | B_SIGNED | B_CHG,
        B_SSEI16x2L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVSXWQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000024, 0x00,
        B_SREGI64x2 | B_SIGNED | B_CHG,
        B_SSEI16x2L,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVSXDQ"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0025380F, 0x00,
        B_SREGI64x2 | B_SIGNED | B_CHG,
        B_SSEI32x2L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVSXDQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000025, 0x00,
        B_SREGI64x2 | B_SIGNED | B_CHG,
        B_SSEI32x2L,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVZXBW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0030380F, 0x00,
        B_SREGI16x8 | B_CHG,
        B_SSEI8x8L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVZXBW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000030, 0x00,
        B_SREGI16x8 | B_CHG,
        B_SSEI8x8L,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVZXBD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0031380F, 0x00,
        B_SREGI32x4 | B_CHG,
        B_SSEI8x4L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVZXBD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000031, 0x00,
        B_SREGI32x4 | B_CHG,
        B_SSEI8x4L,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVZXBQ"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0032380F, 0x00,
        B_SREGI64x2 | B_CHG,
        B_SSEI8x2L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVZXBQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000032, 0x00,
        B_SREGI64x2 | B_CHG,
        B_SSEI8x2L,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVZXWD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0033380F, 0x00,
        B_SREGI32x4 | B_CHG,
        B_SSEI16x4L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVZXWD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000033, 0x00,
        B_SREGI32x4 | B_CHG,
        B_SSEI16x4L,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVZXWQ"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0034380F, 0x00,
        B_SREGI64x2 | B_CHG,
        B_SSEI16x2L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVZXWQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000034, 0x00,
        B_SREGI64x2 | B_CHG,
        B_SSEI16x2L,
        B_NONE,
        B_NONE
    },

    {
        T("PMOVZXDQ"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0035380F, 0x00,
        B_SREGI64x2 | B_CHG,
        B_SSEI32x2L,
        B_NONE,
        B_NONE
    },

    {
        T("VPMOVZXDQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000035, 0x00,
        B_SREGI64x2 | B_CHG,
        B_SSEI32x2L,
        B_NONE,
        B_NONE
    },

    {
        T("PMULDQ"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0028380F, 0x00,
        B_SREGI32x4 | B_SIGNED | B_UPD,
        B_SSEI32x4 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPMULDQ"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000028, 0x00,
        B_SREGI32x4 | B_SIGNED | B_UPD,
        B_SVEXI32x4 | B_SIGNED,
        B_SSEI32x4 | B_SIGNED,
        B_NONE
    },

    {
        T("PMULLD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0040380F, 0x00,
        B_SREGI32x4 | B_SIGNED | B_UPD,
        B_SSEI32x4 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPMULLD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000040, 0x00,
        B_SREGI32x4 | B_SIGNED | B_UPD,
        B_SVEXI32x4 | B_SIGNED,
        B_SSEI32x4 | B_SIGNED,
        B_NONE
    },

    {
        T("PTEST"),
        D_SSE | D_MUST66 | D_ALLFLAGS, 0,
        3, 0x00FFFFFF, 0x0017380F, 0x00,
        B_SREGI32x4,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPTEST"),
        D_AVX | D_MUST66 | D_ALLFLAGS, DX_VEX | DX_LBOTH | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x00000017, 0x00,
        B_SREGI32x4,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("ROUNDPD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00093A0F, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VROUNDPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG | DX_LEAD3A,
        1, 0x000000FF, 0x00000009, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("ROUNDPS"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00083A0F, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VROUNDPS"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG | DX_LEAD3A,
        1, 0x000000FF, 0x00000008, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("ROUNDSD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x000B3A0F, 0x00,
        B_SREGF64L | B_CHG,
        B_SSEF64L,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VROUNDSD"),
        D_AVX | D_MUST66, DX_VEX | DX_IGNOREL | DX_LEAD3A,
        1, 0x000000FF, 0x0000000B, 0x00,
        B_SREGF64L | B_CHG,
        B_SVEXF64L,
        B_SSEF64L,
        B_CONST8 | B_BINARY
    },

    {
        T("ROUNDSS"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x000A3A0F, 0x00,
        B_SREGF32L | B_CHG,
        B_SSEF32L,
        B_CONST8 | B_BINARY,
        B_NONE
    },

    {
        T("VROUNDSS"),
        D_AVX | D_MUST66, DX_VEX | DX_IGNOREL | DX_LEAD3A,
        1, 0x000000FF, 0x0000000A, 0x00,
        B_SREGF32L | B_CHG,
        B_SVEXF32L,
        B_SSEF32L,
        B_CONST8 | B_BINARY
    },

    {
        T("PABSB"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x001C380F, 0x00,
        B_MREG8x8 | B_UNSIGNED | B_CHG,
        B_MMX8x8 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("PABSB"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x001C380F, 0x00,
        B_SREGI8x16 | B_UNSIGNED | B_CHG,
        B_SSEI8x16 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPABSB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x0000001C, 0x00,
        B_SREGI8x16 | B_UNSIGNED | B_CHG,
        B_SSEI8x16 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("PABSW"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x001D380F, 0x00,
        B_MREG16x4 | B_UNSIGNED | B_CHG,
        B_MMX16x4 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("PABSW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x001D380F, 0x00,
        B_SREGI16x8 | B_UNSIGNED | B_CHG,
        B_SSEI16x8 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPABSW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x0000001D, 0x00,
        B_SREGI16x8 | B_UNSIGNED | B_CHG,
        B_SSEI16x8 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("PABSD"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x001E380F, 0x00,
        B_MREG32x2 | B_UNSIGNED | B_CHG,
        B_MMX32x2 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("PABSD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x001E380F, 0x00,
        B_SREGI32x4 | B_UNSIGNED | B_CHG,
        B_SSEI32x4 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPABSD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x0000001E, 0x00,
        B_SREGI32x4 | B_UNSIGNED | B_CHG,
        B_SSEI32x4 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("PALIGNR"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x000F3A0F, 0x00,
        B_MREG8x8 | B_BINARY | B_UPD,
        B_MMX8x8 | B_BINARY,
        B_CONST8 | B_UNSIGNED,
        B_NONE
    },

    {
        T("PALIGNR"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x000F3A0F, 0x00,
        B_SREGI8x16 | B_BINARY | B_UPD,
        B_SSEI8x16 | B_BINARY,
        B_CONST8 | B_UNSIGNED,
        B_NONE
    },

    {
        T("VPALIGNR"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD3A,
        1, 0x000000FF, 0x0000000F, 0x00,
        B_SREGI8x16 | B_BINARY | B_UPD,
        B_SVEXI8x16 | B_BINARY,
        B_SSEI8x16 | B_BINARY,
        B_CONST8 | B_UNSIGNED
    },

    {
        T("PHADDW"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x0001380F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PHADDW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0001380F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPHADDW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000001, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PHADDD"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x0002380F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PHADDD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0002380F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPHADDD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000002, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PHSUBW"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x0005380F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PHSUBW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0005380F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPHSUBW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000005, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PHSUBD"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x0006380F, 0x00,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("PHSUBD"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0006380F, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SSEI32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VPHSUBD"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000006, 0x00,
        B_SREGI32x4 | B_UPD,
        B_SVEXI32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("PHADDSW"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x0003380F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PHADDSW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0003380F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPHADDSW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000003, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PHSUBSW"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x0007380F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PHSUBSW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0007380F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPHSUBSW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000007, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PMADDUBSW"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x0004380F, 0x00,
        B_MREG8x8 | B_UNSIGNED | B_UPD,
        B_MMX8x8 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("PMADDUBSW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0004380F, 0x00,
        B_SREGI8x16 | B_UNSIGNED | B_UPD,
        B_SSEI8x16 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPMADDUBSW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000004, 0x00,
        B_SREGI8x16 | B_UNSIGNED | B_UPD,
        B_SVEXI8x16 | B_UNSIGNED,
        B_SSEI8x16 | B_SIGNED,
        B_NONE
    },

    {
        T("PMULHRSW"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x000B380F, 0x00,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PMULHRSW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x000B380F, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SSEI16x8,
        B_NONE,
        B_NONE
    },

    {
        T("VPMULHRSW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x0000000B, 0x00,
        B_SREGI16x8 | B_UPD,
        B_SVEXI16x8,
        B_SSEI16x8,
        B_NONE
    },

    {
        T("PSHUFB"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x0000380F, 0x00,
        B_MREG8x8 | B_UPD,
        B_MMX8x8 | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("PSHUFB"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0000380F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16 | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("VPSHUFB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000000, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SVEXI8x16,
        B_SSEI8x16 | B_BINARY,
        B_NONE
    },

    {
        T("PSIGNB"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x0008380F, 0x00,
        B_MREG8x8 | B_SIGNED | B_UPD,
        B_MMX8x8 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("PSIGNB"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0008380F, 0x00,
        B_SREGI8x16 | B_SIGNED | B_UPD,
        B_SSEI8x16 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPSIGNB"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000008, 0x00,
        B_SREGI8x16 | B_SIGNED | B_UPD,
        B_SVEXI8x16 | B_SIGNED,
        B_SSEI8x16 | B_SIGNED,
        B_NONE
    },

    {
        T("PSIGNW"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x0009380F, 0x00,
        B_MREG16x4 | B_SIGNED | B_UPD,
        B_MMX16x4 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("PSIGNW"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x0009380F, 0x00,
        B_SREGI16x8 | B_SIGNED | B_UPD,
        B_SSEI16x8 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPSIGNW"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x00000009, 0x00,
        B_SREGI16x8 | B_SIGNED | B_UPD,
        B_SVEXI16x8 | B_SIGNED,
        B_SSEI16x8 | B_SIGNED,
        B_NONE
    },

    {
        T("PSIGND"),
        D_MMX | D_MUSTNONE, 0,
        3, 0x00FFFFFF, 0x000A380F, 0x00,
        B_MREG32x2 | B_SIGNED | B_UPD,
        B_MMX32x2 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("PSIGND"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x000A380F, 0x00,
        B_SREGI32x4 | B_SIGNED | B_UPD,
        B_SSEI32x4 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("VPSIGND"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x0000000A, 0x00,
        B_SREGI32x4 | B_SIGNED | B_UPD,
        B_SVEXI32x4 | B_SIGNED,
        B_SSEI32x4 | B_SIGNED,
        B_NONE
    },

    {
        T("VBROADCASTSS"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LBOTH | DX_NOVREG | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x00000018, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32L | B_MEMORY,
        B_NONE,
        B_NONE
    },

    {
        T("VBROADCASTSD"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LLONG | DX_NOVREG | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x00000019, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64L | B_MEMORY,
        B_NONE,
        B_NONE
    },

    {
        T("VBROADCASTF128"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LLONG | DX_NOVREG | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x0000001A, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2 | B_MEMORY | B_NOVEXSIZE | B_SHOWSIZE,
        B_NONE,
        B_NONE
    },

    {
        T("VEXTRACTF128"),
        D_AVX | D_MUST66, DX_VEX | DX_LLONG | DX_NOVREG | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x00000019, 0x00,
        B_SSEF64x2 | B_NOVEXSIZE | B_SHOWSIZE | B_CHG,
        B_SREGF64x2,
        B_CONST8,
        B_NONE
    },

    {
        T("VINSERTF128"),
        D_AVX | D_MUST66, DX_VEX | DX_LLONG | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x00000018, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2 | B_NOVEXSIZE | B_SHOWSIZE,
        B_CONST8
    },

    {
        T("VMASKMOVPS"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LBOTH | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x0000002C, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEF32x4 | B_MEMORY,
        B_NONE
    },

    {
        T("VMASKMOVPS"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LBOTH | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x0000002E, 0x00,
        B_SSEF32x4 | B_MEMORY | B_CHG,
        B_SVEXF32x4,
        B_SREGF32x4,
        B_NONE
    },

    {
        T("VMASKMOVPD"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LBOTH | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x0000002D, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2 | B_MEMORY,
        B_NONE
    },

    {
        T("VMASKMOVPD"),
        D_AVX | D_MUST66 | D_MEMORY, DX_VEX | DX_LBOTH | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x0000002F, 0x00,
        B_SSEF64x2 | B_MEMORY | B_CHG,
        B_SVEXF64x2,
        B_SREGF64x2,
        B_NONE
    },

    {
        T("VPERMILPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x0000000D, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEI64x2,
        B_NONE
    },

    {
        T("VPERMILPD"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x00000005, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SSEF64x2,
        B_CONST8,
        B_NONE
    },

    {
        T("VPERMILPS"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x0000000C, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SVEXF32x4,
        B_SSEI32x4,
        B_NONE
    },

    {
        T("VPERMILPS"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x00000004, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEF32x4,
        B_CONST8,
        B_NONE
    },

    {
        T("VPERM2F128"),
        D_AVX | D_MUST66, DX_VEX | DX_LLONG | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x00000006, 0x00,
        B_SREGF64x2 | B_CHG,
        B_SVEXF64x2,
        B_SSEF64x2,
        B_CONST8
    },

    {
        T("VTESTPS"),
        D_AVX | D_MUST66 | D_ALLFLAGS, DX_VEX | DX_LBOTH | DX_NOVREG | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x0000000E, 0x00,
        B_SREGF32x4,
        B_SSEF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("VTESTPD"),
        D_AVX | D_MUST66 | D_ALLFLAGS, DX_VEX | DX_LBOTH | DX_NOVREG | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x0000000F, 0x00,
        B_SREGF64x2,
        B_SSEF64x2,
        B_NONE,
        B_NONE
    },

    {
        T("VZEROALL"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LLONG | DX_NOVREG,
        1, 0x000000FF, 0x00000077, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VZEROUPPER"),
        D_AVX | D_MUSTNONE, DX_VEX | DX_LSHORT | DX_NOVREG,
        1, 0x000000FF, 0x00000077, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("AESDEC"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00DE380F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VAESDEC"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x000000DE, 0x00,
        B_SREGI8x16 | B_CHG,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("AESDECLAST"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00DF380F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VAESDECLAST"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x000000DF, 0x00,
        B_SREGI8x16 | B_CHG,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("AESENC"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00DC380F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VAESENC"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x000000DC, 0x00,
        B_SREGI8x16 | B_CHG,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("AESENCLAST"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00DD380F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VAESENCLAST"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_LEAD38,
        1, 0x000000FF, 0x000000DD, 0x00,
        B_SREGI8x16 | B_CHG,
        B_SVEXI8x16,
        B_SSEI8x16,
        B_NONE
    },

    {
        T("AESIMC"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00DB380F, 0x00,
        B_SREGI8x16 | B_CHG,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("VAESIMC"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD38,
        1, 0x000000FF, 0x000000DB, 0x00,
        B_SREGI8x16 | B_CHG,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("AESKEYGENASSIST"),
        D_SSE | D_MUST66, 0,
        3, 0x00FFFFFF, 0x00DF3A0F, 0x00,
        B_SREGI8x16 | B_CHG,
        B_SSEI8x16,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("VAESKEYGENASSIST"),
        D_AVX | D_MUST66, DX_VEX | DX_LSHORT | DX_NOVREG | DX_LEAD3A,
        1, 0x000000FF, 0x000000DF, 0x00,
        B_SREGI8x16 | B_CHG,
        B_SSEI8x16,
        B_CONST8 | B_COUNT,
        B_NONE
    },

    {
        T("VCVTPH2PS"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG | DX_W0 | DX_LEAD38,
        1, 0x000000FF, 0x00000013, 0x00,
        B_SREGF32x4 | B_CHG,
        B_SSEI16x4L,
        B_NONE,
        B_NONE
    },

    {
        T("VCVTPS2PH"),
        D_AVX | D_MUST66, DX_VEX | DX_LBOTH | DX_NOVREG | DX_W0 | DX_LEAD3A,
        1, 0x000000FF, 0x0000001D, 0x00,
        B_SSEI16x4L | B_CHG,
        B_SREGF32x4,
        B_NONE,
        B_NONE
    },

    {
        T("LZCNT"),
        D_CMD | D_NEEDF3 | D_ALLFLAGS, 0,
        2, 0x0000FFFF, 0x0000BD0F, 0x00,
        B_REG | B_CHG,
        B_INT | B_BINARY,
        B_NONE,
        B_NONE
    },

    {
        T("POPCNT"),
        D_CMD | D_NEEDF3 | D_ALLFLAGS, 0,
        2, 0x0000FFFF, 0x0000B80F, 0x00,
        B_REG | B_CHG,
        B_INT | B_NOADDR,
        B_NONE,
        B_NONE
    },

    {
        T("EXTRQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0038FFFF, 0x0000780F, 0x00,
        B_SSEI8x16 | B_REGONLY | B_UPD,
        B_CONST8 | B_COUNT,
        B_CONST8_2 | B_COUNT,
        B_NONE
    },

    {
        T("EXTRQ"),
        D_SSE | D_MUST66, 0,
        2, 0x0000FFFF, 0x0000790F, 0x00,
        B_SREGI8x16 | B_UPD,
        B_SSEI8x2L | B_REGONLY,
        B_NONE,
        B_NONE
    },

    {
        T("INSERTQ"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000780F, 0x00,
        B_SREGI8x16 | B_REGONLY | B_UPD,
        B_SSEI8x8L,
        B_CONST8 | B_COUNT,
        B_CONST8_2 | B_COUNT
    },

    {
        T("INSERTQ"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x0000790F, 0x00,
        B_SREGI8x16 | B_REGONLY | B_UPD,
        B_SSEI8x16,
        B_NONE,
        B_NONE
    },

    {
        T("MOVNTSD"),
        D_SSE | D_MUSTF2, 0,
        2, 0x0000FFFF, 0x00002B0F, 0x00,
        B_SSEF64L | B_MEMONLY | B_CHG,
        B_SREGF64L,
        B_NONE,
        B_NONE
    },

    {
        T("MOVNTSS"),
        D_SSE | D_MUSTF3, 0,
        2, 0x0000FFFF, 0x00002B0F, 0x00,
        B_SSEF32L | B_MEMONLY | B_CHG,
        B_SREGF32L,
        B_NONE,
        B_NONE
    },

    {
        T("INVEPT"),
        D_PRIVILEGED | D_MUST66 | D_MEMORY | D_RARE, 0,
        3, 0x00FFFFFF, 0x0080380F, 0x00,
        B_REG32,
        B_INT128,
        B_NONE,
        B_NONE
    },

    {
        T("INVVPID"),
        D_PRIVILEGED | D_MUST66 | D_MEMORY | D_RARE, 0,
        3, 0x00FFFFFF, 0x0081380F, 0x00,
        B_REG32,
        B_INT128,
        B_NONE,
        B_NONE
    },

    {
        T("VMCALL"),
        D_PRIVILEGED | D_RARE, 0,
        3, 0x00FFFFFF, 0x00C1010F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMCLEAR"),
        D_PRIVILEGED | D_MUST66 | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0030C70F, 0x00,
        B_INT64 | B_MEMONLY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMLAUNCH"),
        D_PRIVILEGED | D_RARE, 0,
        3, 0x00FFFFFF, 0x00C2010F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMFUNC"),
        D_PRIVILEGED | D_RARE, 0,
        3, 0x00FFFFFF, 0x00D4010F, 0x00,
        B_EAX | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("XEND"),
        D_PRIVILEGED | D_RARE, 0,
        3, 0x00FFFFFF, 0x00D5010F, 0x00,
        B_EAX | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("XTEST"),
        D_PRIVILEGED | D_RARE, 0,
        3, 0x00FFFFFF, 0x00D6010F, 0x00,
        B_EAX | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMRESUME"),
        D_PRIVILEGED | D_RARE, 0,
        3, 0x00FFFFFF, 0x00C3010F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMPTRLD"),
        D_PRIVILEGED | D_MUSTNONE | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0030C70F, 0x00,
        B_INT64 | B_MEMONLY,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMPTRST"),
        D_PRIVILEGED | D_MUSTNONE | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0038C70F, 0x00,
        B_INT64 | B_MEMONLY | B_CHG,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMREAD"),
        D_PRIVILEGED | D_MUSTNONE | D_RARE, 0,
        2, 0x0000FFFF, 0x0000780F, 0x00,
        B_INT32 | B_CHG,
        B_REG32,
        B_NONE,
        B_NONE
    },

    {
        T("VMWRITE"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000790F, 0x00,
        B_REG32,
        B_INT32,
        B_NONE,
        B_NONE
    },

    {
        T("VMXOFF"),
        D_PRIVILEGED | D_RARE, 0,
        3, 0x00FFFFFF, 0x00C4010F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMXON"),
        D_PRIVILEGED | D_MUSTF3 | D_MEMORY | D_RARE, 0,
        2, 0x0038FFFF, 0x0030C70F, 0x00,
        B_INT64,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("GETSEC"),
        D_PRIVILEGED | D_RARE, 0,
        2, 0x0000FFFF, 0x0000370F, 0x00,
        B_EAX | B_UPD | B_PSEUDO,
        B_EBX | B_PSEUDO,
        B_ECX | B_PSEUDO,
        B_NONE
    },

    {
        T("FEMMS"),
        D_CMD, 0,
        2, 0x0000FFFF, 0x00000E0F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("PAVGUSB"),
        D_MMX | D_POSTBYTE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xBF,
        B_MREG8x8 | B_UPD,
        B_MMX8x8,
        B_NONE,
        B_NONE
    },

    {
        T("PF2ID"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x1D,
        B_MREG32x2 | B_CHG,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFACC"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xAE,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFADD"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x9E,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFCMPEQ"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xB0,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFCMPGE"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x90,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFCMPGT"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xA0,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFMAX"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xA4,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFMIN"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x94,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFMUL"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xB4,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFRCP"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x96,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFRCPIT1"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xA6,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFRCPIT2"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xB6,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFRSQIT1"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xA7,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFRSQRT"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x97,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFSUB"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x9A,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFSUBR"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xAA,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PI2FD"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x0D,
        B_3DREG | B_UPD,
        B_MMX32x2 | B_SIGNED,
        B_NONE,
        B_NONE
    },

    {
        T("PMULHRW"),
        D_MMX | D_POSTBYTE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xB7,
        B_MREG16x4 | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PF2IW"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x1C,
        B_MREG32x2 | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFNACC"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x8A,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PFPNACC"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x8E,
        B_3DREG | B_UPD,
        B_3DNOW,
        B_NONE,
        B_NONE
    },

    {
        T("PI2FW"),
        D_3DNOW | D_POSTBYTE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0x0C,
        B_3DREG | B_UPD,
        B_MMX16x4,
        B_NONE,
        B_NONE
    },

    {
        T("PSWAPD"),
        D_MMX | D_POSTBYTE | D_MUSTNONE, 0,
        2, 0x0000FFFF, 0x00000F0F, 0xBB,
        B_MREG32x2 | B_UPD,
        B_MMX32x2,
        B_NONE,
        B_NONE
    },

    {
        T("SYSCALL"),
        D_SYS | D_RARE, 0,
        2, 0x0000FFFF, 0x0000050F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SYSRET"),
        D_SYS | D_ALLFLAGS | D_SUSPICIOUS, 0,
        2, 0x0000FFFF, 0x0000070F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("CLGI"),
        D_PRIVILEGED, 0,
        3, 0x00FFFFFF, 0x00DD010F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("STGI"),
        D_PRIVILEGED, 0,
        3, 0x00FFFFFF, 0x00DC010F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("INVLPGA"),
        D_PRIVILEGED | D_RARE, 0,
        3, 0x00FFFFFF, 0x00DF010F, 0x00,
        B_EAX | B_PSEUDO,
        B_ECX | B_PSEUDO,
        B_NONE,
        B_NONE
    },

    {
        T("SKINIT"),
        D_PRIVILEGED, 0,
        3, 0x00FFFFFF, 0x00DE010F, 0x00,
        B_EAX | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMLOAD"),
        D_PRIVILEGED, 0,
        3, 0x00FFFFFF, 0x00DA010F, 0x00,
        B_EAX | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMMCALL"),
        D_SYS | D_SUSPICIOUS, 0,
        3, 0x00FFFFFF, 0x00D9010F, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMRUN"),
        D_PRIVILEGED, 0,
        3, 0x00FFFFFF, 0x00D8010F, 0x00,
        B_EAX | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("VMSAVE"),
        D_PRIVILEGED, 0,
        3, 0x00FFFFFF, 0x00DB010F, 0x00,
        B_EAX | B_PSEUDO,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("ES:"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x00000026, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("CS:"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x0000002E, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SS:"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x00000036, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("DS:"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x0000003E, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("FS:"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x00000064, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("GS:"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x00000065, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("DATASIZE:"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x00000066, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("ADDRSIZE:"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x00000067, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("LOCK"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x000000F0, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("REPNE"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x000000F2, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("REPNZ"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x000000F2, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("REP"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x000000F3, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("REPE"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x000000F3, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("REPZ"),
        D_PREFIX | D_SUSPICIOUS, 0,
        1, 0x000000FF, 0x000000F3, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JCC"),
        D_PSEUDO | D_BHINT | D_COND, 0,
        1, 0x000000F0, 0x00000070, 0x00,
        B_BYTEOFFS | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("JCC"),
        D_PSEUDO | D_BHINT | D_COND, 0,
        2, 0x0000F0FF, 0x0000800F, 0x00,
        B_OFFSET | B_JMPCALL,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("SETCC"),
        D_PSEUDO | D_COND, 0,
        2, 0x0038F0FF, 0x0000900F, 0x00,
        B_INT8 | B_SHOWSIZE | B_CHG,
        B_NONE,
        B_NONE,
        B_NONE
    },

    {
        T("CMOVCC"),
        D_PSEUDO | D_COND, 0,
        2, 0x0000F0FF, 0x0000400F, 0x00,
        B_REG | B_UPD,
        B_INT,
        B_NONE,
        B_NONE
    },

    {
        T("FCMOVCC"),
        D_PSEUDO | D_COND, 0,
        2, 0x0000E0FF, 0x0000C0DA, 0x00,
        B_ST0 | B_CHG,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        T("FCMOVCC"),
        D_PSEUDO | D_COND, 0,
        2, 0x0000E0FF, 0x0000C0DB, 0x00,
        B_ST0 | B_CHG,
        B_ST,
        B_NONE,
        B_NONE
    },

    {
        NULL,
        0, 0,
        0, 0x00000000, 0x00000000, 0x00,
        B_NONE,
        B_NONE,
        B_NONE,
        B_NONE
    }

};

