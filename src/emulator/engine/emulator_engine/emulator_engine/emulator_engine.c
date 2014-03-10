/**
 * @file Emulator engine
 * x86 emulator engine for hard86
 * Stephen Zhang, 2014
 */

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

/* forward declaration of op_table */
op_func op_table[256];

/* textual representation of registers */
char text_regs[22][6]={
	"AX", "BX", "CX", "DX", "SP", "BP", "SI", "DI", "CS", "SS", "DS", "ES", "IP", "FLAGS",
	"AH", "AL", "BH", "BL", "CH", "CL", "DH", "DL"};

/* system state */
struct{
	/* system memory */
	uint8* mem;
	uint32 mem_size;
	/* registers */
	/*	general purpose
		AX, BX, CX, DX

		we store this as an array of uint8
		so we can easily access h.o byte and l.o byte
	*/
	uint8 ax[2];
	uint8 bx[2];
	uint8 cx[2];
	uint8 dx[2];

	uint16 sp;
	uint16 bp;
	uint16 si;
	uint16 di;
	
	/* special */
	/* segment registers */
	uint16 cs;
	uint16 ss;
	uint16 ds;
	uint16 es;

	/* pointers & indicators */
	uint16 ip;

	/*	flags register
		format :
		Bit# : 15	14	13/12	11	10	9	8	7	6	5	4	3	2	1	0
		Flag : 0	NT	IOPL	OF	DF	IF	TF	SF	ZF	0	AF	0	PF	1	CF
	*/
	union{
		struct{
			uint8 CF:1;
			uint8 reserved_1:1;	/* always 1 */
			uint8 PF:1;
			uint8 reserved_2:1; /* always 0 */
			uint8 AF:1;
			uint8 reserved_3:1;	/* always 0 */
			uint8 ZF:1;
			uint8 SF:1;
			uint8 TF:1;
			uint8 IF:1;
			uint8 DF:1;
			uint8 OF:1;
			uint8 IOPL:2;
			uint8 NT:1;
			uint8 reserved_4:1;	/* 1 on 8086, 0 above */
		};
		uint16 flags;
	};
}sys_state, *sys_state_ptr;

#define SET_RESERVED_FLAGS(state)\
	state.reserved_1=state.reserved_4=1;\
	state.reserved_2=state.reserved_3=0;

/* debug function */

/*static*/ void write_debug(char* file, int line, char* str){
	FILE* f=fopen("debug.log", "a");
	char s[1024]="";
	sprintf(s, "%s %d : %s\n", file, line, str);
	fprintf(f, "%s", s);
	fclose(f);
}

/**
 * reads 16-bit value from memory
 * @param addr address
 */
uint16 read_mem_16(uint32 addr){
	if(addr>=sys_state.mem_size){
		WRITE_DEBUG("Error : read_mem_16 called with out-of-range address");
		return 0;
	}
	return *((uint16*)(sys_state.mem+addr));
}

/**
 * writes 16-bit value from memory
 * @param val value
 * @param addr address
 */
void write_mem_16(uint16 val, uint32 addr){
	if(addr>=sys_state.mem_size){
		WRITE_DEBUG("Error : write_mem_16 called with out-of-range address");
		return 0;
	}
	*((uint16*)(sys_state.mem+addr))=val;
}

/**
 * reads 8-bit value from memory
 * @param addr address
 */
uint8 read_mem_8(uint32 addr){
	if(addr>=sys_state.mem_size){
		WRITE_DEBUG("Error : read_mem_8 called with out-of-range address");
		return 0;
	}
	return *(sys_state.mem+addr);
}

/**
 * writes 8-bit value to memory
 * @param val value
 * @param addr address
 */
void write_mem_16(uint8 val, uint32 addr){
	if(addr>=sys_state.mem_size){
		WRITE_DEBUG("Error : write_mem_8 called with out-of-range address");
		return 0;
	}
	*(sys_state.mem+addr)=val;
}

/** 
 * initializes system
 * allocates sys_state structure
 * @param mem_size size of system memory
 */
void init_system(uint32 mem_size){
	memset(&sys_state, 0, sizeof(sys_state));

	sys_state.mem_size=mem_size;
	sys_state.mem=(uint8*)malloc(mem_size);

	memset(sys_state.mem, 0, mem_size);

	/* set flags */
	SET_RESERVED_FLAGS(sys_state);

	/* initialize registers, default value is 0 */
	sys_state.sp=0x100;	/* default stack pointer */
	sys_state.IOPL=0x0;	/* no IO protection */
	sys_state.NT=0;		/* no nested tasks */
}

int op_unknown(){
	WRITE_DEBUG("Error : Unknown opcode encountered");
	return 0;
}

/* opcode function table */

