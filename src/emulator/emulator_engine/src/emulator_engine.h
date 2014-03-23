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
#include "../../../global/defines.h"
#include "../../../global/typedefs.h"

/* system multithreading */
#include "../../../system/multithreading/event/c/event.h"
#include "../../../system/multithreading/mutex/c/cmutex.h"
#include "../../../system/multithreading/thread/c/thread.h"

/* macros */

#define WRITE_DEBUG(msg)\
	write_debug(__FILE__, __LINE__, msg)

//#define op_0x00 op_unknown
//#define op_0x01 op_unknown
//#define op_0x02 op_unknown
//#define op_0x03 op_unknown
//#define op_0x04 op_unknown
//#define op_0x05 op_unknown
#define op_0x06 op_unknown
#define op_0x07 op_unknown
#define op_0x08 op_unknown
#define op_0x09 op_unknown
#define op_0x0a op_unknown
#define op_0x0b op_unknown
#define op_0x0c op_unknown
#define op_0x0d op_unknown
#define op_0x0e op_unknown
#define op_0x0f op_unknown
//#define op_0x10 op_unknown
//#define op_0x11 op_unknown
//#define op_0x12 op_unknown
//#define op_0x13 op_unknown
//#define op_0x14 op_unknown
//#define op_0x15 op_unknown
#define op_0x16 op_unknown
#define op_0x17 op_unknown
#define op_0x18 op_unknown
#define op_0x19 op_unknown
#define op_0x1a op_unknown
#define op_0x1b op_unknown
#define op_0x1c op_unknown
#define op_0x1d op_unknown
#define op_0x1e op_unknown
#define op_0x1f op_unknown
#define op_0x20 op_unknown
#define op_0x21 op_unknown
#define op_0x22 op_unknown
#define op_0x23 op_unknown
#define op_0x24 op_unknown
#define op_0x25 op_unknown
#define op_0x26 op_unknown
#define op_0x27 op_unknown
//#define op_0x28 op_unknown
//#define op_0x29 op_unknown
//#define op_0x2a op_unknown
#define op_0x2b op_unknown
//#define op_0x2c op_unknown
//#define op_0x2d op_unknown
#define op_0x2e op_unknown
#define op_0x2f op_unknown
#define op_0x30 op_unknown
#define op_0x31 op_unknown
#define op_0x32 op_unknown
#define op_0x33 op_unknown
#define op_0x34 op_unknown
#define op_0x35 op_unknown
#define op_0x36 op_unknown
#define op_0x37 op_unknown
//#define op_0x38 op_unknown
//#define op_0x39 op_unknown
//#define op_0x3a op_unknown
//#define op_0x3b op_unknown
//#define op_0x3c op_unknown
//#define op_0x3d op_unknown
#define op_0x3e op_unknown
#define op_0x3f op_unknown
#define op_0x40 op_unknown
#define op_0x41 op_unknown
#define op_0x42 op_unknown
#define op_0x43 op_unknown
#define op_0x44 op_unknown
#define op_0x45 op_unknown
#define op_0x46 op_unknown
#define op_0x47 op_unknown
#define op_0x48 op_unknown
#define op_0x49 op_unknown
#define op_0x4a op_unknown
#define op_0x4b op_unknown
#define op_0x4c op_unknown
#define op_0x4d op_unknown
#define op_0x4e op_unknown
#define op_0x4f op_unknown
//#define op_0x50 op_unknown
#define op_0x51 op_0x50
#define op_0x52 op_0x50
#define op_0x53 op_0x50
#define op_0x54 op_0x50
#define op_0x55 op_0x50
#define op_0x56 op_0x50
#define op_0x57 op_0x50
//#define op_0x58 op_unknown
#define op_0x59 op_0x58
#define op_0x5a op_0x58
#define op_0x5b op_0x58
#define op_0x5c op_0x58
#define op_0x5d op_0x58
#define op_0x5e op_0x58
#define op_0x5f op_0x58
#define op_0x60 op_unknown
#define op_0x61 op_unknown
#define op_0x62 op_unknown
#define op_0x63 op_unknown
#define op_0x64 op_unknown
#define op_0x65 op_unknown
#define op_0x66 op_unknown
#define op_0x67 op_unknown
//#define op_0x68 op_unknown
#define op_0x69 op_unknown
//#define op_0x6a op_unknown
#define op_0x6b op_unknown
#define op_0x6c op_unknown
#define op_0x6d op_unknown
#define op_0x6e op_unknown
#define op_0x6f op_unknown
#define op_0x70 op_unknown
#define op_0x71 op_unknown
#define op_0x72 op_unknown
#define op_0x73 op_unknown
#define op_0x74 op_unknown
#define op_0x75 op_unknown
#define op_0x76 op_unknown
#define op_0x77 op_unknown
#define op_0x78 op_unknown
#define op_0x79 op_unknown
#define op_0x7a op_unknown
#define op_0x7b op_unknown
#define op_0x7c op_unknown
#define op_0x7d op_unknown
#define op_0x7e op_unknown
#define op_0x7f op_unknown
//#define op_0x80 op_unknown
//#define op_0x81 op_unknown
#define op_0x82 op_unknown
//#define op_0x83 op_unknown
#define op_0x84 op_unknown
#define op_0x85 op_unknown
#define op_0x86 op_unknown
#define op_0x87 op_unknown
//#define op_0x88 op_unknown
//#define op_0x89 op_unknown
//#define op_0x8a op_unknown
//#define op_0x8b op_unknown
//#define op_0x8c op_unknown
#define op_0x8d op_unknown
//#define op_0x8e op_unknown
//#define op_0x8f op_unknown
#define op_0x90 op_unknown
#define op_0x91 op_unknown
#define op_0x92 op_unknown
#define op_0x93 op_unknown
#define op_0x94 op_unknown
#define op_0x95 op_unknown
#define op_0x96 op_unknown
#define op_0x97 op_unknown
#define op_0x98 op_unknown
#define op_0x99 op_unknown
#define op_0x9a op_unknown
#define op_0x9b op_unknown
#define op_0x9c op_unknown
#define op_0x9d op_unknown
#define op_0x9e op_unknown
#define op_0x9f op_unknown
//#define op_0xa0 op_unknown
//#define op_0xa1 op_unknown
//#define op_0xa2 op_unknown
//#define op_0xa3 op_unknown
#define op_0xa4 op_unknown
#define op_0xa5 op_unknown
#define op_0xa6 op_unknown
#define op_0xa7 op_unknown
#define op_0xa8 op_unknown
#define op_0xa9 op_unknown
#define op_0xaa op_unknown
#define op_0xab op_unknown
#define op_0xac op_unknown
#define op_0xad op_unknown
#define op_0xae op_unknown
#define op_0xaf op_unknown
//#define op_0xb0 op_unknown
#define op_0xb1 op_0xb0
#define op_0xb2 op_0xb0
#define op_0xb3 op_0xb0
#define op_0xb4 op_0xb0
#define op_0xb5 op_0xb0
#define op_0xb6 op_0xb0
#define op_0xb7 op_0xb0
//#define op_0xb8 op_unknown
#define op_0xb9 op_0xb8
#define op_0xba op_0xb8
#define op_0xbb op_0xb8
#define op_0xbc op_0xb8
#define op_0xbd op_0xb8
#define op_0xbe op_0xb8
#define op_0xbf op_0xb8
#define op_0xc0 op_unknown
#define op_0xc1 op_unknown
#define op_0xc2 op_unknown
#define op_0xc3 op_unknown
#define op_0xc4 op_unknown
#define op_0xc5 op_unknown
//#define op_0xc6 op_unknown
//#define op_0xc7 op_unknown
#define op_0xc8 op_unknown
#define op_0xc9 op_unknown
#define op_0xca op_unknown
#define op_0xcb op_unknown
#define op_0xcc op_unknown
#define op_0xcd op_unknown
#define op_0xce op_unknown
#define op_0xcf op_unknown
#define op_0xd0 op_unknown
#define op_0xd1 op_unknown
#define op_0xd2 op_unknown
#define op_0xd3 op_unknown
#define op_0xd4 op_unknown
#define op_0xd5 op_unknown
#define op_0xd6 op_unknown
#define op_0xd7 op_unknown
#define op_0xd8 op_unknown
#define op_0xd9 op_unknown
#define op_0xda op_unknown
#define op_0xdb op_unknown
#define op_0xdc op_unknown
#define op_0xdd op_unknown
#define op_0xde op_unknown
#define op_0xdf op_unknown
#define op_0xe0 op_unknown
#define op_0xe1 op_unknown
#define op_0xe2 op_unknown
#define op_0xe3 op_unknown
//#define op_0xe4 op_unknown
//#define op_0xe5 op_unknown
//#define op_0xe6 op_unknown
//#define op_0xe7 op_unknown
#define op_0xe8 op_unknown
#define op_0xe9 op_unknown
#define op_0xea op_unknown
//#define op_0xeb op_unknown
//#define op_0xec op_unknown
//#define op_0xed op_unknown
//#define op_0xee op_unknown
//#define op_0xef op_unknown
#define op_0xf0 op_unknown
#define op_0xf1 op_unknown
#define op_0xf2 op_unknown
#define op_0xf3 op_unknown
#define op_0xf4 op_unknown
#define op_0xf5 op_unknown
#define op_0xf6 op_unknown
#define op_0xf7 op_unknown
#define op_0xf8 op_unknown
#define op_0xf9 op_unknown
#define op_0xfa op_unknown
#define op_0xfb op_unknown
#define op_0xfc op_unknown
#define op_0xfd op_unknown
#define op_0xfe op_unknown
//#define op_0xff op_unknown

