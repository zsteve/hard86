/*  Hard86 - An 8086 Emulator with support for virtual hardware
	
    Copyright (C) 2014 Stephen Zhang

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.	
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <wchar.h>
#include <varargs.h>
#include <assert.h>

#include "emulator_engine.h"
#include "emulator_engine_interface.h"
#include "opcodes.h"
#include "flags.h"
#include "../../../global/defines.h"
#include "../../../global/typedefs.h"

#include "dasm/dasm.h"

/* data structures */
#include "../../../system/datastruct/clist/clist.h"

/* system multithreading */
#include "../../../system/multithreading/mutex/c/cmutex.h"
#include "../../../system/multithreading/thread/c/thread.h"

/* forward declaration of op_table */
op_func op_table[256];

/* textual representation of registers */
char text_regs[22][6]={
	"AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI", "ES", "CS", "SS", "DS", "IP", "FLAGS",
	"AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH"};

/* system state */
sys_state_type sys_state;

op_data_type op_data;

extern int op_prefix_size;

DBGCALLBACK breakpoint_hit_callback;
DBGCALLBACK pre_execute_callback;
DBGCALLBACK post_execute_callback;
DBGCALLBACK user_input_callback;

/* default register values */
#define DEFAULT_CS 0x700
#define DEFAULT_IP 0x100
#define DEFAULT_SS 0x700
#define DEFAULT_SP 0xFFFE
#define DEFAULT_DS 0x700
#define DEFAULT_ES 0x700

static int halt_flag=0;

#define SET_RESERVED_FLAGS(state)\
	state.f_bits.reserved_1=state.f_bits.reserved_4=1;\
	state.f_bits.reserved_2=state.f_bits.reserved_3=0;

/* debug function */

/*static*/ void write_debug(char* file, int line, char* str){
	FILE* f=fopen("debug.log", "a");
	char s[1024]="";
	sprintf(s, "%s %d : %s\n", file, line, str);
	fprintf(f, "%s", s);
	fclose(f);
}

void (*out_opinfo_ptr)(char* str, va_list* v)=0;

void out_opinfo(char* str, ...){
	va_list v;
	va_start(v, str);
	if(!out_opinfo_ptr){
		vprintf(str, v);
		printf("\n");
	}
	else{
		out_opinfo_ptr(str, &v);
	}
	va_end(v);
}

/* register access */
void write_reg(int reg, uint16 val){
	switch(reg){
	case REG_AX:
		AX=val;
		break;
	case REG_CX:
		CX=val;
		break;
	case REG_DX:
		DX=val;
		break;
	case REG_BX:
		BX=val;
		break;
	case REG_SP:
		SP=val;
		break;
	case REG_BP:
		BP=val;
		break;
	case REG_SI:
		SI=val;
		break;
	case REG_DI:
		DI=val;
		break;
	case REG_ES:
		ES=val;
		break;
	case REG_CS:
		CS=val;
		break;
	case REG_SS:
		SS=val;
		break;
	case REG_DS:
		DS=val;
		break;
	case REG_IP:
		IP=val;
		break;
	case REG_FLAGS:
		FLAGS=val;
		break;
	case REG_AH:
		AH=(val & 0xff);
		break;
	case REG_AL:
		AL=(val & 0xff);
		break;
	case REG_CH:
		CH=(val & 0xff);
		break;
	case REG_CL:
		CL=(val & 0xff);
		break;
	case REG_DH:
		DH=(val & 0xff);
		break;
	case REG_DL:
		DL=(val & 0xff);
		break;
	case REG_BH:
		BH=(val & 0xff);
		break;
	case REG_BL:
		BL=(val & 0xff);
		break;
	}
}

uint16 read_reg(int reg){
	switch(reg){
	case REG_AX:
		return AX;
		break;
	case REG_CX:
		return CX;
		break;
	case REG_DX:
		return DX;
		break;
	case REG_BX:
		return BX;
		break;
	case REG_SP:
		return SP;
		break;
	case REG_BP:
		return BP;
		break;
	case REG_SI:
		return SI;
		break;
	case REG_DI:
		return DI;
		break;
	case REG_ES:
		return ES;
		break;
	case REG_CS:
		return CS;
		break;
	case REG_SS:
		return SS;
		break;
	case REG_DS:
		return DS;
		break;
	case REG_IP:
		return IP;
		break;
	case REG_FLAGS:
		return FLAGS;
		break;
	case REG_AH:
		return (uint16)AH;
		break;
	case REG_AL:
		return (uint16)AL;
		break;
	case REG_CH:
		return (uint16)CH;
		break;
	case REG_CL:
		return (uint16)CL;
		break;
	case REG_DH:
		return (uint16)DH;
		break;
	case REG_DL:
		return (uint16)DL;
		break;
	case REG_BH:
		return (uint16)BH;
		break;
	case REG_BL:
		return (uint16)BL;
		break;
	}
}

