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

/*	IMPORTANT INFORMATION FOR WRITING OPCODE IMPLEMENTATIONS

	General:
		Use M_REG instead of op_data.reg UNLESS it is an OP+ (register operand
		is in lower 3 bits of instruction. In this case, use M_RM

	If the opcode contains a mod-reg-rm byte:
		a) Read the rm byte data first
		b) Read any immediate operands like so:
			RMEMX(R_IP+<offset>+disp_size), thus allowing for the varying displacement sizes
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "opcodes.h"
#include "emulator_engine.h"
#include "emulator_engine_interface.h"
#include "flags.h"
#include "global.h"

#define SHOW_DEBUG

extern op_data_type op_data;
extern sys_state_type sys_state;
extern char text_regs[22][6];

int execute_flag=1;	/*	execute flag :
	if true, opcodes will actually be executed.
	otherwise, only disassembly will be printed
	*/

/* physical address memory access */

#define RMEM8(addr)\
	read_mem_8(addr)

#define WMEM8(val, addr)\
	write_mem_8(val, addr)

#define RMEM16(addr)\
	read_mem_16(addr)

/* segmented memory access */

#define SRMEM8(addr, seg)\
	read_mem_8(GET_ADDR(addr, seg))

#define SRMEM16(addr, seg)\
	read_mem_16(GET_ADDR(addr, seg))

#define SWMEM8(val, addr, seg)\
	write_mem_8(val, GET_ADDR(addr, seg))

#define SWMEM16(val, addr, seg)\
	write_mem_16(val, GET_ADDR(addr, seg))

#define RREG8(reg)\
	read_reg(BREG(reg))

#define RREG16(reg)\
	read_reg(reg)

#define WREG8(reg, val)\
	write_reg(BREG(reg), val)

#define WREG16(reg, val)\
	write_reg(reg, val)

#define WSREG(reg, val)\
	write_reg(SREG(reg), val)

#define RSREG(reg)\
	read_reg(SREG(reg))

/* read relative to IP */
#define RRELIP8(v)\
	read_mem_8(sys_state.r_ip+v)

#define RRELIP16(v)\
	read_mem_16(sys_state.r_ip+v)

/* text register accessing */
#define TEXTREG8(v)\
	text_regs[BREG(v)]

#define TEXTREG16(v)\
	text_regs[v]

#define TEXTSREG(v)\
	text_regs[SREG(v)]

static uint8 disp_size;	/* size of displacement, can be 0, 1 or 2 */

uint16 get_rm_disp_size(){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	disp_size=0;
	switch(op_data.mod){
		/* modes */
	case 0:
		if(op_data.rm==6){
			disp_size=2;
			return disp_size;
		}
	case 1:
	case 2:
		if(op_data.mod==1) disp_size=1;
		else if(op_data.mod==2) disp_size=2;
		return disp_size;
	case 3:
		/* register addressing */
		return disp_size;
		break;
	}
	return 0;
}

uint8 read_rm_val_8(uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RRELIP16(2);
	uint8 disp8=RRELIP8(2);
	uint32 addr=0;
	disp_size=0;
	switch(op_data.mod){
		/* modes */
	case 0:
		if(op_data.rm==6){
			addr=disp16;
			disp_size=2;
			goto end;
		}
	case 1:
	case 2:
		if(op_data.mod==1) disp_size=1;
		else if(op_data.mod==2) disp_size=2;
		/* two byte signed displacement */
		switch(op_data.rm){
		case 0:	/* BX + SI */
			addr=BX+SI;
			break;
		case 1:	/* BX + DI */
			addr=BX+DI;
			break;
		case 2:	/* BP + SI */
			addr=BP+SI;
			break;
		case 3: /* BP + DI */
			addr=BP+DI;
		case 4:	/* SI */
			addr=SI;
			break;
		case 5: /* DI */
			addr=DI;
			break;
		case 6:	/* BP */
			addr=BP;
			break;
		case 7:	/* BX */
			addr=BX;
			break;
		}
		end:
		addr=GET_ADDR(addr&0xffff, seg);
		return read_mem_8(addr);
	case 3:
		/* register addressing */
		return RREG8(op_data.rm);
		break;
	}
	return 0;
}

uint16 read_rm_val_16(uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RRELIP16(2);
	uint8 disp8=RRELIP8(2);
	uint32 addr=0;
	disp_size=0;
	switch(op_data.mod){
		/* modes */
	case 0:
		if(op_data.rm==6){
			addr=disp16;
			disp_size=2;
			goto end;
		}
	case 1:
	case 2:
		if(op_data.mod==1) disp_size=1;
		else if(op_data.mod==2) disp_size=2;
		/* two byte signed displacement */
		switch(op_data.rm){
		case 0:	/* BX + SI */
			addr=BX+SI;
			break;
		case 1:	/* BX + DI */
			addr=BX+DI;
			break;
		case 2:	/* BP + SI */
			addr=BP+SI;
			break;
		case 3: /* BP + DI */
			addr=BP+DI;
		case 4:	/* SI */
			addr=SI;
			break;
		case 5: /* DI */
			addr=DI;
			break;
		case 6:	/* BP */
			addr=BP;
			break;
		case 7:	/* BX */
			addr=BX;
			break;
		}
		end:
		addr=GET_ADDR(addr&0xffff, seg);
		return read_mem_16(addr);
	case 3:
		/* register addressing */
		return RREG16(op_data.rm);
		break;
	}
	return 0;
}


static char rm_val_str[80]="";

char* print_rm_val_16(uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	char disp_str[32]="";
	uint16 disp16=RRELIP16(2);
	uint8 disp8=RRELIP8(2);
	uint32 addr=0;
	strcpy(rm_val_str, "word ptr [");
	switch(op_data.mod){
		/* modes */
	case 0:
		if(op_data.rm==6){
			/* displacement only mode */
			strcat(rm_val_str, itoa(disp16, disp_str, 16));
			strcat(rm_val_str, "]");
			return rm_val_str;
		}
	case 1:
	case 2:
		/* two byte signed displacement */
		switch(op_data.rm){
		case 0:	/* BX + SI */
			strcat(rm_val_str, "BX+SI");
			break;
		case 1:	/* BX + DI */
			strcat(rm_val_str, "BX+DI");
			break;
		case 2:	/* BP + SI */
			strcat(rm_val_str, "BP+SI");
			break;
		case 3: /* BP + DI */
			strcat(rm_val_str, "BP+DI");
		case 4:	/* SI */
			strcat(rm_val_str, "SI");
			break;
		case 5: /* DI */
			strcat(rm_val_str, "DI");
			break;
		case 6:	/* BP */
			strcat(rm_val_str, "BP");
			break;
		case 7:	/* BX */
			strcat(rm_val_str, "BX");
			break;
		}
		if(op_data.rm!=7)
			strcat(rm_val_str, "] + ");
		else
			strcat(rm_val_str, "]");
		if(op_data.mod==1){
			strcat(rm_val_str, itoa(disp8, disp_str, 16));
		}else if(op_data.mod==2){
			strcat(rm_val_str, itoa(disp16, disp_str, 16));
		}
		return rm_val_str;
	case 3:
		/* register addressing */
		return text_regs[op_data.rm];
	}
	return "";
}

char* print_rm_val_8(uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	char disp_str[32]="";
	uint16 disp16=RRELIP16(2);
	uint8 disp8=RRELIP8(2);
	uint32 addr=0;
	strcpy(rm_val_str, "byte ptr [");
	switch(op_data.mod){
		/* modes */
	case 0:
		if(op_data.rm==6){
			/* displacement only mode */
			strcat(rm_val_str, itoa(disp16, disp_str, 16));
			strcat(rm_val_str, "]");
			return rm_val_str;
		}
	case 1:
	case 2:
		/* two byte signed displacement */
		switch(op_data.rm){
		case 0:	/* BX + SI */
			strcat(rm_val_str, "BX+SI");
			break;
		case 1:	/* BX + DI */
			strcat(rm_val_str, "BX+DI");
			break;
		case 2:	/* BP + SI */
			strcat(rm_val_str, "BP+SI");
			break;
		case 3: /* BP + DI */
			strcat(rm_val_str, "BP+DI");
		case 4:	/* SI */
			strcat(rm_val_str, "SI");
			break;
		case 5: /* DI */
			strcat(rm_val_str, "DI");
			break;
		case 6:	/* BP */
			strcat(rm_val_str, "BP");
			break;
		case 7:	/* BX */
			strcat(rm_val_str, "BX");
			break;
		}
		if(op_data.rm!=7)
			strcat(rm_val_str, "] + ");
		else
			strcat(rm_val_str, "]");

		if(op_data.mod==1){
			strcat(rm_val_str, itoa(disp8, disp_str, 16));
		}
		else if(op_data.mod==2){
			strcat(rm_val_str, itoa(disp16, disp_str, 16));
		}
		return rm_val_str;
	case 3:
		/* register addressing */
		return text_regs[BREG(op_data.rm)];
	}
	return "";
}