op_func op_table[256]={
	&op_0x00, &op_0x01, &op_0x02, &op_0x03, &op_0x04, &op_0x05, &op_0x06, &op_0x07, &op_0x08, &op_0x09, &op_0x0a, &op_0x0b, &op_0x0c, &op_0x0d, &op_0x0e, &op_0x0f, 
	&op_0x01, &op_0x03, &op_0x05, &op_0x07, &op_0x09, &op_0x0b, &op_0x0d, &op_0x0f, &op_0x11, &op_0x13, &op_0x15, &op_0x17, &op_0x19, &op_0x1b, &op_0x1d, &op_0x1f, 
	&op_0x02, &op_0x05, &op_0x08, &op_0x0b, &op_0x0e, &op_0x11, &op_0x14, &op_0x17, &op_0x1a, &op_0x1d, &op_0x20, &op_0x23, &op_0x26, &op_0x29, &op_0x2c, &op_0x2f, 
	&op_0x03, &op_0x07, &op_0x0b, &op_0x0f, &op_0x13, &op_0x17, &op_0x1b, &op_0x1f, &op_0x23, &op_0x27, &op_0x2b, &op_0x2f, &op_0x33, &op_0x37, &op_0x3b, &op_0x3f, 
	&op_0x04, &op_0x09, &op_0x0e, &op_0x13, &op_0x18, &op_0x1d, &op_0x22, &op_0x27, &op_0x2c, &op_0x31, &op_0x36, &op_0x3b, &op_0x40, &op_0x45, &op_0x4a, &op_0x4f, 
	&op_0x05, &op_0x0b, &op_0x11, &op_0x17, &op_0x1d, &op_0x23, &op_0x29, &op_0x2f, &op_0x35, &op_0x3b, &op_0x41, &op_0x47, &op_0x4d, &op_0x53, &op_0x59, &op_0x5f, 
	&op_0x06, &op_0x0d, &op_0x14, &op_0x1b, &op_0x22, &op_0x29, &op_0x30, &op_0x37, &op_0x3e, &op_0x45, &op_0x4c, &op_0x53, &op_0x5a, &op_0x61, &op_0x68, &op_0x6f, 
	&op_0x07, &op_0x0f, &op_0x17, &op_0x1f, &op_0x27, &op_0x2f, &op_0x37, &op_0x3f, &op_0x47, &op_0x4f, &op_0x57, &op_0x5f, &op_0x67, &op_0x6f, &op_0x77, &op_0x7f, 
	&op_0x08, &op_0x11, &op_0x1a, &op_0x23, &op_0x2c, &op_0x35, &op_0x3e, &op_0x47, &op_0x50, &op_0x59, &op_0x62, &op_0x6b, &op_0x74, &op_0x7d, &op_0x86, &op_0x8f, 
	&op_0x09, &op_0x13, &op_0x1d, &op_0x27, &op_0x31, &op_0x3b, &op_0x45, &op_0x4f, &op_0x59, &op_0x63, &op_0x6d, &op_0x77, &op_0x81, &op_0x8b, &op_0x95, &op_0x9f, 
	&op_0x0a, &op_0x15, &op_0x20, &op_0x2b, &op_0x36, &op_0x41, &op_0x4c, &op_0x57, &op_0x62, &op_0x6d, &op_0x78, &op_0x83, &op_0x8e, &op_0x99, &op_0xa4, &op_0xaf, 
	&op_0x0b, &op_0x17, &op_0x23, &op_0x2f, &op_0x3b, &op_0x47, &op_0x53, &op_0x5f, &op_0x6b, &op_0x77, &op_0x83, &op_0x8f, &op_0x9b, &op_0xa7, &op_0xb3, &op_0xbf, 
	&op_0x0c, &op_0x19, &op_0x26, &op_0x33, &op_0x40, &op_0x4d, &op_0x5a, &op_0x67, &op_0x74, &op_0x81, &op_0x8e, &op_0x9b, &op_0xa8, &op_0xb5, &op_0xc2, &op_0xcf, 
	&op_0x0d, &op_0x1b, &op_0x29, &op_0x37, &op_0x45, &op_0x53, &op_0x61, &op_0x6f, &op_0x7d, &op_0x8b, &op_0x99, &op_0xa7, &op_0xb5, &op_0xc3, &op_0xd1, &op_0xdf, 
	&op_0x0e, &op_0x1d, &op_0x2c, &op_0x3b, &op_0x4a, &op_0x59, &op_0x68, &op_0x77, &op_0x86, &op_0x95, &op_0xa4, &op_0xb3, &op_0xc2, &op_0xd1, &op_0xe0, &op_0xef, 
	&op_0x0f, &op_0x1f, &op_0x2f, &op_0x3f, &op_0x4f, &op_0x5f, &op_0x6f, &op_0x7f, &op_0x8f, &op_0x9f, &op_0xaf, &op_0xbf, &op_0xcf, &op_0xdf, &op_0xef, &op_0xff
};