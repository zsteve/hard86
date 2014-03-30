/**
 * @file Opcode implementations
 * x86 emulator engine for hard86
 * Stephen Zhang, 2014
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

/* read relative to IP */
#define RRELIP8(v)\
	read_mem_8(sys_state.r_ip+v)

#define RRELIP16(v)\
	read_mem_16(sys_state.r_ip+v)

/* macro for returning "good" (nothing happened!) */
#define RET_GOOD return 0

static uint32 disp_size;	/* size of displacement, can be 0, 1 or 2 */

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
	RET_GOOD;
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
	RET_GOOD;
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
		return RREG8(op_data.rm);
		break;
	}
	RET_GOOD;
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
	uint16 disp16=RRELIP16(2);
	uint8 disp8=RRELIP8(2);
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
		addr=GET_ADDR(addr & 0xffff, seg);
		write_mem_8(val, addr);
		RET_GOOD;
	case 3:
		/* register addressing */
		write_reg(BREG(op_data.rm), val);
		break;
	}
	RET_GOOD;
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
		RET_GOOD;
	case 3:
		/* register addressing */
		write_reg((op_data.rm), val);
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
	set_def_segs();
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
int op_0xeb(){
	int8 rel8=(int8)RRELIP8(1);	/* signed */
#ifdef SHOW_DEBUG
	out_opinfo("JMP %X", (IP+2+rel8));
#endif
	if(execute_flag){
		SET_IP_REL(rel8+2);
	}
	else{
		INC_IP(1);
	}
	RET_GOOD;
}

/* JMP rel16 */
int op_0xe9(){
	int16 rel16=(int16)RRELIP16(1);
#ifdef SHOW_DEBUG
	out_opinfo("JMP %X", (IP+3+rel16));
#endif
	if(execute_flag){
		SET_IP_REL(rel16+2);
	}
	else{
		INC_IP(2);
	}
	RET_GOOD;
}

/** MOV Instructions **/

/* MOV+ r16, imm16 */
int op_0xb8(){
	uint16 imm16;
	MOD_REG_RM(0);
	/* r16 is in lower 3 bits of instruction */
	imm16=RRELIP16(1);
	if(execute_flag)
		WREG16(M_RM, imm16);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %X", text_regs[M_RM], (uint32)imm16);
#endif
	INC_IP(2);
	RET_GOOD;
}

/* MOV r/m8, r8 */
int op_0x88(){
	MOD_REG_RM(1);
	if(execute_flag)
		write_rm_val_8(RREG8(M_REG), OP_DS);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", print_rm_val_8(OP_DS), text_regs[BREG(M_REG)]);
#endif
	INC_IP(1+disp_size);
	RET_GOOD;
}

/* MOV r/m16, r16 */
int op_0x89(){
	MOD_REG_RM(1);
	if(execute_flag)
		write_rm_val_16(RREG16(M_REG), OP_DS);
	
#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", print_rm_val_16(OP_DS), text_regs[M_REG]);
#endif
	INC_IP(1+disp_size);
	RET_GOOD;
}

/* MOV r8, r/m8 */
int op_0x8a(){
	uint8 rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(OP_DS);
	if(execute_flag)
		WREG8(M_REG, rm8);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", text_regs[BREG(M_REG)], print_rm_val_8(OP_DS));
#endif
	INC_IP(disp_size);
	RET_GOOD;
}

/* MOV r16, r/m16 */
int op_0x8b(){
	uint8 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	if(execute_flag)
		WREG16(M_REG, rm16);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", text_regs[(M_REG)], print_rm_val_16(OP_DS));
#endif
	INC_IP(disp_size+1);
	RET_GOOD;
}

/* MOV r/m16, Sreg */
int op_0x8c(){
	uint16 sreg;
	MOD_REG_RM(1);
	sreg=RREG16(M_REG);
	if(execute_flag)
		write_rm_val_16(sreg, OP_DS);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", print_rm_val_16(OP_DS), text_regs[M_REG]);
#endif
	INC_IP(disp_size+1);
	RET_GOOD;
}

/* MOV Sreg, r/m16 */
int op_0x8e(){
	uint16 rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	if(execute_flag)
		WREG16(M_REG, rm16);

#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %s", text_regs[M_REG], print_rm_val_16(OP_DS));
#endif
	INC_IP(disp_size+1);
	RET_GOOD;
}