int write_rm_val_8(uint8 val, uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RRELIP16(2);
	uint8 disp8=RRELIP8(2);
	uint32 addr=0;
	disp_size=0;
	switch(op_data.mod){
		/* modes */
	case 0:
		if(op_data.rm==6){
			addr=disp16;
			disp_size=2;
			goto end;
		}
	case 1:
	case 2:
		if(op_data.mod==1) disp_size=1;
		else if(op_data.mod==2) disp_size=2;
		/* two byte signed displacement */
		switch(op_data.rm){
		case 0:	/* BX + SI */
			addr=BX+SI;
			break;
		case 1:	/* BX + DI */
			addr=BX+DI;
			break;
		case 2:	/* BP + SI */
			addr=BP+SI;
			break;
		case 3: /* BP + DI */
			addr=BP+DI;
		case 4:	/* SI */
			addr=SI;
			break;
		case 5: /* DI */
			addr=DI;
			break;
		case 6:	/* BP */
			addr=BP;
			break;
		case 7:	/* BX */
			addr=BX;
			break;
		}
		end:
		addr=GET_ADDR(addr, seg);
		write_mem_8(val, addr);
		return;
	case 3:
		/* register addressing */
		write_reg(BREG(op_data.rm), val);
		break;
	}
	return;
}

int write_rm_val_16(uint16 val, uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RRELIP16(2);
	uint8 disp8=RRELIP8(2);
	uint32 addr=0;
	disp_size=0;
	switch(op_data.mod){
		/* modes */
	case 0:
		if(op_data.rm==6){
			addr=disp16;
			disp_size=2;
			goto end;
		}
	case 1:
	case 2:
		if(op_data.mod==1) disp_size=1;
		else if(op_data.mod==2) disp_size=2;
		/* two byte signed displacement */
		switch(op_data.rm){
		case 0:	/* BX + SI */
			addr=BX+SI;
			break;
		case 1:	/* BX + DI */
			addr=BX+DI;
			break;
		case 2:	/* BP + SI */
			addr=BP+SI;
			break;
		case 3: /* BP + DI */
			addr=BP+DI;
		case 4:	/* SI */
			addr=SI;
			break;
		case 5: /* DI */
			addr=DI;
			break;
		case 6:	/* BP */
			addr=BP;
			break;
		case 7:	/* BX */
			addr=BX;
			break;
		}
		end:
		addr=GET_ADDR(addr&0xffff, seg);
		write_mem_16(val, addr);
		return;
	case 3:
		/* register addressing */
		write_reg((op_data.rm), val);
		break;
	}
	return;
}

uint16 get_rm_addr(uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RRELIP16(2);
	uint8 disp8=RRELIP8(2);
	uint32 addr=0;
	switch(op_data.mod){
		/* modes */
	case 0:
		if(op_data.rm==6){
			addr=disp16;
			goto end;
		}
	case 1:
	case 2:
		/* two byte signed displacement */
		switch(op_data.rm){
		case 0:	/* BX + SI */
			addr=BX+SI;
			break;
		case 1:	/* BX + DI */
			addr=BX+DI;
			break;
		case 2:	/* BP + SI */
			addr=BP+SI;
			break;
		case 3: /* BP + DI */
			addr=BP+DI;
		case 4:	/* SI */
			addr=SI;
			break;
		case 5: /* DI */
			addr=DI;
			break;
		case 6:	/* BP */
			addr=BP;
			break;
		case 7:	/* BX */
			addr=BX;
			break;
		}
	end:
		addr=GET_ADDR(addr&0xffff, seg);
		return addr;
	case 3:
		/* register addressing */
		return NULL;	// we cannot return the 'address' of a reg
		break;
	}
	return 0;
}

static void set_def_segs(){
	/* sets default segments to op_data._segs_*/
	op_data.cs=CS;
	op_data.ss=SS;
	op_data.ds=DS;
	op_data.es=ES;
}

static void set_all_segs(uint16 seg){
	/*	sets all op_data segments to given segment
		used for segment override
	*/
	op_data.cs=seg;
	op_data.ss=seg;
	op_data.ds=seg;
	op_data.es=seg;
}

int op_prefix_size=0;

void process_instr_prefixes(){
	/*	processes instruction prefixes
		expects R_IP and IP to point to
		the current instruction
	*/
	uint32 offset=0;
	uint8 b=read_mem_8(R_IP+offset);
	set_def_segs();
	while(b==0x2e||
		b==0x36||
		b==0x3e||
		b==0x26||
		b==0xf3||
		b==0xf2){
		
		switch(b){
		case 0x2e:
			/* CS */
			set_all_segs(CS);
			break;
		case 0x36:
			/* SS */
			set_all_segs(SS);
			break;
		case 0x3E:
			/* DS */
			set_all_segs(DS);
			break;
		case 0x26:
			/* ES */
			set_all_segs(ES);
			break;
		}

		switch(b){
		case 0xf3:
			/* REP/REPE */
			op_data.rep=1;
			break;
		case 0xf2:
			/* REPNE */
			op_data.repne=1;
			break;
		}
		IP++;
		R_IP=GET_ADDR(IP, CS);
		b=read_mem_8(R_IP);
	}
	/* when we get here, R_IP and IP point to opcode */
}

/* to make life easier */

#define OP_DS op_data.ds
#define OP_SS op_data.ss
#define M_REG op_data.reg
#define M_RM op_data.rm
#define M_MOD op_data.mod

/*	warning : This macro takes the 1-byte opcode byte into account already.
	Therefore, if an instruction has 1 op byte and an imm16 operand, you should do:
	INC_IP(2), _NOT_ INC_IP(3) */
#define INC_IP(offset)\
	IP+=offset+1

#define SET_IP(val)\
	IP=GET_ADDR(val)

#define SET_IP_REL(val)\
	IP+=val;