/* system state structure */

#define IO_BUS_SIZE RANGE_16
/*	maximum addressable memory with segmentation
	is 16*65536 = 1 meg
*/
#define MEM_SIZE 1048576

/*	opcode data structure
	program may expect the prefixes to be initialized as boolean values,
	signifying the presence of the prefixes.

	mod reg r/m field may not be initialized or have meaningful information
	depending on what is accessing it
*/
typedef struct{
	/* prefixes */
	int lock, rep, repne;	/* nonzero for present, 0 for not present */
	uint8 seg_ovr;			/* segment override, 0 if none present */
	union{
		struct{
			uint8 rm:3;
			uint8 reg:3;
			uint8 mod:2;
		};
		uint8 mod_reg_rm;
	};

	/*	these are the segments for the instruction to use
		if not overridden, they will be their defaults 
	*/

	uint16 cs, ss, ds, es;

	uint8 op_size;
}op_data_type;

#define MOD_REG_RM(offset)\
	op_data.mod_reg_rm=read_mem_8(R_IP+offset);

/* memory access */

/* get the real address of a segment address and segment number */
#define GET_ADDR(address, segment)\
	(segment*0x10)+address

/* register access */
#define REG_16(rg) *((uint16*)sys_state.rg)
#define REG_8_LO(rg) *((uint8*)sys_state.rg+0)	/* assuming little endian host machine */
#define REG_8_HI(rg) *((uint8*)sys_state.rg+1)