/* MOV al, moffs8 */
int op_0xa0(){
	uint16 moffs8=RRELIP16(1);
	if(execute_flag)
		AL=SRMEM8(moffs8, OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("MOV AL, [%X]", (uint32)moffs8);
#endif
	INC_IP(2);
	RET_GOOD;
}

/* MOV AX, moffs16 */
int op_0xa1(){
	uint16 moffs16=RRELIP16(1);
	if(execute_flag)
		AX=SRMEM16(moffs16, OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("MOV AL, [%X]", (uint32)moffs16);
#endif
	INC_IP(2);
	RET_GOOD;
}

/* MOV moffs8, AL */
int op_0xa2(){
	uint16 moffs8=RRELIP16(1);
	if(execute_flag)
		SWMEM8(AL, moffs8, OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("MOV [%X], AL", (uint32)moffs8);
#endif
	INC_IP(2);
	RET_GOOD;
}

/* MOV moffs16, AX */
int op_0xa3(){
	uint16 moffs16=RRELIP16(1);
	if(execute_flag)
		SWMEM16(AX, moffs16, OP_DS);
#ifdef SHOW_DEBUG
	out_opinfo("MOV [%X], AX", (uint32)moffs16);
#endif
	INC_IP(2);
	RET_GOOD;
}

/* MOV+ r8, imm8 */
int op_0xb0(){
	uint8 imm8=RRELIP8(1);
	MOD_REG_RM(0);
	if(execute_flag)
		WREG8((M_RM), imm8);
#ifdef SHOW_DEBUG
	out_opinfo("MOV %s, %X", text_regs[BREG(M_RM)], (uint32)imm8);
#endif
	INC_IP(1);
	RET_GOOD;
}

/* Various (?) */
int op_0xc6(){
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
			out_opinfo("MOV %s, %X", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			RET_GOOD;
		}
	}
	RET_GOOD;
}

/* Various 0xc7 */
int op_0xc7(){
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
			  out_opinfo("MOV %s, %X", print_rm_val_16(OP_DS), (uint32)imm16);
#endif
			  INC_IP(3+disp_size);
			  RET_GOOD;
	}
	}
	RET_GOOD;
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

/* ADD AL, imm8 */
int op_0x04(){
	uint8 imm8=RRELIP16(1);
	if(execute_flag)
		AL=add_8(AL, imm8);

#ifdef SHOW_DEBUG
	out_opinfo("ADD AL, %X", (uint32)imm8);
#endif
	INC_IP(1);
	RET_GOOD;
}

/* ADD AX, imm16 */
int op_0x05(){
	uint16 imm16=RRELIP16(1);
	if(execute_flag)
		AX=add_16(AX, imm16);

#ifdef SHOW_DEBUG
	out_opinfo("ADD AX, %X", (uint32)imm16);
#endif
	INC_IP(2);
	RET_GOOD;
}

/* various 0x80 */
int op_0x80(){
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
			  out_opinfo("ADD %s, %X", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			  INC_IP(2+disp_size);
			  RET_GOOD;
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
			out_opinfo("ADC %s, %X", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			RET_GOOD;
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
			out_opinfo("SUB %s, %X", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			RET_GOOD;
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
			  out_opinfo("CMP %s, %X", print_rm_val_8(OP_DS), (uint32)imm8);
#endif
			  INC_IP(2+disp_size);
			  RET_GOOD;
	}
	}
	RET_GOOD;
}

/* Various 0x81 */
int op_0x81(){
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
	out_opinfo("ADD %s, %X", print_rm_val_16(OP_DS), (uint32)imm16);
#endif
			INC_IP(3+disp_size);
			RET_GOOD;
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
			out_opinfo("ADC %s, %X", print_rm_val_16(OP_DS), (uint32)imm16);
#endif
			INC_IP(3+disp_size);
			RET_GOOD;
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
			  out_opinfo("SUB %s, %X", print_rm_val_16(OP_DS), (uint32)imm16);
#endif
			  INC_IP(3+disp_size);
			  RET_GOOD;
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
			  out_opinfo("CMP %s, %X", print_rm_val_16(OP_DS), (uint32)imm16);
#endif
			  INC_IP(3+disp_size);
			  RET_GOOD;
	}
	}
	RET_GOOD;
}