/* memory access functions */

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
void write_mem_8(uint8 val, uint32 addr){
	if(addr>=sys_state.mem_size){
		WRITE_DEBUG("Error : write_mem_8 called with out-of-range address");
	}
	*(sys_state.mem+addr)=val;
}

/* Stack operations */
void stack_push(uint16 val){
	sys_state.sp-=2;
	write_mem_16(val, GET_ADDR(sys_state.sp, sys_state.ss));
}

uint16 stack_pop(){
	uint16 val=read_mem_16(GET_ADDR(sys_state.sp, sys_state.ss));
	sys_state.sp+=2;
	return val;
}

int stack_empty(){
	if(sys_state.sp==DEFAULT_SP){
		return 1;
	}
	return 0;
}

/* I/O port access */

/**
 * reads from I/O port
 */
uint8 read_io_port(uint16 port){
	return *(sys_state.io_bus+port);
}

/**
 * writes to I/O port
 */
void write_io_port(uint8 val, uint16 port){
	*(sys_state.io_bus+port)=val;
}

/**
 * Makes external interrupt
 */

// define int_call() from opcodes.c as an extern func
extern void int_call(uint8 inum);

void extern_int(uint8 inum){
	sys_state.is_in_extern_int++;	// we are now in an external interrupt.
						// we increment in order to allow for 
						// nested external interrupts (possibly redundant, but added jic)
	int_call(inum);
}

void set_step_through_extern_int(int v){
	sys_state.step_through_extern_int=v;
}

int* get_is_in_extern_int(){ return &sys_state.is_in_extern_int; }

void set_sys_mutex(MUTEX mutex){
	sys_state.sys_mutex=mutex;
}

void set_step_through_int(int v){
	sys_state.step_through_int=v;
}

int* get_is_in_int(){ return &sys_state.is_in_int; }

void system_load_mem(uint8* data, uint32 size){
	int i;
	uint32 o=DS<<4;
	o+=0x100;
	for(i=0; i<size; i++){
		*(sys_state.mem+i+o)=*(data+i);
	}
}

void system_load_bios(uint8* data, uint16 size){
	/* BIOS gets loaded at base 0x400 */
	/*	Last 512 bytes of BIOS are interrupt offsets into the BIOS.
		These get written to 0x400 instead
	*/
	uint16 int_offset=size-512;
	int i;
	assert(size>=512);
	// copy IDT
	for(i=0; i<256; i++){
		/*	IDT format :
			SEG:IP
		*/
		((uint16*)sys_state.mem)[(i*2)]=BIOS_MEM_SEG;
		((uint16*)sys_state.mem)[(i*2)+1]=((uint16*)(data+int_offset))[i];
	}
	// copy rest of BIOS
	for(i=0; i<int_offset; i++){
		sys_state.mem[i+0x400]=data[i];
	}
}

/** 
 * initializes system
 * allocates sys_state structure
 * @param mem_size size of system memory
 */
int system_init(MUTEX sys_mutex_,
				DBGCALLBACK bp_hit_func,
				DBGCALLBACK pre_ex_func,
				DBGCALLBACK post_ex_func,
				DBGCALLBACK user_in_func){

	breakpoint_hit_callback=bp_hit_func;
	pre_execute_callback=pre_ex_func;
	post_execute_callback=post_ex_func;
	user_input_callback=user_in_func;

	memset(&sys_state, 0, sizeof(sys_state));

	sys_state.sys_mutex=sys_mutex_;	/* system MUTEX */
	sys_state.op_data=&op_data;
	sys_state.mem_size=MEM_SIZE;

	if(!(sys_state.mem=(uint8*)malloc(MEM_SIZE))){
		printf("Error : failed to allocate sys_state.mem\n");
		abort();
	}

	memset(sys_state.mem, 0, MEM_SIZE);
	
	if(!(sys_state.io_bus=(uint8*)malloc(IO_BUS_SIZE))){
		printf("Error : failed to allocate sys_state.io_bus\n");
		abort();
	}
	
	memset(sys_state.io_bus, 0, IO_BUS_SIZE);

	/* set flags */
	SET_RESERVED_FLAGS(sys_state);

	/* initialize registers, default value is 0 */

	CS=DEFAULT_CS;
	IP=DEFAULT_IP;
	SS=DEFAULT_SS;
	SP=DEFAULT_SP;
	DS=DEFAULT_DS;
	ES=DEFAULT_ES;
	sys_state.f_bits.IOPL=0x0;	/* no IO protection */
	sys_state.f_bits.NT=0;		/* no nested tasks */

	sys_state.is_in_extern_int=0;
	sys_state.step_through_extern_int=0;

	sys_state.is_in_int=0;
	sys_state.step_through_int=0;
	return 0;
}

