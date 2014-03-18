/**
 * @file Opcode implementations
 * x86 emulator engine for hard86
 * Stephen Zhang, 2014
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "opcodes.h"
#include "emulator_engine.h"
#include "emulator_engine_interface.h"
#include "flags.h"

#define SHOW_DEBUG

extern op_data_type op_data;
extern sys_state_type sys_state;
extern char text_regs[22][6];

#define RMEM8(addr)\
	read_mem_8(addr)

#define WMEM8(val, addr)\
	write_mem_8(val, addr)

#define RMEM16(addr)\
	read_mem_16(addr)

#define SRMEM8(addr, seg)\
	read_mem_8(GET_ADDR(addr, seg))

#define SRMEM16(addr, seg)\
	read_mem_16(GET_ADDR(addr, seg))

#define SWMEM8(val, addr, seg)\
	write_mem_8(val, GET_ADDR(addr, seg))

#define SWMEM16(val, addr, seg)\
	write_mem_16(val, GET_ADDR(addr, seg))

#define RET_GOOD return 0

static uint32 disp_size;	/* size of displacement, can be 0, 1 or 2 */

uint8 read_rm_val_8(uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RMEM16(R_IP+2);
	uint8 disp8=RMEM8(R_IP+2);
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
		return read_reg(TO_BYTE_REG(op_data.rm));
		break;
	}
	RET_GOOD;
}

uint16 read_rm_val_16(uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RMEM16(R_IP+2);
	uint8 disp8=RMEM8(R_IP+2);
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
		return read_reg(TO_BYTE_REG(op_data.rm));
		break;
	}
	RET_GOOD;
}

static char rm_val_str[80]="";

char* print_rm_val_16(uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	char disp_str[32]="";
	uint16 disp16=RMEM16(R_IP+2);
	uint8 disp8=RMEM8(R_IP+2);
	uint32 addr=0;
	strcpy(rm_val_str, "word ptr [");
	switch(op_data.mod){
		/* modes */
	case 0:
		/* displacement only mode */
		strcat(rm_val_str, itoa(disp16, disp_str, 16));
		strcat(rm_val_str, "]");
		return rm_val_str;
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
		strcat(rm_val_str, "] + ");
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
	uint16 disp16=RMEM16(R_IP+2);
	uint8 disp8=RMEM8(R_IP+2);
	uint32 addr=0;
	strcpy(rm_val_str, "byte ptr [");
	switch(op_data.mod){
		/* modes */
	case 0:
		/* displacement only mode */
		strcat(rm_val_str, itoa(disp16, disp_str, 16));
		strcat(rm_val_str, "]");
		return rm_val_str;
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
		strcat(rm_val_str, "] , ");
		if(op_data.mod==1){
			strcat(rm_val_str, itoa(disp8, disp_str, 16));
		}
		else if(op_data.mod==2){
			strcat(rm_val_str, itoa(disp16, disp_str, 16));
		}
		return rm_val_str;
	case 3:
		/* register addressing */
		return text_regs[TO_BYTE_REG(op_data.rm)];
	}
	return "";
}

int write_rm_val_8(uint8 val, uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RMEM16(R_IP+2);
	uint8 disp8=RMEM8(R_IP+2);
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
		addr=GET_ADDR(addr & 0xffff, seg);
		write_mem_8(val, addr);
		RET_GOOD;
	case 3:
		/* register addressing */
		write_reg(TO_BYTE_REG(op_data.rm), val);
		break;
	}
	RET_GOOD;
}

int write_rm_val_16(uint16 val, uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RMEM16(R_IP+2);
	uint8 disp8=RMEM8(R_IP+2);
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
		RET_GOOD;
	case 3:
		/* register addressing */
		write_reg(TO_BYTE_REG(op_data.rm), val);
		break;
	}
	RET_GOOD;
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