/* Various 0x83 */
int op_0x83(){
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
			out_opinfo("ADD %s, %X", print_rm_val_16(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			RET_GOOD;
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
			out_opinfo("ADC %s, %X", print_rm_val_16(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			RET_GOOD;
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
			out_opinfo("SUB %s, %X", print_rm_val_16(OP_DS), (uint32)imm8);
#endif
			INC_IP(2+disp_size);
			RET_GOOD;
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
			  out_opinfo("CMP %s, %X", print_rm_val_16(OP_DS), (uint32)imm8);
#endif
			  INC_IP(2+disp_size);
			  RET_GOOD;
	}
	}
	RET_GOOD;
}

/* ADD r/m8, r8 */
int op_0x00(){
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
	RET_GOOD;
}

/* ADD r/m16, r16 */
int op_0x01(){
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
	RET_GOOD;
}

/* ADD r8, r/m8 */
int op_0x02(){
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
	RET_GOOD;
}

/* ADD r16, r/m16 */
int op_0x03(){
	uint16 r16, rm16;
	MOD_REG_RM(1);
	rm16=read_rm_val_16(OP_DS);
	r16=RREG16(M_REG);
	if(execute_flag)
		WREG16(M_REG, add_16(r16, rm16));

#ifdef SHOW_DEBUG
	out_opinfo("ADD %s, %s", print_rm_val_16(OP_DS), text_regs[M_REG]);
#endif
	INC_IP(0+disp_size);
	RET_GOOD;
}

/* ADC AL, imm8 */
int op_0x14(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		AL=adc_8(AL, imm8);
#ifdef SHOW_DEBUG
	out_opinfo("ADC AL, %X", (uint32)imm8);
#endif
	INC_IP(1);
	RET_GOOD;
}

/* ADC AX, imm16 */
int op_0x15(){
	uint16 imm16=RRELIP16(1);
	if(execute_flag)
		AX=adc_16(AX, imm16);

#ifdef SHOW_DEBUG
	out_opinfo("ADD AX, %X", (uint32)imm16);
#endif
	INC_IP(2);
	RET_GOOD;
}

/* ADC rm8, r8 */
int op_0x10(){
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
	RET_GOOD;
}

/* ADC rm16, r16 */
int op_0x11(){
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
	RET_GOOD;
}

/* ADC r8, rm8 */
int op_0x12(){
	uint8 r8, rm8;
	MOD_REG_RM(1);

	r8=RREG8(M_REG);
	rm8=read_rm_val_8(OP_DS);

	if(execute_flag)
		WREG8((M_REG), adc_8(r8, rm8));

#ifdef SHOW_DEBUG
	out_opinfo("ADC %s, %X", text_regs[BREG(M_REG)], (uint32)rm8);
#endif
	INC_IP(1+disp_size);
	RET_GOOD;
}

/* ADC r16, rm16 */
int op_0x13(){
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
	RET_GOOD;
}

/* Various 0xff */
int op_0xff(){
	uint8 op_ext;
	MOD_REG_RM(1);
	op_ext=M_REG;
	switch(op_ext){
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
			RET_GOOD;
		}
	}
	RET_GOOD;
}

/* PUSH+ r16 */
int op_0x50(){
	uint16 r16;
	MOD_REG_RM(0);
	r16=M_RM;
	if(execute_flag)
		stack_push((uint16)RREG16(r16));
#ifdef SHOW_DEBUG
	out_opinfo("PUSH %s", text_regs[r16]);
#endif
	INC_IP(0);
	RET_GOOD;
}

/* PUSH imm8 */
int op_0x6a(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		stack_push((uint16)imm8);
#ifdef SHOW_DEBUG
	out_opinfo("PUSH %X", (uint16)imm8);
#endif
	INC_IP(1);
	RET_GOOD;
}

/* PUSH imm16 */
int op_0x68(){
	uint16 imm16=RRELIP16(1);
	if(execute_flag)
		stack_push(imm16);
#ifdef SHOW_DEBUG
	out_opinfo("PUSH %X", imm16);
#endif
	INC_IP(2);
	RET_GOOD;
}

/* Various 0x8f */
int op_0x8f(){
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
			  RET_GOOD;
	}
	}
	RET_GOOD;
}

