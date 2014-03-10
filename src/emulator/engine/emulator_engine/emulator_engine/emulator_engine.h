/**
 * @file Emulator engine
 * x86 emulator engine for hard86
 * Stephen Zhang, 2014
 */

#ifndef EMULATOR_ENGINE_H
#define EMULATOR_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <wchar.h>

#include "emulator_engine.h"
#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"

/* system multithreading */
#include "../../../../system/multithreading/event/c/event.h"
#include "../../../../system/multithreading/mutex/c/mutex.h"
#include "../../../../system/multithreading/thread/c/thread.h"

/* macros */

#define WRITE_DEBUG(msg)\
	write_debug(__FILE__, __LINE__, msg)

#define op_0x00 op_unknown
#define op_0x01 op_unknown
#define op_0x01 op_unknown
#define op_0x02 op_unknown
#define op_0x02 op_unknown
#define op_0x03 op_unknown
#define op_0x03 op_unknown
#define op_0x03 op_unknown
#define op_0x04 op_unknown
#define op_0x04 op_unknown
#define op_0x05 op_unknown
#define op_0x05 op_unknown
#define op_0x05 op_unknown
#define op_0x05 op_unknown
#define op_0x06 op_unknown
#define op_0x06 op_unknown
#define op_0x07 op_unknown
#define op_0x07 op_unknown
#define op_0x07 op_unknown
#define op_0x07 op_unknown
#define op_0x08 op_unknown
#define op_0x08 op_unknown
#define op_0x08 op_unknown
#define op_0x09 op_unknown
#define op_0x09 op_unknown
#define op_0x09 op_unknown
#define op_0x09 op_unknown
#define op_0x0a op_unknown
#define op_0x0a op_unknown
#define op_0x0b op_unknown
#define op_0x0b op_unknown
#define op_0x0b op_unknown
#define op_0x0b op_unknown
#define op_0x0b op_unknown
#define op_0x0b op_unknown
#define op_0x0c op_unknown
#define op_0x0c op_unknown
#define op_0x0d op_unknown
#define op_0x0d op_unknown
#define op_0x0d op_unknown
#define op_0x0d op_unknown
#define op_0x0e op_unknown
#define op_0x0e op_unknown
#define op_0x0e op_unknown
#define op_0x0e op_unknown
#define op_0x0f op_unknown
#define op_0x0f op_unknown
#define op_0x0f op_unknown
#define op_0x0f op_unknown
#define op_0x0f op_unknown
#define op_0x11 op_unknown
#define op_0x11 op_unknown
#define op_0x11 op_unknown
#define op_0x11 op_unknown
#define op_0x13 op_unknown
#define op_0x13 op_unknown
#define op_0x13 op_unknown
#define op_0x13 op_unknown
#define op_0x14 op_unknown
#define op_0x14 op_unknown
#define op_0x15 op_unknown
#define op_0x15 op_unknown
#define op_0x17 op_unknown
#define op_0x17 op_unknown
#define op_0x17 op_unknown
#define op_0x17 op_unknown
#define op_0x17 op_unknown
#define op_0x17 op_unknown
#define op_0x18 op_unknown
#define op_0x19 op_unknown
#define op_0x19 op_unknown
#define op_0x1a op_unknown
#define op_0x1a op_unknown
#define op_0x1b op_unknown
#define op_0x1b op_unknown
#define op_0x1b op_unknown
#define op_0x1b op_unknown
#define op_0x1d op_unknown
#define op_0x1d op_unknown
#define op_0x1d op_unknown
#define op_0x1d op_unknown
#define op_0x1d op_unknown
#define op_0x1d op_unknown
#define op_0x1f op_unknown
#define op_0x1f op_unknown
#define op_0x1f op_unknown
#define op_0x1f op_unknown
#define op_0x20 op_unknown
#define op_0x20 op_unknown
#define op_0x22 op_unknown
#define op_0x22 op_unknown
#define op_0x23 op_unknown
#define op_0x23 op_unknown
#define op_0x23 op_unknown
#define op_0x23 op_unknown
#define op_0x23 op_unknown
#define op_0x26 op_unknown
#define op_0x26 op_unknown
#define op_0x27 op_unknown
#define op_0x27 op_unknown
#define op_0x27 op_unknown
#define op_0x27 op_unknown
#define op_0x29 op_unknown
#define op_0x29 op_unknown
#define op_0x29 op_unknown
#define op_0x29 op_unknown
#define op_0x2b op_unknown
#define op_0x2b op_unknown
#define op_0x2c op_unknown
#define op_0x2c op_unknown
#define op_0x2c op_unknown
#define op_0x2c op_unknown
#define op_0x2f op_unknown
#define op_0x2f op_unknown
#define op_0x2f op_unknown
#define op_0x2f op_unknown
#define op_0x2f op_unknown
#define op_0x2f op_unknown
#define op_0x30 op_unknown
#define op_0x31 op_unknown
#define op_0x31 op_unknown
#define op_0x33 op_unknown
#define op_0x33 op_unknown
#define op_0x35 op_unknown
#define op_0x35 op_unknown
#define op_0x36 op_unknown
#define op_0x36 op_unknown
#define op_0x37 op_unknown
#define op_0x37 op_unknown
#define op_0x37 op_unknown
#define op_0x37 op_unknown
#define op_0x3b op_unknown
#define op_0x3b op_unknown
#define op_0x3b op_unknown
#define op_0x3b op_unknown
#define op_0x3b op_unknown
#define op_0x3b op_unknown
#define op_0x3e op_unknown
#define op_0x3e op_unknown
#define op_0x3f op_unknown
#define op_0x3f op_unknown
#define op_0x3f op_unknown
#define op_0x40 op_unknown
#define op_0x40 op_unknown
#define op_0x41 op_unknown
#define op_0x41 op_unknown
#define op_0x45 op_unknown
#define op_0x45 op_unknown
#define op_0x45 op_unknown
#define op_0x45 op_unknown
#define op_0x47 op_unknown
#define op_0x47 op_unknown
#define op_0x47 op_unknown
#define op_0x47 op_unknown
#define op_0x4a op_unknown
#define op_0x4a op_unknown
#define op_0x4c op_unknown
#define op_0x4c op_unknown
#define op_0x4d op_unknown
#define op_0x4d op_unknown
#define op_0x4f op_unknown
#define op_0x4f op_unknown
#define op_0x4f op_unknown
#define op_0x4f op_unknown
#define op_0x50 op_unknown
#define op_0x53 op_unknown
#define op_0x53 op_unknown
#define op_0x53 op_unknown
#define op_0x53 op_unknown
#define op_0x57 op_unknown
#define op_0x57 op_unknown
#define op_0x59 op_unknown
#define op_0x59 op_unknown
#define op_0x59 op_unknown
#define op_0x59 op_unknown
#define op_0x5a op_unknown
#define op_0x5a op_unknown
#define op_0x5f op_unknown
#define op_0x5f op_unknown
#define op_0x5f op_unknown
#define op_0x5f op_unknown
#define op_0x61 op_unknown
#define op_0x61 op_unknown
#define op_0x62 op_unknown
#define op_0x62 op_unknown
#define op_0x63 op_unknown
#define op_0x67 op_unknown
#define op_0x67 op_unknown
#define op_0x68 op_unknown
#define op_0x68 op_unknown
#define op_0x6b op_unknown
#define op_0x6b op_unknown
#define op_0x6d op_unknown
#define op_0x6d op_unknown
#define op_0x6f op_unknown
#define op_0x6f op_unknown
#define op_0x6f op_unknown
#define op_0x6f op_unknown
#define op_0x74 op_unknown
#define op_0x74 op_unknown
#define op_0x77 op_unknown
#define op_0x77 op_unknown
#define op_0x77 op_unknown
#define op_0x77 op_unknown
#define op_0x78 op_unknown
#define op_0x7d op_unknown
#define op_0x7d op_unknown
#define op_0x7f op_unknown
#define op_0x7f op_unknown
#define op_0x81 op_unknown
#define op_0x81 op_unknown
#define op_0x83 op_unknown
#define op_0x83 op_unknown
#define op_0x86 op_unknown
#define op_0x86 op_unknown
#define op_0x8b op_unknown
#define op_0x8b op_unknown
#define op_0x8e op_unknown
#define op_0x8e op_unknown
#define op_0x8f op_unknown
#define op_0x8f op_unknown
#define op_0x8f op_unknown
#define op_0x95 op_unknown
#define op_0x95 op_unknown
#define op_0x99 op_unknown
#define op_0x99 op_unknown
#define op_0x9b op_unknown
#define op_0x9b op_unknown
#define op_0x9f op_unknown
#define op_0x9f op_unknown
#define op_0xa4 op_unknown
#define op_0xa4 op_unknown
#define op_0xa7 op_unknown
#define op_0xa7 op_unknown
#define op_0xa8 op_unknown
#define op_0xaf op_unknown
#define op_0xaf op_unknown
#define op_0xb3 op_unknown
#define op_0xb3 op_unknown
#define op_0xb5 op_unknown
#define op_0xb5 op_unknown
#define op_0xbf op_unknown
#define op_0xbf op_unknown
#define op_0xc2 op_unknown
#define op_0xc2 op_unknown
#define op_0xc3 op_unknown
#define op_0xcf op_unknown
#define op_0xcf op_unknown
#define op_0xd1 op_unknown
#define op_0xd1 op_unknown
#define op_0xdf op_unknown
#define op_0xdf op_unknown
#define op_0xe0 op_unknown
#define op_0xef op_unknown
#define op_0xef op_unknown
#define op_0xff op_unknown

