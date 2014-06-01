/**
 * Hard86 Virtual Devices
 * Virtual Device DLL Include File
 */

#ifndef HARD86_VDEV_H
#define HARD86_VDEV_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

typedef unsigned char uint8;
typedef unsigned long uint32;
typedef unsigned short uint16;

typedef HANDLE MUTEX;

#define DLL_EXPORT __declspec(dllexport)

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

	/* information for the emulator */
	int rep_cond;	// 1 - keep repeating, 0 - stop

}op_data_type, *op_data_ptr;

typedef struct{
	/* system memory */
	uint8* mem;
	uint32 mem_size;

	/* I/O bus */
	/*	The I/O address space consists of 2 16 (64K)
	individually addressable 8-bit I/O ports,
	numbered 0 through FFFFH
	*/
	uint8* io_bus;
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
	uint32 r_ip;	/*	this is the 'real' ip value
					that is, not segmented	*/

	/*	flags register
	format :
	Bit# : 15	14	13/12	11	10	9	8	7	6	5	4	3	2	1	0
	Flag : 0	NT	IOPL	OF	DF	IF	TF	SF	ZF	0	AF	0	PF	1	CF
	*/
	union{
		struct{
			uint8 CF : 1;
			uint8 reserved_1 : 1;	/* always 1 */
			uint8 PF : 1;
			uint8 reserved_2 : 1; /* always 0 */
			uint8 AF : 1;
			uint8 reserved_3 : 1;	/* always 0 */
			uint8 ZF : 1;
			uint8 SF : 1;
			uint8 TF : 1;
			uint8 IF : 1;
			uint8 DF : 1;
			uint8 OF : 1;
			uint8 IOPL : 2;
			uint8 NT : 1;
			uint8 reserved_4 : 1;	/* 1 on 8086, 0 above */
		}f_bits;
		uint16 flags;
	};

	// Emulator specific data (non-8086 related ... kinda)
	MUTEX sys_mutex;

	/*
		* True if we want the debugger callbacks to be executed
		* for each opcode while we are in an external interrupt.
		* False if we want external interrupts to be 'invisible'
		* to the rest of the program
		*/
	int step_through_extern_int;
	int is_in_extern_int;	// in external interrupt?

	op_data_ptr op_data;

}sys_state_type, *sys_state_ptr;

#define DEFAULT_SP 0xfffe

#define GET_ADDR(address, segment)\
	(((uint32)segment)*0x10)+address

/* register access */

#define REG_16(rg) *((uint16*)sys_state->rg)
#define REG_8_LO(rg) *((uint8*)sys_state->rg+0)	/* assuming little endian host machine */
#define REG_8_HI(rg) *((uint8*)sys_state->rg+1)

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

#define IP sys_state->ip
#define CS sys_state->cs
#define SS sys_state->ss
#define DS sys_state->ds
#define FS sys_state->fs
#define ES sys_state->es
#define FLAGS sys_state->flags

#define SP sys_state->sp
#define BP sys_state->bp
#define SI sys_state->si
#define DI sys_state->di

#define FLAG_DF	sys_state->f_bits.DF
#define FLAG_IF	sys_state->f_bits.IF
#define FLAG_TF	sys_state->f_bits.TF
#define FLAG_CF	sys_state->f_bits.CF
#define FLAG_PF	sys_state->f_bits.PF
#define FLAG_AF	sys_state->f_bits.AF
#define FLAG_ZF	sys_state->f_bits.ZF
#define FLAG_SF	sys_state->f_bits.SF
#define FLAG_OF	sys_state->f_bits.OF

DLL_EXPORT int VirtualDevice_AcceptEmulationMutex(MUTEX a, sys_state_ptr b);
DLL_EXPORT int VirtualDevice_Terminate();
DLL_EXPORT int VirtualDevice_Initialize(void* a, void* b);

// this should get put in a .cpp file later

/**
 * reads 16-bit value from memory
 * @param addr address
 */
static uint16 read_mem_16(sys_state_ptr sys_state, uint32 addr){
	if(addr>=sys_state->mem_size){
		printf("Error : read_mem_16 called with out-of-range address");
		return 0;
	}
	return *((uint16*)(sys_state->mem+addr));
}

/**
 * writes 16-bit value from memory
 * @param val value
 * @param addr address
 */
static void write_mem_16(sys_state_ptr sys_state, uint16 val, uint32 addr){
	if(addr>=sys_state->mem_size){
		printf("Error : write_mem_16 called with out-of-range address");
	}
	*((uint16*)(sys_state->mem+addr))=val;
}

/**
 * reads 8-bit value from memory
 * @param addr address
 */
static uint8 read_mem_8(sys_state_ptr sys_state, uint32 addr){
	if(addr>=sys_state->mem_size){
		printf("Error : read_mem_8 called with out-of-range address");
		return 0;
	}
	return *(sys_state->mem+addr);
}

/**
 * writes 8-bit value to memory
 * @param val value
 * @param addr address
 */
static void write_mem_8(sys_state_ptr sys_state, uint8 val, uint32 addr){
	if(addr>=sys_state->mem_size){
		printf("Error : write_mem_8 called with out-of-range address");
	}
	*(sys_state->mem+addr)=val;
}

/* Stack operations */
static void stack_push(sys_state_ptr sys_state, uint16 val){
	sys_state->sp-=2;
	write_mem_16(sys_state, val, GET_ADDR(sys_state->sp, sys_state->ss));
}

static uint16 stack_pop(sys_state_ptr sys_state){
	uint16 val=read_mem_16(sys_state, GET_ADDR(sys_state->sp, sys_state->ss));
	sys_state->sp+=2;
	return val;
}

static int stack_empty(sys_state_ptr sys_state){
	if(sys_state->sp==DEFAULT_SP){
		return 1;
	}
	return 0;
}

/* I/O port access */

/**
 * reads from I/O port
 */
static uint8 read_io_port(sys_state_ptr sys_state, uint16 port){
	return *(sys_state->io_bus+port);
}

/**
 * writes to I/O port
 */
static void write_io_port(sys_state_ptr sys_state, uint8 val, uint16 port){
	*(sys_state->io_bus+port)=val;
}
#include <windows.h>

static void make_int_call(sys_state_ptr sys_state, uint8 inum){
	sys_state->is_in_extern_int++;
	uint16 new_cs, new_ip;
	uint16 mem_offset=inum<<2;
	new_cs=read_mem_16(sys_state, mem_offset);
	new_ip=read_mem_16(sys_state, mem_offset+2);
	stack_push(sys_state, FLAGS);
	stack_push(sys_state, CS);
	stack_push(sys_state, IP);
	FLAG_IF=0;
	CS=new_cs;
	IP=new_ip;
}

#undef DEFAULT_SP

#ifdef __cplusplus
}
#endif


#endif