int system_destroy(){
	if(sys_state.mem)
		free(sys_state.mem);
	if(sys_state.io_bus)
		free(sys_state.io_bus);
	halt_flag=0;
	return 0;
}

void system_halt(){
	halt_flag=1;
}

sys_state_ptr get_system_state(){
	return &sys_state;
}

void system_print_state(){
	printf("----------------------------------------\n");
	printf("AX: %X\tBX: %X\tCX: %X\tDX: %X\n"\
		"CS: %X\tSS: %X\tDS: %X\tES: %X\n"\
		"IP: %X\t\n", \
		AX, BX, CX, DX, CS, SS, DS, ES, IP);
	printf("\n");
	printf("O : %d\tD: %d\tI: %d\tT: %d\tS: %d\t Z: %d\tA: %d\t P: %d\tC: %d\n",
		FLAG_OF, FLAG_DF, FLAG_IF, FLAG_TF, FLAG_SF, FLAG_ZF, FLAG_AF, FLAG_PF, FLAG_CF);
	printf("----------------------------------------\n");
	//getch();
}

void reset_op_data(){
	op_data.rep=op_data.repne=0;
	op_data.seg_ovr=0;
}

int system_execute(){
	int counter=0;
	extern int halt_flag;


	while(!halt_flag){
		counter++;

		/* do not execute this if we are in an external interrupt
			and step_through_extern_int flag is clear
		*/
		if((!sys_state.is_in_extern_int || (sys_state.is_in_extern_int && sys_state.step_through_extern_int))
			&& (!sys_state.is_in_int || (sys_state.is_in_int && sys_state.step_through_int)))
			post_execute_callback(sys_state.sys_mutex, &sys_state);

		if((!sys_state.is_in_extern_int || (sys_state.is_in_extern_int && sys_state.step_through_extern_int)))
			mutex_lock(sys_state.sys_mutex);

		/*	we make this callback to allow for user input to alter the system state
		i.e - changing a register value, editing memory, etc
		*/
		if((!sys_state.is_in_extern_int || (sys_state.is_in_extern_int && sys_state.step_through_extern_int))
			&& (!sys_state.is_in_int || (sys_state.is_in_int && sys_state.step_through_int))
			&& counter)
			user_input_callback(sys_state.sys_mutex, &sys_state);

		R_IP=GET_ADDR(IP, CS);

		if(read_mem_8(GET_ADDR(IP, CS))==0xcc){
			/* int3 debug interrupt - hand control to debugger */
			mutex_unlock(sys_state.sys_mutex);
			breakpoint_hit_callback(sys_state.sys_mutex, &sys_state);
			continue;
		}

		//system_print_state();

		op_data.rep_cond=1;

		if(op_data.rep || op_data.repne){
			while(op_data.rep_cond){
				uint16 old_ip=IP;
				R_IP=GET_ADDR(IP, CS);
				(*op_table[read_mem_8(GET_ADDR(IP, CS))])();
				if(op_data.rep_cond)
					IP=old_ip;
			}
			// reset rep/repne
			reset_op_data();
		}
		else{
			(*op_table[read_mem_8(GET_ADDR(IP, CS))])();
		}

		if((!sys_state.is_in_extern_int || (sys_state.is_in_extern_int && sys_state.step_through_extern_int)))
			mutex_unlock(sys_state.sys_mutex);

		R_IP=GET_ADDR(IP, CS);
		process_instr_prefixes();

		if((!sys_state.is_in_extern_int || (sys_state.is_in_extern_int && sys_state.step_through_extern_int))
			&& (!sys_state.is_in_int || (sys_state.is_in_int && sys_state.step_through_int)))
			pre_execute_callback(sys_state.sys_mutex, &sys_state);

	}
	post_execute_callback(sys_state.sys_mutex, &sys_state);
	halt_flag=0;
	return 0;
}

static int op_unknown(){
	WRITE_DEBUG("Error : Unknown opcode encountered");
	printf("Error : Unknown opcode\n");
	out_opinfo("???");
	IP++;
	return 0;
}

/* opcode function table */