/* register access */
#define REG_16(rg) *(uint16)sys_state.rg;
#define REG_8_LO(rg) *((uint8*)sys_state.rg+0);	/* assuming little endian host machine */
#define REG_8_HI(rg) *((uint8*)sys_state.rg+1);

#define AX REG_16(ax);
#define AL REG_8_LO(ax);
#define AH REG_8_HI(ax);
#define BX REG_16(bx);
#define BL REG_8_LO(bx);
#define BH REG_8_HI(bx);
#define CX REG_16(cx);
#define CL REG_8_LO(cx);
#define CH REG_8_HI(cx);
#define DX REG_16(dx);
#define DL REG_8_LO(dx);
#define DH REG_8_HI(dx);

/* registers */
#define REG_AX	0
#define REG_BX	1
#define REG_CX	2
#define REG_DX	3
#define REG_SP	4
#define REG_BP	5
#define REG_SI	6
#define REG_DI	7
#define REG_CS	8
#define REG_SS	9
#define REG_DS	10
#define REG_ES	11
#define REG_IP	12
#define REG_FLAGS 13
#define REG_AH	14
#define REG_AL	15
#define REG_BH	16
#define REG_BL	17
#define REG_CH	18
#define REG_CL	19
#define REG_DH	20
#define REG_DL	21

#define TEXT_REG(reg) text_regs[reg]

/* MB and KB, RANGE_8/16 definitions */
#define KB 1024
#define MB KB*KB
#define RANGE_8 256
#define RANGE_16 65536

typedef int (*op_func)();

void write_debug(char* str);
/* TODO */


#endif