#define AX REG_16(ax)
#define AL REG_8_LO(ax)
#define AH REG_8_HI(ax)
#define BX REG_16(bx)
#define BL REG_8_LO(bx)
#define BH REG_8_HI(bx)
#define CX REG_16(cx)
#define CL REG_8_LO(cx)
#define CH REG_8_HI(cx)
#define DX REG_16(dx)
#define DL REG_8_LO(dx)
#define DH REG_8_HI(dx)

#define IP sys_state.ip
#define CS sys_state.cs
#define SS sys_state.ss
#define DS sys_state.ds
#define FS sys_state.fs
#define ES sys_state.es
#define FLAGS sys_state.flags

#define SP sys_state.sp
#define BP sys_state.bp
#define SI sys_state.si
#define DI sys_state.di

/* special R_IP access */
#define R_IP sys_state.r_ip

/* registers */
#define REG_AX	0
#define REG_CX	1
#define REG_DX	2
#define REG_BX	3
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
#define REG_AL	14
#define REG_CL	15
#define REG_DL	16
#define REG_BL	17
#define REG_AH	18
#define REG_CH	19
#define REG_DH	20
#define REG_BH	21

/*	converts a REG value (in mod reg r/m) to a
	emulator internal value (defined above)
	for use with 8-bit registers.
	For example, BREG(0) -> 14 (AH)
*/
#define BREG(reg) reg+14

#define TEXT_REG(reg) text_regs[reg]

typedef int (*op_func)();

void write_debug(char* file, int line, char* str);

uint16 read_mem_16(uint32 addr);
void write_mem_16(uint16 val, uint32 addr);
uint8 read_mem_8(uint32 addr);
void write_mem_8(uint8 val, uint32 addr);

void write_io_port(uint8 val, uint16 port);
uint8 read_io_port(uint16 port);

void system_load_mem(uint8* data, uint32 size);

void write_reg(int reg, uint16 val);
uint16 read_reg(int reg);

void out_opinfo(char* str, ...);

void stack_push(uint16 val);
uint16 stack_pop();

/* TODO */


#endif