op_func op_table[256]={
	&op_0x00,	&op_0x01,	&op_0x02,	&op_0x03,	&op_0x04,	&op_0x05,	&op_0x06,	&op_0x07,	&op_0x08,	&op_0x09,	&op_0x0a,	&op_0x0b,	&op_0x0c,	&op_0x0d,	&op_0x0e,	&op_0x0f,	
	&op_0x10,	&op_0x11,	&op_0x12,	&op_0x13,	&op_0x14,	&op_0x15,	&op_0x16,	&op_0x17,	&op_0x18,	&op_0x19,	&op_0x1a,	&op_0x1b,	&op_0x1c,	&op_0x1d,	&op_0x1e,	&op_0x1f,	
	&op_0x20,	&op_0x21,	&op_0x22,	&op_0x23,	&op_0x24,	&op_0x25,	&op_0x26,	&op_0x27,	&op_0x28,	&op_0x29,	&op_0x2a,	&op_0x2b,	&op_0x2c,	&op_0x2d,	&op_0x2e,	&op_0x2f,	
	&op_0x30,	&op_0x31,	&op_0x32,	&op_0x33,	&op_0x34,	&op_0x35,	&op_0x36,	&op_0x37,	&op_0x38,	&op_0x39,	&op_0x3a,	&op_0x3b,	&op_0x3c,	&op_0x3d,	&op_0x3e,	&op_0x3f,	
	&op_0x40,	&op_0x41,	&op_0x42,	&op_0x43,	&op_0x44,	&op_0x45,	&op_0x46,	&op_0x47,	&op_0x48,	&op_0x49,	&op_0x4a,	&op_0x4b,	&op_0x4c,	&op_0x4d,	&op_0x4e,	&op_0x4f,	
	&op_0x50,	&op_0x51,	&op_0x52,	&op_0x53,	&op_0x54,	&op_0x55,	&op_0x56,	&op_0x57,	&op_0x58,	&op_0x59,	&op_0x5a,	&op_0x5b,	&op_0x5c,	&op_0x5d,	&op_0x5e,	&op_0x5f,	
	&op_0x60,	&op_0x61,	&op_0x62,	&op_0x63,	&op_0x64,	&op_0x65,	&op_0x66,	&op_0x67,	&op_0x68,	&op_0x69,	&op_0x6a,	&op_0x6b,	&op_0x6c,	&op_0x6d,	&op_0x6e,	&op_0x6f,	
	&op_0x70,	&op_0x71,	&op_0x72,	&op_0x73,	&op_0x74,	&op_0x75,	&op_0x76,	&op_0x77,	&op_0x78,	&op_0x79,	&op_0x7a,	&op_0x7b,	&op_0x7c,	&op_0x7d,	&op_0x7e,	&op_0x7f,	
	&op_0x80,	&op_0x81,	&op_0x82,	&op_0x83,	&op_0x84,	&op_0x85,	&op_0x86,	&op_0x87,	&op_0x88,	&op_0x89,	&op_0x8a,	&op_0x8b,	&op_0x8c,	&op_0x8d,	&op_0x8e,	&op_0x8f,	
	&op_0x90,	&op_0x91,	&op_0x92,	&op_0x93,	&op_0x94,	&op_0x95,	&op_0x96,	&op_0x97,	&op_0x98,	&op_0x99,	&op_0x9a,	&op_0x9b,	&op_0x9c,	&op_0x9d,	&op_0x9e,	&op_0x9f,	
	&op_0xa0,	&op_0xa1,	&op_0xa2,	&op_0xa3,	&op_0xa4,	&op_0xa5,	&op_0xa6,	&op_0xa7,	&op_0xa8,	&op_0xa9,	&op_0xaa,	&op_0xab,	&op_0xac,	&op_0xad,	&op_0xae,	&op_0xaf,	
	&op_0xb0,	&op_0xb1,	&op_0xb2,	&op_0xb3,	&op_0xb4,	&op_0xb5,	&op_0xb6,	&op_0xb7,	&op_0xb8,	&op_0xb9,	&op_0xba,	&op_0xbb,	&op_0xbc,	&op_0xbd,	&op_0xbe,	&op_0xbf,	
	&op_0xc0,	&op_0xc1,	&op_0xc2,	&op_0xc3,	&op_0xc4,	&op_0xc5,	&op_0xc6,	&op_0xc7,	&op_0xc8,	&op_0xc9,	&op_0xca,	&op_0xcb,	&op_0xcc,	&op_0xcd,	&op_0xce,	&op_0xcf,	
	&op_0xd0,	&op_0xd1,	&op_0xd2,	&op_0xd3,	&op_0xd4,	&op_0xd5,	&op_0xd6,	&op_0xd7,	&op_0xd8,	&op_0xd9,	&op_0xda,	&op_0xdb,	&op_0xdc,	&op_0xdd,	&op_0xde,	&op_0xdf,	
	&op_0xe0,	&op_0xe1,	&op_0xe2,	&op_0xe3,	&op_0xe4,	&op_0xe5,	&op_0xe6,	&op_0xe7,	&op_0xe8,	&op_0xe9,	&op_0xea,	&op_0xeb,	&op_0xec,	&op_0xed,	&op_0xee,	&op_0xef,	
	&op_0xf0,	&op_0xf1,	&op_0xf2,	&op_0xf3,	&op_0xf4,	&op_0xf5,	&op_0xf6,	&op_0xf7,	&op_0xf8,	&op_0xf9,	&op_0xfa,	&op_0xfb,	&op_0xfc,	&op_0xfd,	&op_0xfe,	&op_0xff,
};