void process_instr_prefixes(){
	/*	processes instruction prefixes
		expects R_IP and IP to point to
		the current instruction
	*/
	uint32 offset=0;
	uint8 b=read_mem_8(R_IP+offset);
	while(b==0x2e||
		b==0x36||
		b==0x3e||
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

#define OP_DS op_data.ds
#define OP_SS op_data.ss
#define M_REG op_data.reg

/* JMP rel8 */
int op_0xeb(){
	int8 rel8=(int8)RMEM8(R_IP+1);	/* signed */
	IP+=rel8+2;
#ifdef SHOW_DEBUG
	out_opinfo("JMP %X\n", IP);
#endif
	RET_GOOD;
}

/* JMP rel16 */
int op_0xe9(){
	int16 rel16=(int16)RMEM16(R_IP+1);
	IP+=rel16+1;
#ifdef SHOW_DEBUG
	out_opinfo("JMP %X\n", IP);
#endif
	RET_GOOD;
}

/** MOV Instructions **/

/* MOV+ r16, imm16 */
int op_0xb8(){
	uint8 r16;
	uint16 imm16;
	MOD_REG_RM(0);
	r16=op_data.rm;
	/* r16 is in lower 3 bits of instruction */
	imm16=RMEM16(R_IP+1);
	write_reg(r16, imm16);

	IP+=3;
#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %X\n", text_regs[r16], (uint32)imm16);
#endif
	RET_GOOD;
}

/* MOV r/m8, r8 */
int op_0x88(){
	uint8 r8;
	MOD_REG_RM(1);
	r8=read_reg(TO_BYTE_REG(M_REG));
	write_rm_val_8(r8, OP_DS);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %S, %S\n", print_rm_val_8(OP_DS), text_regs[TO_BYTE_REG(r8)]);
#endif
	RET_GOOD;
}

/* MOV r/m16, r16 */
int op_0x89(){
	uint16 r16;
	MOD_REG_RM(1);
	r16=read_reg(M_REG);
	write_rm_val_16(r16, OP_DS);

	IP+=2+disp_size;
#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s\n", print_rm_val_16(OP_DS), text_regs[r16]);
#endif
	RET_GOOD;
}

/* MOV r8, r/m8 */
int op_0x8a(){
	uint8 rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	write_reg(TO_BYTE_REG(M_REG), rm8);

	IP+=1+disp_size;
#ifdef SHOW_DEBUG
	out_opinfo("MOV %S, %S\n", text_regs[TO_BYTE_REG(M_REG)], print_rm_val_8(OP_DS));
#endif
	RET_GOOD;
}

/* MOV r16, r/m16 */
int op_0x8b(){
	uint8 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	write_reg((M_REG), rm16);

	IP+=2+disp_size;
#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s\n", text_regs[(M_REG)], print_rm_val_16(OP_DS));
#endif
	RET_GOOD;
}

/* MOV r/m16, Sreg */
int op_0x8c(){
	uint16 sreg;
	MOD_REG_RM(1);
	sreg=read_reg(M_REG);
	write_rm_val_16(sreg, OP_DS);

	IP+=1+disp_size;
#ifdef SHOW_DEBUG
	out_opinfo("MOV %S, %S\n", print_rm_val_16(OP_DS), text_regs[M_REG]);
#endif
	RET_GOOD;
}

/* MOV Sreg, r/m16 */
int op_0x8e(){
	uint16 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	write_reg(M_REG, rm16);

	IP+=1+disp_size;
#ifdef SHOW_DEBUG
	out_opinfo("MOV %S, %S\n", text_regs[M_REG], print_rm_val_16(OP_DS));
#endif
	RET_GOOD;
}

/* MOV al, moffs8 */
int op_0xa0(){
	uint16 moffs8=RMEM16(R_IP+1);
	AL=SRMEM8(moffs8, OP_DS);
	IP+=3;
#ifdef SHOW_DEBUG
	out_opinfo("MOV AL, [%X]\n", (uint32)moffs8);
#endif
	RET_GOOD;
}

/* MOV AX, moffs16 */
int op_0xa1(){
	uint16 moffs16=RMEM16(R_IP+1);
	AX=SRMEM16(moffs16, OP_DS);
	IP+=3;
#ifdef SHOW_DEBUG
	out_opinfo("MOV AL, [%X]\n", (uint32)moffs16);
#endif
	RET_GOOD;
}

/* MOV moffs8, AL */
int op_0xa2(){
	uint16 moffs8=RMEM16(R_IP+1);
	SWMEM8(AL, moffs8, OP_DS);
	IP+=3;
#ifdef SHOW_DEBUG
	out_opinfo("MOV [%X], AL\n", (uint32)moffs8);
#endif
	RET_GOOD;
}

/* MOV moffs16, AX */
int op_0xa3(){
	uint16 moffs16=RMEM16(R_IP+1);
	SWMEM16(AX, moffs16, OP_DS);
	IP+=3;
#ifdef SHOW_DEBUG
	out_opinfo("MOV [%X], AX\n", (uint32)moffs16);
#endif
	RET_GOOD;
}

/* MOV+ r8, imm8 */
int op_0xb0(){
	uint8 r8;
	uint8 imm8=RMEM8(R_IP+1);
	MOD_REG_RM(0);
	r8=op_data.reg;
	write_reg(TO_BYTE_REG(r8), r8);
	IP+=2;
#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %X\n", (uint32)imm8);
#endif
	RET_GOOD;
}

/* ADD AL, imm8 */
int op_0x04(){
	uint8 imm8=RMEM8(R_IP+1);
	AL+=imm8;

	setf_add8(AL, imm8);

	IP+=2;
#ifdef SHOW_DEBUG
	out_opinfo("ADD AL, %X\n", (uint32)imm8);
#endif
	RET_GOOD;
}

/* ADD AX, imm16 */
int op_0x05(){
	uint16 imm16=RMEM16(R_IP+1);
	AX+=imm16;

	setf_add16(AX, imm16);
	IP+=3;
#ifdef SHOW_DEBUG
	out_opinfo("ADD AX, %X", (uint32)imm16);
#endif
	RET_GOOD;
}

/* ADD r/m8, imm8 */
int op_0x80(){
	uint8 imm8=RMEM8(R_IP+2);
	uint8 rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	rm8+=imm8;
	write_rm_val_8(rm8, OP_DS);

	setf_add8(rm8, imm8);

	IP+=3;
#ifdef SHOW_DEBUG
	out_opinfo("ADD %s, %X\n", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
	RET_GOOD;
}

/* Various 0xff */
int op_0xff(){
	MOD_REG_RM(1);
	uint8 op_ext=op_data.reg;
	switch(op_ext){
	case 6:
		{
			/* PUSH r/m16 */
			uint16 rm16=read_rm_val_16(OP_DS);
			stack_push(rm16);
			IP+=2+disp_size;
#ifdef SHOW_DEBUG
			out_opinfo("PUSH %s\n", print_rm_val_8(OP_DS));
#endif
			RET_GOOD;
		}
	}
	RET_GOOD;
}

/* PUSH+ r16 */
int op_0x50(){
	MOD_REG_RM(0);
	uint8 r16=op_data.rm;
	stack_push((uint16)read_reg(r16));
	IP+=1;
#ifdef SHOW_DEBUG
	out_opinfo("PUSH %s\n", text_regs[r16]);
#endif
	RET_GOOD;
}

/* PUSH imm8 */
int op_0x6a(){
	uint8 imm8=RMEM8(R_IP+1);
	stack_push((uint16)imm8);
	IP+=2;
#ifdef SHOW_DEBUG
	out_opinfo("PUSH %X\n", (uint16)imm8);
#endif
	RET_GOOD;
}

/* PUSH imm16 */
int op_0x68(){
	uint16 imm16=RMEM16(R_IP+1);
	stack_push(imm16);
	IP+=3;
#ifdef SHOW_DEBUG
	out_opinfo("PUSH %X\n", imm16);
#endif
	RET_GOOD;
}

/* Various 0x8f */
int op_0x8f(){
	MOD_REG_RM(1);
	uint8 op_ext=op_data.reg;
	switch(op_ext){
	case 6:
	{
			  /* POP r/m16 */
			  write_rm_val_16(stack_pop(), OP_DS);
			  IP+=2;
#ifdef SHOW_DEBUG
			  out_opinfo("POP %s\n", print_rm_val_8(OP_DS));
#endif
			  RET_GOOD;
	}
	}
	RET_GOOD;
}

/* POP+ r16 */
int op_0x58(){
	MOD_REG_RM(0);
	uint8 r16=op_data.rm;
	write_reg(r16, stack_pop());
	IP+=1;
#ifdef SHOW_DEBUG
	out_opinfo("POP %s\n", text_regs[r16]);
#endif
	RET_GOOD;
}

/* OUT imm8, AL */
int op_0xe6(){
	uint8 imm8=RMEM8(R_IP+1);
	write_io_port(AL, (uint16)imm8);
	IP+=2;
#ifdef SHOW_DEBUG
	out_opinfo("OUT %X, AL\n", imm8);
#endif
	RET_GOOD;
}

/* OUT imm8, AX */
int op_0xe7(){
	uint8 imm8=RMEM8(R_IP+1);
	write_io_port(LO_BYTE(AX), imm8);
	write_io_port(HI_BYTE(AX), imm8+1);
	IP+=2;
#ifdef SHOW_DEBUG
	out_opinfo("OUT %X, AX\n", imm8);
#endif
	RET_GOOD;
}

/* OUT DX, AL */
int op_0xee(){
	write_io_port(AL, DX);
#ifdef SHOW_DEBUG
	out_opinfo("OUT DX, AL\n");
#endif
	RET_GOOD;
}

/* OUT DX, AX */
int op_0xef(){
	write_io_port(LO_BYTE(AX), DX);
	write_io_port(HI_BYTE(AX), DX+1);
#ifdef SHOW_DEBUG
	out_opinfo("OUT DX, AX\n");
#endif
	RET_GOOD;
}

/* IN AL, imm8 */
int op_0xe4(){
	uint8 imm8=RMEM8(R_IP+1);
	AL=read_io_port(imm8);
	IP+=2;
#ifdef SHOW_DEBUG
	out_opinfo("IN AL, %X\n", imm8);
#endif
	RET_GOOD;
}

/* IN AX, imm8 */
int op_0xe5(){
	uint8 imm8=RMEM8(R_IP+1);
	AX=0;
	AX|=read_io_port(imm8+1);
	AX<<=8;
	AX|=read_io_port(imm8);
	IP+=2;
#ifdef SHOW_DEBUG
	out_opinfo("IN AX, %X\n", imm8);
#endif
	RET_GOOD;
}

/* IN AL, DX */
int op_0xec(){
	AL=read_io_port(DX);
	IP++;
#ifdef SHOW_DEBUG
	out_opinfo("IN AL, DX\n");
#endif
	RET_GOOD;
}

/* IN AX, DX */
int op_0xed(){
	AX=0;
	AX|=read_io_port(DX+1);
	AX<<=8;
	AX|=read_io_port(DX);
	IP++;
#ifdef SHOW_DEBUG
	out_opinfo("IN AX, DX\n");
#endif
	RET_GOOD;
}