/* POP+ r16 */
int op_0x58(){
	MOD_REG_RM(0);
	if(execute_flag)
		WREG16(M_RM, stack_pop());
#ifdef SHOW_DEBUG
	out_opinfo("POP %s", text_regs[M_RM]);
#endif
	INC_IP(0);
	RET_GOOD;
}

/* OUT imm8, AL */
int op_0xe6(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		write_io_port(AL, (uint16)imm8);
#ifdef SHOW_DEBUG
	out_opinfo("OUT %X, AL", imm8);
#endif
	INC_IP(1);
	RET_GOOD;
}

/* OUT imm8, AX */
int op_0xe7(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag){
		write_io_port(LO_BYTE(AX), imm8);
		write_io_port(HI_BYTE(AX), imm8+1);
	}
#ifdef SHOW_DEBUG
	out_opinfo("OUT %X, AX", imm8);
#endif
	INC_IP(1);
	RET_GOOD;
}

/* OUT DX, AL */
int op_0xee(){
	if(execute_flag)
		write_io_port(AL, DX);
#ifdef SHOW_DEBUG
	out_opinfo("OUT DX, AL");
#endif
	INC_IP(0);
	RET_GOOD;
}

/* OUT DX, AX */
int op_0xef(){
	if(execute_flag){
		write_io_port(LO_BYTE(AX), DX);
		write_io_port(HI_BYTE(AX), DX+1);
	}
#ifdef SHOW_DEBUG
	out_opinfo("OUT DX, AX");
#endif
	INC_IP(0);
	RET_GOOD;
}

/* IN AL, imm8 */
int op_0xe4(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		AL=read_io_port(imm8);
#ifdef SHOW_DEBUG
	out_opinfo("IN AL, %X", imm8);
#endif
	INC_IP(1);
	RET_GOOD;
}

/* IN AX, imm8 */
int op_0xe5(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag){
		AX=0;
		AX|=read_io_port(imm8+1);
		AX<<=8;
		AX|=read_io_port(imm8);
	}
#ifdef SHOW_DEBUG
	out_opinfo("IN AX, %X", imm8);
#endif
	INC_IP(1);
	RET_GOOD;
}

/* IN AL, DX */
int op_0xec(){
	if(execute_flag)
		AL=read_io_port(DX);
#ifdef SHOW_DEBUG
	out_opinfo("IN AL, DX");
#endif
	INC_IP(0);
	RET_GOOD;
}

/* IN AX, DX */
int op_0xed(){
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
	RET_GOOD;
}

/* SUB AL, imm8 */
int op_0x2c(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		AL=sub_8(AL, imm8);

#ifdef SHOW_DEBUG
	out_opinfo("SUB AL, %X", (uint32)imm8);
#endif
	INC_IP(1);
	RET_GOOD;
}

/* SUB AX, imm16 */
int op_0x2d(){
	uint16 imm16=RRELIP16(1);
	if(execute_flag)
		AX=sub_16(AX, imm16);

#ifdef SHOW_DEBUG
	out_opinfo("SUB AX, %X", (uint32)imm16);
#endif
	INC_IP(2);
	RET_GOOD;
}

/* SUB rm8, r8 */
int op_0x28(){
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
	RET_GOOD;
}

/* SUB rm16, r16 */
int op_0x29(){
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
	RET_GOOD;
}

/* SUB r16, rm16 */
int op_0x2a(){
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
	RET_GOOD;
}

/* CMP AL, imm8 */
int op_0x3c(){
	uint8 imm8=RRELIP8(1);
	if(execute_flag)
		cmp_8(AL, imm8);
#ifdef SHOW_DEBUG
	out_opinfo("CMP AL, %X", (uint32)imm8);
#endif
	INC_IP(1);
	RET_GOOD;
}

/* CMP AX, imm16 */
int op_0x3d(){
	uint16 imm16=RRELIP16(1);
	if(execute_flag)
		cmp_16(AX, imm16);
#ifdef SHOW_DEBUG
	out_opinfo("CMP AX, %X", (uint32)imm16);
#endif
	INC_IP(2);
	RET_GOOD;
}

/* CMP rm8, r8 */
int op_0x38(){
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
	RET_GOOD;
}

/* CMP rm16, r16 */
int op_0x39(){
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
	RET_GOOD;
}

/* CMP r8, rm8 */
int op_0x3a(){
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
	RET_GOOD;
}

/* CMP r16, rm16 */
int op_0x3b(){
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
	RET_GOOD;
}