/* JMP rel8 */
void op_0xeb(){
	int8 rel8=(int8)RRELIP8(1);	/* signed */
#ifdef SHOW_DEBUG
	out_opinfo("JMP %x", (IP+2+rel8));
#endif
	if(execute_flag){
		SET_IP_REL(rel8+2);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JMP rel16 */
void op_0xe9(){
	int16 rel16=(int16)RRELIP16(1);
#ifdef SHOW_DEBUG
	out_opinfo("JMP %x", (IP+3+rel16));
#endif
	if(execute_flag){
		SET_IP_REL(rel16+3);
	}
	else{
		INC_IP(2);
	}
	return;
}

void jcc_8(int8 rel, int cond){
	if(cond){
		SET_IP_REL(rel+2);
	}
	else{
		INC_IP(1);
	}
}

/* JE rel8 */
void op_0x74(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JE %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, FLAG_ZF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JNE/JNZ rel8 */
void op_0x75(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JNE %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, !FLAG_ZF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JLE/JG rel8 */
void op_0x7e(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JLE %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, FLAG_ZF || (FLAG_SF!=FLAG_OF));
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JL/JGE rel8 */
void op_0x7c(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JLE %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, FLAG_SF!=FLAG_OF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JB/JNAE rel8 */
void op_0x72(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JB %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, FLAG_CF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JBE/JNA rel8 */
void op_0x76(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JBE %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, FLAG_CF | FLAG_ZF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JP/JPE rel8 */
void op_0x7a(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JP %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, FLAG_PF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JO rel8 */
void op_0x70(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JO %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, FLAG_OF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JS rel8 */
void op_0x78(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JS %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, FLAG_SF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JNL rel8 */
void op_0x7d(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JNL %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, FLAG_SF==FLAG_OF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JNLE rel8 */
void op_0x7f(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JNLE %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, !FLAG_ZF && FLAG_SF==FLAG_OF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JNB/JAE rel8 */
void op_0x73(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JNB %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, !FLAG_CF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JNBE/JA rel8 */
void op_0x77(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JA %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, !FLAG_CF && !FLAG_ZF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JNP/JPO rel8 */
void op_0x7b(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JNP %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, !FLAG_PF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JNO rel8 */
void op_0x71(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("Jcc %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, !FLAG_OF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/* JNS rel8 */
void op_0x79(){
	int rel8=(char)RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("JNS %x", (IP+2+rel8));
#endif
	if(execute_flag){
		jcc_8(rel8, !FLAG_SF);
	}
	else{
		INC_IP(1);
	}
	return;
}

/** MOV Instructions **/

/* MOV+ r16, imm16 */
void op_0xb8(){
	uint16 imm16;
	MOD_REG_RM(0);
	/* r16 is in lower 3 bits of instruction */
	imm16=RRELIP16(1);
	if(execute_flag)
		WREG16(M_RM, imm16);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %x", text_regs[M_RM], (uint32)imm16);
#endif
	INC_IP(2);
	return;
}

/* MOV r/m8, r8 */
void op_0x88(){
	MOD_REG_RM(1);
	if(execute_flag)
		write_rm_val_8(RREG8(M_REG), OP_DS);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", print_rm_val_8(OP_DS), text_regs[BREG(M_REG)]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* MOV r/m16, r16 */
void op_0x89(){
	MOD_REG_RM(1);
	if(execute_flag)
		write_rm_val_16(RREG16(M_REG), OP_DS);
	
#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", print_rm_val_16(OP_DS), text_regs[M_REG]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* MOV r8, r/m8 */
void op_0x8a(){
	uint8 rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	if(execute_flag)
		WREG8(M_REG, rm8);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", text_regs[BREG(M_REG)], print_rm_val_8(OP_DS));
#endif
	INC_IP(1+disp_size);
	return;
}

/* MOV r16, r/m16 */
void op_0x8b(){
	uint8 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	if(execute_flag)
		WREG16(M_REG, rm16);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", text_regs[(M_REG)], print_rm_val_16(OP_DS));
#endif
	INC_IP(disp_size+1);
	return;
}

/* MOV r/m16, Sreg */
void op_0x8c(){
	uint16 sreg;
	MOD_REG_RM(1);
	sreg=RSREG(M_REG);
	if(execute_flag)
		write_rm_val_16(sreg, OP_DS);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", print_rm_val_16(OP_DS), TEXTSREG(M_REG));
#endif
	INC_IP(disp_size+1);
	return;
}

/* MOV Sreg, r/m16 */
void op_0x8e(){
	uint16 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	if(execute_flag)
		WSREG(M_REG, rm16);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", TEXTSREG(M_REG), print_rm_val_16(OP_DS));
#endif
	INC_IP(disp_size+1);
	return;
}

/* MOV al, moffs8 */
void op_0xa0(){
	uint16 moffs8=RRELIP16(1);
	if(execute_flag)
		AL=SRMEM8(moffs8, OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("MOV AL, [%x]", (uint32)moffs8);
#endif
	INC_IP(2);
	return;
}

/* MOV AX, moffs16 */
void op_0xa1(){
	uint16 moffs16=RRELIP16(1);
	if(execute_flag)
		AX=SRMEM16(moffs16, OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("MOV AL, [%x]", (uint32)moffs16);
#endif
	INC_IP(2);
	return;
}

/* MOV moffs8, AL */
void op_0xa2(){
	uint16 moffs8=RRELIP16(1);
	if(execute_flag)
		SWMEM8(AL, moffs8, OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("MOV [%x], AL", (uint32)moffs8);
#endif
	INC_IP(2);
	return;
}

/* MOV moffs16, AX */
void op_0xa3(){
	uint16 moffs16=RRELIP16(1);
	if(execute_flag)
		SWMEM16(AX, moffs16, OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("MOV [%x], AX", (uint32)moffs16);
#endif
	INC_IP(2);
	return;
}

/* MOV+ r8, imm8 */
void op_0xb0(){
	uint8 imm8=RRELIP8(1);
	MOD_REG_RM(0);
	if(execute_flag)
		WREG8((M_RM), imm8);
#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %x", text_regs[BREG(M_RM)], (uint32)imm8);
#endif
	INC_IP(1);
	return;
}

/* Various (?) */
void op_0xc6(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 0:
		/* MOV r/m8, imm8 */
		{
			uint8 imm8=RRELIP8(2+get_rm_disp_size());
			if(execute_flag)
				write_rm_val_8(imm8, OP_DS);

#ifdef SHOW_DEBUG
			out_opinfo("MOV %s, %x", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	}
	return;
}

/* Various 0xc7 */
void op_0xc7(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 0:
		/* MOV r/m16, imm16 */
	{
			  uint16 imm16=RRELIP16(2+get_rm_disp_size());
			  if(execute_flag)
				 write_rm_val_16(imm16, OP_DS);

#ifdef SHOW_DEBUG
			  out_opinfo("MOV %s, %x", print_rm_val_16(OP_DS), (uint32)imm16);
#endif
			  INC_IP(3+disp_size);
			  return;
	}
	}
	return;
}

uint16 add_16(uint16 a, uint16 b){
	setf_add16(a, b);
	return a+b;
}

uint8 add_8(uint8 a, uint8 b){
	setf_add8(a, b);
	return a+b;
}

uint16 adc_16(uint16 a, uint16 b){
	setf_adc16(a, b, FLAG_CF);
	return a+b+FLAG_CF;
}

uint8 adc_8(uint8 a, uint8 b){
	setf_adc8(a, b, FLAG_CF);
	return a+b+FLAG_CF;
}

uint8 sub_8(uint8 a, uint8 b){
	setf_sub8(a, b);
	return a-b;
}

uint16 sub_16(uint16 a, uint16 b){
	setf_sub16(a, b);
	return a-b;
}

uint16 cmp_16(uint16 a, uint16 b){
	return sub_16(a, b);
}

uint8 cmp_8(uint8 a, uint8 b){
	return sub_8(a, b);
}

uint16 mul_8(uint8 a, uint8 b){
	setf_mul8(a, b);
	return (uint16)a*b;
}

uint32 mul_16(uint16 a, uint16 b){
	setf_mul16(a, b);
	return (uint32)a*b;
}

int16 imul_8(int8 a, int8 b){
	setf_imul8(a, b);
	return (int8)a*b;
}

int32 imul_16(int16 a, int16 b){
	setf_imul16(a, b);
	return (int16)a*b;
}

uint8 inc_8(uint8 n){
	setf_inc8(n);
	return n+1;
}

uint16 inc_16(uint16 n){
	setf_inc16(n);
	return n+1;
}

uint8 dec_8(uint8 n){
	setf_dec8(n);
	return n-1;
}

uint16 dec_16(uint16 n){
	setf_dec16(n);
	return n-1;
}

uint8 and_8(uint8 a, uint8 b){
	setf_and8(a, b);
	return a&b;
}

uint16 and_16(uint16 a, uint16 b){
	setf_and16(a, b);
	return a&b;
}

uint8 or_8(uint8 a, uint8 b){
	setf_or8(a, b);
	return a|b;
}

uint16 or_16(uint16 a, uint16 b){
	setf_or16(a, b);
	return a|b;
}

uint8 xor_8(uint8 a, uint8 b){
	setf_xor8(a, b);
	return a^b;
}

uint16 xor_16(uint16 a, uint16 b){
	setf_xor16(a, b);
	return a^b;
}

void call_near_rel(int16 disp){
	stack_push(IP);
	IP=IP+disp;
}

void call_near_abs(uint16 addr){
	stack_push(IP);
	IP=addr;
}

void call_far_abs(uint16 seg, uint16 addr){
	stack_push(CS);
	stack_push(IP);
	CS=seg;
	IP=addr;
}

void ret_near(uint16 pop_count){
	int i=0;
	while(i++<pop_count){
		stack_pop();
	}
	if(stack_empty()){
		// terminate
		system_halt();
		return;
	}
	IP=stack_pop();
}


void ret_far(uint16 pop_count){
	int i=0;
	while(i++<pop_count){
		stack_pop();
	}
	if(stack_empty()){
		// terminate
	}
	CS=stack_pop();
	IP=stack_pop();
}

/* ADD AL, imm8 */
void op_0x04(){
	uint8 imm8=RRELIP16(1);
	if(execute_flag)
		AL=add_8(AL, imm8);

#ifdef SHOW_DEBUG
	out_opinfo("ADD AL, %x", (uint32)imm8);
#endif
	INC_IP(1);
	return;
}

/* ADD AX, imm16 */
void op_0x05(){
	uint16 imm16=RRELIP16(1);
	if(execute_flag)
		AX=add_16(AX, imm16);

#ifdef SHOW_DEBUG
	out_opinfo("ADD AX, %x", (uint32)imm16);
#endif
	INC_IP(2);
	return;
}

/* various 0x80 */
void op_0x80(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 0:
		{
			/* ADD r/m8, imm8 */
			uint8 imm8=RRELIP8(2+get_rm_disp_size());
			uint8 rm8;
			rm8=read_rm_val_8(OP_DS);
			if(execute_flag)
				write_rm_val_8(add_8(rm8, imm8), OP_DS);

#ifdef SHOW_DEBUG
			out_opinfo("ADD %s, %x", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 1:
		{
			/* OR rm8, imm8 */
			uint8 rm8, imm8;
			rm8=read_rm_val_8(OP_DS);
			imm8=RRELIP8(2+disp_size);
			write_rm_val_8(or_8(rm8, imm8), OP_DS);
			#ifdef SHOW_DEBUG
			out_opinfo("OR %s, %x", print_rm_val_8(OP_DS), (uint32)imm8);
			#endif
			INC_IP(2+disp_size);
			return;
		}
	case 2:
		{
			/* ADC r/m8, imm8 */
			uint8 rm8, imm8;
			rm8=read_rm_val_8(OP_DS);
			imm8=RRELIP8(2+get_rm_disp_size());
			if(execute_flag)
				write_rm_val_8(adc_8(rm8, imm8), OP_DS);

#ifdef SHOW_DEBUG
			out_opinfo("ADC %s, %x", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 4:
		{
			/* AND rm8, imm8 */
			uint8 rm8, imm8;
			rm8=read_rm_val_8(OP_DS);
			imm8=RRELIP8(1+disp_size);
			write_rm_val_8(and_8(rm8, imm8), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("AND %s, %x", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 5:
		{
			/* SUB rm8, imm8 */
			uint8 rm8, imm8;
			rm8=read_rm_val_8(OP_DS);
			imm8=RRELIP8(2+get_rm_disp_size());
			if(execute_flag)
				write_rm_val_8(sub_8(rm8, imm8), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("SUB %s, %x", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 6:
		{
			/* XOR rm8, imm8 */
			uint8 rm8, imm8;
			rm8=read_rm_val_8(OP_DS);
			imm8=RRELIP8(2+disp_size);
			write_rm_val_8(xor_8(rm8, imm8), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("XOR %s, %x", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 7:
		{
			/* CMP rm8, imm8 */
			uint8 rm8, imm8;
			rm8=read_rm_val_8(OP_DS);
			imm8=RRELIP8(2+get_rm_disp_size());
			if(execute_flag)
				cmp_8(rm8, imm8);
#ifdef SHOW_DEBUG
			out_opinfo("CMP %s, %x", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	}
	return;
}

/* Various 0x81 */
void op_0x81(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 0:
		{
			/* ADD r/m16, imm16 */
			uint16 rm16, imm16;
			rm16=read_rm_val_16(OP_DS);
			imm16=RRELIP16(2+disp_size);
			if(execute_flag)
				write_rm_val_16(add_16(rm16, imm16), OP_DS);

#ifdef SHOW_DEBUG
	out_opinfo("ADD %s, %x", print_rm_val_16(OP_DS), (uint32)imm16);
#endif
			INC_IP(3+disp_size);
			return;
		}
	case 1:
		{
			/* OR rm16, imm16 */
			uint16 rm16, imm16;
			rm16=read_rm_val_16(OP_DS);
			imm16=RRELIP16(2+disp_size);
			write_rm_val_16(or_16(rm16, imm16), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("OR %s, %x", print_rm_val_16(OP_DS), (uint32)imm16);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 2:
		{
			/* ADC rm16, imm16 */
			uint16 rm16, imm16;
			rm16=read_rm_val_16(OP_DS);
			imm16=RRELIP16(2+disp_size);

			if(execute_flag)
				write_rm_val_16(adc_16(rm16, imm16), OP_DS);

#ifdef SHOW_DEBUG
			out_opinfo("ADC %s, %x", print_rm_val_16(OP_DS), (uint32)imm16);
#endif
			INC_IP(3+disp_size);
			return;
		}
	case 4:
		{
			/* AND rm16, imm16 */
			uint16 rm16, imm16;
			rm16=read_rm_val_16(OP_DS);
			imm16=RRELIP16(1+disp_size);
			write_rm_val_16(and_8(rm16, imm16), OP_DS);
	#ifdef SHOW_DEBUG
			out_opinfo("AND %s, %x", print_rm_val_8(OP_DS), (uint32)imm16);
	#endif
			INC_IP(3+disp_size);
			return;
		}
	case 5:
		{
			/* SUB rm16, imm16 */
			uint16 rm16, imm16;
			rm16=read_rm_val_16(OP_DS);
			imm16=RRELIP16(2+disp_size);

			if(execute_flag)
				write_rm_val_16(sub_16(rm16, imm16), OP_DS);

	#ifdef SHOW_DEBUG
			out_opinfo("SUB %s, %x", print_rm_val_16(OP_DS), (uint32)imm16);
	#endif
			INC_IP(3+disp_size);
			return;
		}
	case 6:
		{
			/* XOR rm16, imm16 */
			uint16 rm16, imm16;
			rm16=read_rm_val_16(OP_DS);
			imm16=RRELIP16(2+disp_size);
			write_rm_val_16(xor_16(rm16, imm16), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("XOR %s, %x", print_rm_val_16(OP_DS), (uint32)imm16);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 7:
		{
			/* CMP rm16, imm16 */
			uint16 rm16, imm16;
			rm16=read_rm_val_16(OP_DS);
			imm16=RRELIP16(2+disp_size);
			if(execute_flag)
				cmp_16(rm16, imm16);

		#ifdef SHOW_DEBUG
			out_opinfo("CMP %s, %x", print_rm_val_16(OP_DS), (uint32)imm16);
		#endif
			INC_IP(3+disp_size);
			return;
		}
	}
	return;
}

/* Various 0x83 */
void op_0x83(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 0:
		{
			/* ADD r/m16, imm8 */
			uint16 rm16;
			uint8 imm8;
			rm16=read_rm_val_16(OP_DS);
			imm8=RRELIP8(2+disp_size);
			if(execute_flag)
				write_rm_val_16(add_16(rm16, imm8), OP_DS);
			  
#ifdef SHOW_DEBUG
			out_opinfo("ADD %s, %x", print_rm_val_16(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 1:
		{
			/* OR rm16, imm8 */
			uint16 rm16, imm8;
			rm16=read_rm_val_16(OP_DS);
			imm8=signext8(RRELIP8(2+disp_size));
			write_rm_val_16(or_16(rm16, imm8), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("OR %s, %x", print_rm_val_16(OP_DS), (uint32)imm8);
#endif
			INC_IP(1+disp_size);
			return;
		}
	case 2:
		{
			/* ADC r/m16, imm8 */
			uint16 rm16;
			uint8 imm8;
			rm16=read_rm_val_16(OP_DS);
			imm8=RRELIP8(2+disp_size);
			if(execute_flag)
				write_rm_val_16(adc_16(rm16, imm8), OP_DS);

#ifdef SHOW_DEBUG
			out_opinfo("ADC %s, %x", print_rm_val_16(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 4:
		{
			/* AND rm16, imm8 */
			uint16 rm16;
			uint16 imm8;
			rm16=read_rm_val_16(OP_DS);
			imm8=signext8(RRELIP8(1+disp_size));
			write_rm_val_16(and_16(rm16, imm8), OP_DS);
	#ifdef SHOW_DEBUG
			out_opinfo("AND %s, %x", print_rm_val_16(OP_DS), (uint32)imm8);
	#endif
			INC_IP(2+disp_size);
			return;
		}
	case 5:
		{
			/* SUB r/m16, imm8 */
			uint16 rm16;
			uint8 imm8;
			rm16=read_rm_val_16(OP_DS);
			imm8=RRELIP8(2+disp_size);
			if(execute_flag)
				write_rm_val_16(sub_16(rm16, signext8(imm8)), OP_DS);

#ifdef SHOW_DEBUG
			out_opinfo("SUB %s, %x", print_rm_val_16(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 6:
		{
			/* XOR rm16, imm8 */
			uint16 rm16, imm8;
			rm16=read_rm_val_16(OP_DS);
			imm8=signext8(RRELIP8(2+disp_size));
			write_rm_val_16(xor_16(rm16, imm8), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("XOR %s, %x", print_rm_val_16(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 7:
		{
			/* CMP r/m16, imm8 */
			uint16 rm16;
			uint8 imm8;
			rm16=read_rm_val_16(OP_DS);
			imm8=RRELIP8(2+disp_size);
			if(execute_flag)
			cmp_16(rm16, signext8(imm8));

	#ifdef SHOW_DEBUG
			out_opinfo("CMP %s, %x", print_rm_val_16(OP_DS), (uint32)imm8);
	#endif
			INC_IP(2+disp_size);
			return;
		}
	}
	return;
}

/* ADD r/m8, r8 */
void op_0x00(){
	uint8 rm8, r8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	r8=RREG8(M_REG);
	if(execute_flag)
		write_rm_val_8(add_8(rm8, r8), OP_DS);

#ifdef SHOW_DEBUG
	out_opinfo("ADD %s, %s", print_rm_val_8(OP_DS), text_regs[BREG(M_REG)]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* ADD r/m16, r16 */
void op_0x01(){
	uint16 rm16, r16;

	MOD_REG_RM(1);

	rm16=read_rm_val_16(OP_DS);
	r16=RREG16(M_REG);
	if(execute_flag)
		write_rm_val_16(add_16(rm16, r16), OP_DS);

#ifdef SHOW_DEBUG
	out_opinfo("ADD %s, %s", print_rm_val_16(OP_DS), text_regs[M_REG]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* ADD r8, r/m8 */
void op_0x02(){
	uint8 r8, rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	r8=RREG8(M_REG);
	if(execute_flag)
		WREG8((M_REG), add_8(r8, rm8));

#ifdef SHOW_DEBUG
	out_opinfo("ADD %s, %s", print_rm_val_8(OP_DS), text_regs[BREG(M_REG)]);
#endif
	INC_IP(0+disp_size);
	return;
}

/* ADD r16, r/m16 */
void op_0x03(){
	uint16 r16, rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	r16=RREG16(M_REG);
	if(execute_flag)
		WREG16(M_REG, add_16(r16, rm16));

#ifdef SHOW_DEBUG
	out_opinfo("ADD %s, %s", text_regs[M_REG], print_rm_val_16(OP_DS));
#endif
	INC_IP(1+disp_size);
	return;
}

/* ADC AL, imm8 */
void op_0x14(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		AL=adc_8(AL, imm8);
#ifdef SHOW_DEBUG
	out_opinfo("ADC AL, %x", (uint32)imm8);
#endif
	INC_IP(1);
	return;
}

/* ADC AX, imm16 */
void op_0x15(){
	uint16 imm16=RRELIP16(1);
	if(execute_flag)
		AX=adc_16(AX, imm16);

#ifdef SHOW_DEBUG
	out_opinfo("ADD AX, %x", (uint32)imm16);
#endif
	INC_IP(2);
	return;
}

/* ADC rm8, r8 */
void op_0x10(){
	uint8 rm8, r8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	r8=RREG8(M_REG);
	if(execute_flag)
		write_rm_val_8(adc_8(rm8, r8), OP_DS);

#ifdef SHOW_DEBUG
	out_opinfo("ADC %s, %s", print_rm_val_8(OP_DS), text_regs[BREG(M_REG)]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* ADC rm16, r16 */
void op_0x11(){
	uint16 rm16, r16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	r16=RREG16(M_REG);
	if(execute_flag)
		write_rm_val_16(adc_16(rm16, r16), OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("ADD %s, %s", print_rm_val_16(OP_DS), text_regs[(op_data.reg)]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* ADC r8, rm8 */
void op_0x12(){
	uint8 r8, rm8;
	MOD_REG_RM(1);

	r8=RREG8(M_REG);
	rm8=read_rm_val_8(OP_DS);

	if(execute_flag)
		WREG8((M_REG), adc_8(r8, rm8));

#ifdef SHOW_DEBUG
	out_opinfo("ADC %s, %x", text_regs[BREG(M_REG)], (uint32)rm8);
#endif
	INC_IP(1+disp_size);
	return;
}

/* ADC r16, rm16 */
void op_0x13(){
	uint16 r16, rm16;
	MOD_REG_RM(1);

	rm16=read_rm_val_16(OP_DS);
	r16=RREG16(M_REG);

	if(execute_flag)
		WREG16(M_REG, adc_16(r16, rm16));

#ifdef SHOW_DEBUG
	out_opinfo("ADC %s, %s", print_rm_val_8(OP_DS), text_regs[op_data.reg]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* Various 0xff */
void op_0xff(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 0:
		{
				/* INC rm16 */
				uint16 rm16=read_rm_val_16(OP_DS);
				write_rm_val_16(inc_16(rm16), OP_DS);
#ifdef SHOW_DEBUG
				out_opinfo("INC %s", print_rm_val_16(OP_DS));
#endif
				INC_IP(1+disp_size);
				return;
		}
	case 1:
		{
			/* DEC rm16 */
			uint16 rm16=read_rm_val_16(OP_DS);
			write_rm_val_16(dec_16(rm16), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("DEC %s", print_rm_val_16(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	case 2:
		{
			/* CALL rm16 */
			uint16 rm16=read_rm_val_16(OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("CALL %s", print_rm_val_16(OP_DS));
#endif
			INC_IP(1+disp_size);
			if(execute_flag){
				call_near_abs(rm16, 2+disp_size);
			}
			return;
		}
	case 3:
		{
			/* CALL m16:16 */
			uint16 m16, offs16;
			m16=RMEM16(RRELIP16(1));
			offs16=RMEM16(RRELIP16(1)+2);
			INC_IP(2);
#ifdef SHOW_DEBUG
			out_opinfo("CALL [%x]", RRELIP16(1));
#endif
			if(execute_flag){
				call_far_abs(m16, offs16);
			}
			return;
		}
	case 6:
		{
			/* PUSH r/m16 */
			uint16 rm16=read_rm_val_16(OP_DS);
			if(execute_flag)
				stack_push(rm16);
#ifdef SHOW_DEBUG
			out_opinfo("PUSH %s", print_rm_val_8(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	}
	return;
}

/* PUSH+ r16 */
void op_0x50(){
	uint16 r16;
	MOD_REG_RM(0);
	r16=M_RM;
	if(execute_flag)
		stack_push((uint16)RREG16(r16));
#ifdef SHOW_DEBUG
	out_opinfo("PUSH %s", text_regs[r16]);
#endif
	INC_IP(0);
	return;
}

/* PUSH imm8 */
void op_0x6a(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		stack_push((uint16)imm8);
#ifdef SHOW_DEBUG
	out_opinfo("PUSH %x", (uint16)imm8);
#endif
	INC_IP(1);
	return;
}

/* PUSH imm16 */
void op_0x68(){
	uint16 imm16=RRELIP16(1);
	if(execute_flag)
		stack_push(imm16);
#ifdef SHOW_DEBUG
	out_opinfo("PUSH %x", imm16);
#endif
	INC_IP(2);
	return;
}

/* PUSH CS */
void op_0x0e(){
	if(execute_flag)
		stack_push(CS);
#ifdef SHOW_DEBUG
	out_opinfo("PUSH CS");
#endif
	INC_IP(0);
	return;
}

/* PUSH SS */
void op_0x16(){
	if(execute_flag)
		stack_push(SS);
#ifdef SHOW_DEBUG
	out_opinfo("PUSH SS");
#endif
	INC_IP(0);
	return;
}

/* PUSH DS */
void op_0x1e(){
	if(execute_flag)
		stack_push(DS);
#ifdef SHOW_DEBUG
	out_opinfo("PUSH DS");
#endif
	INC_IP(0);
	return;
}

/* PUSH ES */
void op_0x06(){
	if(execute_flag)
		stack_push(ES);
#ifdef SHOW_DEBUG
	out_opinfo("PUSH ES");
#endif
	INC_IP(0);
	return;
}

/* Various 0x8f */
void op_0x8f(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 6:
	{
			  /* POP r/m16 */
			  if(execute_flag)
				 write_rm_val_16(stack_pop(), OP_DS);
#ifdef SHOW_DEBUG
			  out_opinfo("POP %s", print_rm_val_8(OP_DS));
#endif
			  INC_IP(1+disp_size);
			  return;
	}
	}
	return;
}

/* POP+ r16 */
void op_0x58(){
	MOD_REG_RM(0);
	if(execute_flag)
		WREG16(M_RM, stack_pop());
#ifdef SHOW_DEBUG
	out_opinfo("POP %s", text_regs[M_RM]);
#endif
	INC_IP(0);
	return;
}

/* POP DS */
void op_0x1f(){
	if(execute_flag)
		DS=stack_pop();
#ifdef SHOW_DEBUG
	out_opinfo("POP DS");
#endif
	INC_IP(0);
}

/* POP ES */
void op_0x07(){
	if(execute_flag)
		ES=stack_pop();
#ifdef SHOW_DEBUG
	out_opinfo("POP ES");
#endif
	INC_IP(0);
}

/* POP SS */
void op_0x17(){
	if(execute_flag)
		SS=stack_pop();
#ifdef SHOW_DEBUG
	out_opinfo("POP SS");
#endif
	INC_IP(0);
}

/* OUT imm8, AL */
void op_0xe6(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		write_io_port(AL, (uint16)imm8);
#ifdef SHOW_DEBUG
	out_opinfo("OUT %x, AL", imm8);
#endif
	INC_IP(1);
	return;
}

/* OUT imm8, AX */
void op_0xe7(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag){
		write_io_port(LO_BYTE(AX), imm8);
		write_io_port(HI_BYTE(AX), imm8+1);
	}
#ifdef SHOW_DEBUG
	out_opinfo("OUT %x, AX", imm8);
#endif
	INC_IP(1);
	return;
}

/* OUT DX, AL */
void op_0xee(){
	if(execute_flag)
		write_io_port(AL, DX);
#ifdef SHOW_DEBUG
	out_opinfo("OUT DX, AL");
#endif
	INC_IP(0);
	return;
}

/* OUT DX, AX */
void op_0xef(){
	if(execute_flag){
		write_io_port(LO_BYTE(AX), DX);
		write_io_port(HI_BYTE(AX), DX+1);
	}
#ifdef SHOW_DEBUG
	out_opinfo("OUT DX, AX");
#endif
	INC_IP(0);
	return;
}

/* IN AL, imm8 */
void op_0xe4(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		AL=read_io_port(imm8);
#ifdef SHOW_DEBUG
	out_opinfo("IN AL, %x", imm8);
#endif
	INC_IP(1);
	return;
}

/* IN AX, imm8 */
void op_0xe5(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag){
		AX=0;
		AX|=read_io_port(imm8+1);
		AX<<=8;
		AX|=read_io_port(imm8);
	}
#ifdef SHOW_DEBUG
	out_opinfo("IN AX, %x", imm8);
#endif
	INC_IP(1);
	return;
}

/* IN AL, DX */
void op_0xec(){
	if(execute_flag)
		AL=read_io_port(DX);
#ifdef SHOW_DEBUG
	out_opinfo("IN AL, DX");
#endif
	INC_IP(0);
	return;
}

/* IN AX, DX */
void op_0xed(){
	if(execute_flag){
		AX=0;
		AX|=read_io_port(DX+1);
		AX<<=8;
		AX|=read_io_port(DX);
	}
#ifdef SHOW_DEBUG
	out_opinfo("IN AX, DX");
#endif
	INC_IP(0);
	return;
}

/* SUB AL, imm8 */
void op_0x2c(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		AL=sub_8(AL, imm8);

#ifdef SHOW_DEBUG
	out_opinfo("SUB AL, %x", (uint32)imm8);
#endif
	INC_IP(1);
	return;
}

/* SUB AX, imm16 */
void op_0x2d(){
	uint16 imm16=RRELIP16(1);
	if(execute_flag)
		AX=sub_16(AX, imm16);

#ifdef SHOW_DEBUG
	out_opinfo("SUB AX, %x", (uint32)imm16);
#endif
	INC_IP(2);
	return;
}

/* SUB rm8, r8 */
void op_0x28(){
	uint8 rm8, r8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	r8=RREG8(M_REG);
	if(execute_flag)
		write_rm_val_8(sub_8(rm8, r8), OP_DS);

#ifdef SHOW_DEBUG
	out_opinfo("SUB %s, %s", print_rm_val_8(OP_DS), text_regs[BREG(M_REG)]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* SUB rm16, r16 */
void op_0x29(){
	uint16 rm16, r16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	r16=RREG16(M_REG);
	if(execute_flag)
		write_rm_val_16(sub_16(rm16, r16), OP_DS);

#ifdef SHOW_DEBUG
	out_opinfo("SUB %s, %s", print_rm_val_16(OP_DS), text_regs[M_REG]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* SUB r16, rm16 */
void op_0x2a(){
	uint16 r16, rm16;
	MOD_REG_RM(1);
	r16=RREG16(M_REG);
	rm16=read_rm_val_16(OP_DS);
	if(execute_flag)
		WREG16(M_REG, sub_16(r16, rm16));

#ifdef SHOW_DEBUG
	out_opinfo("SUB %s, %s", text_regs[M_REG], print_rm_val_16(OP_DS));
#endif
	INC_IP(1+disp_size);
	return;
}

/* CMP AL, imm8 */
void op_0x3c(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		cmp_8(AL, imm8);
#ifdef SHOW_DEBUG
	out_opinfo("CMP AL, %x", (uint32)imm8);
#endif
	INC_IP(1);
	return;
}

/* CMP AX, imm16 */
void op_0x3d(){
	uint16 imm16=RRELIP16(1);
	if(execute_flag)
		cmp_16(AX, imm16);
#ifdef SHOW_DEBUG
	out_opinfo("CMP AX, %x", (uint32)imm16);
#endif
	INC_IP(2);
	return;
}

/* CMP rm8, r8 */
void op_0x38(){
	uint8 rm8, r8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	r8=RREG8(M_REG);
	if(execute_flag)
		cmp_8(rm8, r8);

#ifdef SHOW_DEBUG
	out_opinfo("CMP %s, %s", print_rm_val_8(OP_DS), text_regs[BREG(M_REG)]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* CMP rm16, r16 */
void op_0x39(){
	uint16 rm16, r16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	r16=RREG16(M_REG);
	if(execute_flag)
		cmp_16(rm16, r16);

#ifdef SHOW_DEBUG
	out_opinfo("CMP %s, %s", print_rm_val_16(OP_DS), text_regs[(M_REG)]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* CMP r8, rm8 */
void op_0x3a(){
	uint8 r8, rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	r8=RREG8(M_REG);

	if(execute_flag)
		cmp_8(r8, rm8);
#ifdef SHOW_DEBUG
	out_opinfo("CMP %s, %s", print_rm_val_8(OP_DS), text_regs[BREG(M_REG)]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* CMP r16, rm16 */
void op_0x3b(){
	uint16 r16, rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	r16=RREG16(M_REG);
	if(execute_flag)
		cmp_16(r16, rm16);
#ifdef SHOW_DEBUG
	out_opinfo("CMP %s, %s", print_rm_val_16(OP_DS), text_regs[(M_REG)]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* various 0xFE */
void op_0xfe(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 0:
		{
			/* INC rm8 */
			uint8 rm8=read_rm_val_8(OP_DS);
			write_rm_val_8(inc_8(rm8), OP_DS);
	#ifdef SHOW_DEBUG
			out_opinfo("INC %s", print_rm_val_8(OP_DS));
	#endif
			INC_IP(1+disp_size);
		return;
		}
	case 1:
		{
			/* DEC rm8 */
			uint8 rm8=read_rm_val_8(OP_DS);
			write_rm_val_8(dec_8(rm8), OP_DS);
	#ifdef SHOW_DEBUG
			out_opinfo("DEC %s", print_rm_val_8(OP_DS));
	#endif
			INC_IP(1+disp_size);
			return;
		}
	}
	return;
}

/* INC r16 */
void op_0x40(){
	MOD_REG_RM(0);
	WREG16(M_RM, inc_16(RREG16(M_RM)));
#ifdef SHOW_DEBUG
	out_opinfo("INC %s", text_regs[M_RM]);
#endif
	INC_IP(0);
	return;
}

/* DEC r16 */
void op_0x48(){
	MOD_REG_RM(0);
	WREG16(M_RM, dec_16(RREG16(M_RM)));

#ifdef SHOW_DEBUG
	out_opinfo("DEC %s", text_regs[M_RM]);
#endif
	INC_IP(0);
	return;
}

uint8 neg_8(uint8 a){
	setf_neg8(a);
	return -a;
}

uint16 neg_16(uint16 a){
	setf_neg16(a);
	return -a;
}

uint8 not_8(uint8 a){
	return ~a;
}

uint16 not_16(uint16 a){
	return ~a;
}

/* various 0xf6 */
void op_0xf6(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 2:
	{
		/* NOT rm8 */
		uint8 rm8=read_rm_val_8(OP_DS);
		write_rm_val_8(not_8(rm8), OP_DS);
#ifdef SHOW_DEBUG
		out_opinfo("NOT %s", print_rm_val_8(OP_DS));
#endif
		INC_IP(1+disp_size);
		return;
	}
	case 3:
	{
		/* NEG rm8 */
		uint8 rm8=read_rm_val_8(OP_DS);
		write_rm_val_8(neg_8(rm8), OP_DS);
#ifdef SHOW_DEBUG
		out_opinfo("NEG %s", print_rm_val_8(OP_DS));
#endif
		INC_IP(1+disp_size);
		return;
	}
	case 4:
		{
			/* MUL rm8 */
			uint8 rm8=read_rm_val_8(OP_DS);
			AX=mul_8(AL, rm8);
	#ifdef SHOW_DEBUG
			out_opinfo("MUL %s", print_rm_val_8(OP_DS));
	#endif
			INC_IP(1+disp_size);
			return;
		}
	case 5:
		{
			/* IMUL rm8 */
			int8 rm8=read_rm_val_8(OP_DS);
			AX=imul_8(AL, rm8);
	#ifdef SHOW_DEBUG
			out_opinfo("IMUL %s", print_rm_val_8(OP_DS));
	#endif
			INC_IP(1+disp_size);
			return;
		}
	case 6:
		{
			/* DIV rm8 */
			if(execute_flag){
				uint8 rm8=read_rm_val_8(OP_DS);
				AH=AX % rm8;
				AL=AX / rm8;
			}
#ifdef SHOW_DEBUG
			out_opinfo("DIV %s", print_rm_val_8(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	case 7:
		{
			/* IDIV rm8 */
			if(execute_flag){
				int8 rm8=read_rm_val_8(OP_DS);
				if(rm8==0) WRITE_DEBUG("Error : division by zero");
				else{
					AH=AX % rm8;
					AL=AX / rm8;
				}
			}
#ifdef SHOW_DEBUG
			out_opinfo("IDIV %s", print_rm_val_8(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	}
	return;
}

/* various 0xf7 */
void op_0xf7(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 2:
		{
			/* NOT rm16 */
			uint8 rm16=read_rm_val_16(OP_DS);
			write_rm_val_16(not_16(rm16), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("NOT %s", print_rm_val_16(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	case 3:
		{
			/* NEG rm16 */
			uint8 rm16=read_rm_val_16(OP_DS);
			write_rm_val_16(neg_16(rm16), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("NEG %s", print_rm_val_16(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	case 4:
		{
			/* MUL rm16 */
			uint16 rm16=read_rm_val_16(OP_DS);
			uint32 res=mul_16(AX, rm16);
			DX=HI_WORD(res);
			AX=LO_WORD(res);
#ifdef SHOW_DEBUG
			out_opinfo("MUL %s", print_rm_val_16(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	case 5:
		{
			/* IMUL rm16 */
			int16 rm16=read_rm_val_16(OP_DS);
			int32 res=imul_16(AX, rm16);
			DX=HI_WORD(res);
			AX=LO_WORD(res);
#ifdef SHOW_DEBUG
			out_opinfo("IMUL %s", print_rm_val_16(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	case 6:
		{
			/* DIV rm16 */
			if(execute_flag){
				uint16 rm16=read_rm_val_16(OP_DS);
				uint32 v=DWORD_B(DX, AX);
				AX=v / rm16;
				DX=v % rm16;
			}
#ifdef SHOW_DEBUG
			out_opinfo("DIV %s", print_rm_val_16(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	case 7:
		{
			/* IDIV rm16 */
			if(execute_flag){
				int16 rm16=read_rm_val_16(OP_DS);
				int32 v=signext16(DWORD_B(DX, AX));
				AX=v / rm16;
				DX=v % rm16;
			}
#ifdef SHOW_DEBUG
			out_opinfo("IDIV %s", print_rm_val_16(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	}
	return;
}

uint8 shl_8(uint8 a, uint8 s){
	setf_shl8(a, s);
	return a<<s;
}

uint16 shl_16(uint16 a, uint8 s){
	setf_shl16(a, s);
	return a<<s;
}

uint8 shr_8(uint8 n, uint8 s){
	setf_shl8(n, s);
	return n>>s;
}

uint16 shr_16(uint16 n, uint8 s){
	setf_shr16(n, s);
	return n>>s;
}

void op_0xd0(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 4:
		{
			/* SAL rm8, 1 */
			uint8 rm8;
			rm8=read_rm_val_8(OP_DS);
			write_rm_val_8(shl_8(rm8, 1), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("SHL %s, 1", print_rm_val_8(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	case 5:
		{
			/* SHR rm8, 1 */
			  uint8 rm8;
			  rm8=read_rm_val_8(OP_DS);
			  write_rm_val_8(shr_8(rm8, 1), OP_DS);
#ifdef SHOW_DEBUG
			  out_opinfo("SHR %s, 1", print_rm_val_8(OP_DS));
#endif
			  INC_IP(1+disp_size);
			  return;
		}

	}
	return;
}

void op_0xd2(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 4:
	{
			  /* SAL rm8, CL */
			  uint8 rm8;
			  rm8=read_rm_val_8(OP_DS);
			  write_rm_val_8(shl_8(rm8, CL), OP_DS);
#ifdef SHOW_DEBUG
			  out_opinfo("SHL %s, CL", print_rm_val_8(OP_DS));
#endif
			  INC_IP(1+disp_size);
			  return;
	}
	case 5:
	{
		{
			/* SHR rm8, CL */
			uint8 rm8;
			rm8=read_rm_val_8(OP_DS);
			write_rm_val_8(shr_8(rm8, CL), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("SHR %s, 1", print_rm_val_8(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	}
	}
	return;
}

void op_0xd1(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 4:
		{
			/* SHL rm16, 1 */
			uint16 rm16;
			rm16=read_rm_val_16(OP_DS);
			write_rm_val_8(shl_16(rm16, 1), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("SHL %s, 1", print_rm_val_16(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	case 5:
		{
			/* SHR rm16, 1 */
			uint16 rm16;
			rm16=read_rm_val_16(OP_DS);
			write_rm_val_8(shr_16(rm16, 1), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("SHR %s, 1", print_rm_val_16(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	}
	return;
}

void op_0xd3(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 4:
		{
			/* SHL rm16, CL */
			uint16 rm16;
			rm16=read_rm_val_16(OP_DS);
			write_rm_val_8(shl_16(rm16, CL), OP_DS);

		#ifdef SHOW_DEBUG
			out_opinfo("SHL %s, CL", print_rm_val_16(OP_DS));
		#endif
			INC_IP(1+disp_size);
			return;
		}
	case 5:
		{
			/* SHR rm16, CL */
			uint16 rm16;
			rm16=read_rm_val_16(OP_DS);
			write_rm_val_8(shr_16(rm16, CL), OP_DS);

#ifdef SHOW_DEBUG
			out_opinfo("SHR %s, CL", print_rm_val_16(OP_DS));
#endif
			INC_IP(1+disp_size);
			return;
		}
	}
	return;
}

void op_0xc0(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 4:
		{
			/* SHL rm8, imm8 */
			uint8 rm8, imm8;
			rm8=read_rm_val_8(OP_DS);
			imm8=RRELIP8(2+disp_size);
			write_rm_val_8(shl_8(rm8, imm8), OP_DS);
	#ifdef SHOW_DEBUG
			out_opinfo("SHL %s, %X", print_rm_val_8(OP_DS), imm8);
	#endif
			INC_IP(2+disp_size);
			return;
	}
	case 5:
		{
			/* SHR rm8, imm8 */
			uint8 rm8, imm8;
			rm8=read_rm_val_8(OP_DS);
			imm8=RRELIP8(2+disp_size);
			write_rm_val_8(shr_8(rm8, imm8), OP_DS);
	#ifdef SHOW_DEBUG
			out_opinfo("SHR %s, %X", print_rm_val_8(OP_DS), imm8);
	#endif
			INC_IP(2+disp_size);
			return;
		}
	}
	return;
}

void op_0xc1(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
	case 4:
		{
			/* SHL rm16, imm8 */
			uint16 rm16;
			uint8 imm8;
			rm16=read_rm_val_16(OP_DS);
			imm8=RRELIP8(2+disp_size);
			write_rm_val_16(shl_16(rm16, imm8), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("SHL %s, %X", print_rm_val_16(OP_DS), imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	case 5:
		{
			/* SHR rm16, imm8 */
			uint16 rm16;
			uint8 imm8;
			rm16=read_rm_val_16(OP_DS);
			imm8=RRELIP8(2+disp_size);
			write_rm_val_16(shr_16(rm16, imm8), OP_DS);
#ifdef SHOW_DEBUG
			out_opinfo("SHR %s, %X", print_rm_val_16(OP_DS), imm8);
#endif
			INC_IP(2+disp_size);
			return;
		}
	}
	return;
}

/* AND AL, imm8 */
void op_0x24(){
	uint8 imm8=RRELIP8(1);
	AL=and_8(AL, imm8);
#ifdef SHOW_DEBUG
	out_opinfo("AND AL, %x", imm8);
#endif
	INC_IP(1);
	return;
}

/* AND AX, imm16 */
void op_0x25(){
	uint16 imm16=RRELIP16(1);
	AX=and_16(AX, imm16);
#ifdef SHOW_DEBUG
	out_opinfo("AND AX, %x", imm16);
#endif
	INC_IP(2);
	return;
}

/* AND rm8, r8 */
void op_0x20(){
	uint8 rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	if(execute_flag){
		write_rm_val_8(and_8(rm8, RREG8(M_REG)), OP_DS);
	}
#ifdef SHOW_DEBUG
	out_opinfo("AND %s, %s", print_rm_val_8(OP_DS), text_regs[BREG(M_REG)]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* AND rm16, r16 */
void op_0x21(){
	uint16 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	if(execute_flag){
		write_rm_val_16(and_16(rm16, RREG16(M_REG)), OP_DS);
	}
#ifdef SHOW_DEBUG
	out_opinfo("AND %s, %s", print_rm_val_16(OP_DS), text_regs[M_REG]);
#endif
	INC_IP(1+disp_size);
	return;
}

/* AND r8, rm8 */
void op_0x22(){
	uint8 rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	if(execute_flag){
		WREG8(M_REG, and_8(RREG8(M_REG), rm8));
	}
#ifdef SHOW_DEBUG
	out_opinfo("AND %s, %s", text_regs[BREG(M_REG)], print_rm_val_8(OP_DS));
#endif
	INC_IP(1+disp_size);
	return;
}

/* AND r16, rm16 */
void op_0x23(){
	uint16 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	if(execute_flag){
		WREG16(M_REG, and_16(RREG16(M_REG), rm16));
	}
#ifdef SHOW_DEBUG
	out_opinfo("AND %s, %s", text_regs[M_REG], print_rm_val_8(OP_DS));
#endif
	INC_IP(1+disp_size);
	return;
}

/* OR AL, imm8 */
void op_0x0c(){
	uint8 imm8=RRELIP8(1);
	AL=or_8(AL, imm8);
#ifdef SHOW_DEBUG
	out_opinfo("OR AL, %x", imm8);
#endif
	INC_IP(1);
}

/* OR AX, imm16 */
void op_0x0d(){
	uint16 imm16=RRELIP16(1);
	AX=or_16(AX, imm16);
#ifdef SHOW_DEBUG
	out_opinfo("OR AX, %x", imm16);
#endif
	INC_IP(2);
}

/* OR rm8, r8 */
void op_0x08(){
	uint8 rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	write_rm_val_8(or_8(RREG8(M_REG), rm8), OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("OR %s, %s", print_rm_val_8(OP_DS), TEXTREG8(M_REG));
#endif
	INC_IP(1+disp_size);
}

/* OR rm16, r16 */
void op_0x09(){
	uint16 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	write_rm_val_16(or_16(RREG16(M_REG), rm16), OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("OR %s, %s", print_rm_val_16(OP_DS), TEXTREG16(M_REG));
#endif
	INC_IP(1+disp_size);
}

/* OR r8, rm8 */
void op_0x0a(){
	uint8 rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	write_rm_val_8(or_8(RREG8(M_REG), rm8), OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("OR %s, %s", TEXTREG8(M_REG), print_rm_val_8(OP_DS));
#endif
	INC_IP(1+disp_size);
}

/* OR r16, rm16 */
void op_0x0b(){
	uint16 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	write_rm_val_16(or_16(RREG16(M_REG), rm16), OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("OR %s, %s", TEXTREG16(M_REG), print_rm_val_16(OP_DS));
#endif
	INC_IP(1+disp_size);
}

/* XOR AL, imm8 */
void op_0x34(){
	uint8 imm8=RRELIP8(1);
	AL=xor_8(AL, imm8);
#ifdef SHOW_DEBUG
	out_opinfo("XOR AL, %x", imm8);
#endif
	INC_IP(1);
}

/* XOR AX, imm16 */
void op_0x35(){
	uint16 imm16=RRELIP16(1);
	AX=xor_16(AX, imm16);
#ifdef SHOW_DEBUG
	out_opinfo("XOR AX, %x", imm16);
#endif
	INC_IP(2);
}

/* XOR rm8, r8 */
void op_0x30(){
	uint8 rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	write_rm_val_8(or_8(RREG8(M_REG), rm8), OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("XOR %s, %s", print_rm_val_8(OP_DS), TEXTREG8(M_REG));
#endif
	INC_IP(1+disp_size);
}

/* XOR rm16, r16 */
void op_0x31(){
	uint16 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	write_rm_val_16(xor_16(RREG16(M_REG), rm16), OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("XOR %s, %s", print_rm_val_16(OP_DS), TEXTREG16(M_REG));
#endif
	INC_IP(1+disp_size);
}

/* XOR r8, rm8 */
void op_0x32(){
	uint8 rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	write_rm_val_8(xor_8(RREG8(M_REG), rm8), OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("XOR %s, %s", TEXTREG8(M_REG), print_rm_val_8(OP_DS));
#endif
	INC_IP(1+disp_size);
}

/* XOR r16, rm16 */
void op_0x33(){
	uint16 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	write_rm_val_16(xor_16(RREG16(M_REG), rm16), OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("XOR %s, %s", TEXTREG16(M_REG), print_rm_val_16(OP_DS));
#endif
	INC_IP(1+disp_size);
}

/* CALL rel16 */
void op_0xe8(){
	int16 rel16;
	rel16=RRELIP16(1);
#ifdef SHOW_DEBUG
	out_opinfo("CALL %x", IP+3+rel16);
#endif
	INC_IP(2);
	if(execute_flag){
		call_near_rel(rel16, 3);
	}
}

/* CALL ptr16:16 */
void op_0x9a(){
	uint16 ptr16, offs16;
	ptr16=RRELIP16(1);
	offs16=RRELIP16(3);

#ifdef SHOW_DEBUG
	out_opinfo("CALL %x:%x", ptr16, offs16);
#endif
	INC_IP(4);
	if(execute_flag){
		call_far_abs(ptr16, offs16, 5);
	}
}

/* RET */
void op_0xc3(){
#ifdef SHOW_DEBUG
	out_opinfo("RET");
#endif
	if(execute_flag){
		ret_near(0);
	}
	else{
		INC_IP(0);
	}
}

/* RETF */
void op_0xcb(){
#ifdef SHOW_DEBUG
	out_opinfo("RETF");
#endif
	if(execute_flag){
		ret_far(0);
	}
	else{
		INC_IP(0);
	}
}

/* RET imm16 */
void op_0xc2(){
	uint16 imm16=RRELIP16(1);
#ifdef SHOW_DEBUG
	out_opinfo("RET %x", imm16);
#endif
	if(execute_flag){
		ret_near(imm16);
	}
	else{
		INC_IP(2);
	}
}

/* RETF imm16 */
void op_0xca(){
	uint16 imm16=RRELIP16(1);
#ifdef SHOW_DEBUG
	out_opinfo("RETF %x", imm16);
#endif
	if(execute_flag){
		ret_far(imm16);
	}
	else{
		INC_IP(2);
	}
}

/* execute interrupt call */
void int_call(uint8 inum){
	uint16 new_cs, new_ip;
	uint16 mem_offset=inum<<2;
	new_cs=read_mem_16(mem_offset);
	new_ip=read_mem_16(mem_offset+2)+0x400;
	stack_push(FLAGS);
	stack_push(CS);
	stack_push(IP+2);
	FLAG_IF=0;
	CS=new_cs;
	IP=new_ip;
}

void int_return(){
	IP=stack_pop();
	CS=stack_pop();
	FLAGS=stack_pop();
	// if we were in an external interrupt
	// decrement extern interrupt nest count
	if(sys_state.is_in_extern_int) sys_state.is_in_extern_int--;

}

/* INT imm8 */
void op_0xcd(){
	uint8 imm8=RRELIP8(1);
#ifdef SHOW_DEBUG
	out_opinfo("INT %x", imm8);
#endif
	if(execute_flag){
		int_call(imm8);
	}else{
		INC_IP(1);
	}
}

/* IRET */
void op_0xcf(){
#ifdef SHOW_DEBUG
	out_opinfo("IRET");
#endif
	if(execute_flag){
		int_return();
	}else{
		INC_IP(0);
	}
}

/* LAHF */
void op_0x9f(){
	AH=NULL;
	AH|=(FLAG_CF |
		(1 << 1) |
		(FLAG_PF << 2)|
		(FLAG_AF << 4)|
		(FLAG_ZF << 5)|
		(FLAG_SF << 7));
#ifdef SHOW_DEBUG
	out_opinfo("LAHF");
#endif
	INC_IP(0);
}

/* SAHF */
void op_0x9e(){
	FLAG_CF=(AH & 1);
	FLAG_PF=(AH & 4);
	FLAG_AF=(AH & 16);
	FLAG_ZF=(AH & 64);
	FLAG_SF=(AH & 128);
#ifdef SHOW_DEBUG
	out_opinfo("SAHF");
#endif
	INC_IP(0);
}

/* LEA r16, m */
void op_0x8d(){
	uint16 m;
	MOD_REG_RM(1);
	m=get_rm_addr(OP_DS);
	m-=(OP_DS << 4);	// we don't want segments here
	WREG16(M_REG, m);
#ifdef SHOW_DEBUG
	out_opinfo("LEA %s, %x", TEXTREG16(M_REG), (uint32)m);
#endif
	INC_IP(1+get_rm_disp_size());
}

/* MOVS m8, m8 */
void op_0xa4(){
	SWMEM8(SRMEM8(SI, OP_DS), DI, OP_DS);
	if(FLAG_DF == 0){
		SI++;
		DI++;
	}
	else{
		SI--;
		DI--;
	}
#ifdef SHOW_DEBUG
	out_opinfo("MOVSB");
#endif
	INC_IP(0);
}

/* MOVS m16, m16 */
void op_0xa5(){
	SWMEM16(SRMEM16(SI, OP_DS), DI, OP_DS);
	if(FLAG_DF == 0){
		SI+=2;
		DI+=2;
	}
	else{
		SI-=2;
		DI-=2;
	}
#ifdef SHOW_DEBUG
	out_opinfo("MOVSW");
#endif
	INC_IP(0);
}
