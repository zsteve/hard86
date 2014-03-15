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

uint8 read_rm_val_8(uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RMEM16(R_IP+2);
	uint8 disp8=RMEM8(R_IP+2);
	uint32 addr=0;
	switch(op_data.mod){
		/* modes */
	case 0:
		if(op_data.rm==6){
			addr=disp16;
			break;
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
		addr=GET_ADDR(addr&0xffff, seg);
		return read_mem_8(addr);
	case 3:
		/* register addressing */
		return read_reg(TO_BYTE_REG(op_data.rm));
		break;
	}
	return 0;
}

uint16 read_rm_val_16(uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RMEM16(R_IP+2);
	uint8 disp8=RMEM8(R_IP+2);
	uint32 addr=0;
	switch(op_data.mod){
		/* modes */
	case 0:
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
		addr=GET_ADDR(addr&0xffff, seg);
		return read_mem_16(addr);
	case 3:
		/* register addressing */
		return read_reg(TO_BYTE_REG(op_data.rm));
		break;
	}
	return 0;
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
	switch(op_data.mod){
		/* modes */
	case 0:

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
		addr=GET_ADDR(addr & 0xffff, seg);
		write_mem_8(val, addr);
		return 0;
	case 3:
		/* register addressing */
		write_reg(TO_BYTE_REG(op_data.rm), val);
		break;
	}
	return 0;
}

int write_rm_val_16(uint16 val, uint16 seg){
	/* assumes you have mod-reg-rm */
	/* does MOD RM <- VAL */
	uint16 disp16=RMEM16(R_IP+2);
	uint8 disp8=RMEM8(R_IP+2);
	uint32 addr=0;
	switch(op_data.mod){
		/* modes */
	case 0:
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
		addr=GET_ADDR(addr&0xffff, seg);
		write_mem_16(val, addr);
		return 0;
	case 3:
		/* register addressing */
		write_reg(TO_BYTE_REG(op_data.rm), val);
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

/* JMP rel8 */
int op_0xeb(){
	int8 rel8=(int8)RMEM8(R_IP+1);	/* signed */
	IP+=rel8+2;
#ifdef SHOW_DEBUG
	printf("JMP %X\n", IP);
#endif
	return 0;
}

/* JMP rel16 */
int op_0xe9(){
	int16 rel16=(int16)RMEM16(R_IP+1);
	IP+=rel16+1;
#ifdef SHOW_DEBUG
	printf("JMP %X\n", IP);
#endif
	return 0;
}

/* MOV+ r16, imm16 */
int op_0xb8(){
	uint8 r16;
	uint16 imm16;
	MOD_REG_RM(1);
	r16=op_data.rm;
	/* r16 is in lower 3 bits of instruction */
	imm16=RMEM16(R_IP+1);
	write_reg(r16, imm16);

	IP+=3;
#ifdef SHOW_DEBUG
	printf("MOV %s, %X\n", text_regs[r16], imm16);
#endif
	return 0;
}

/* ADD AL, imm8 */
int op_0x04(){
	int8 imm8=RMEM8(R_IP+1);
	AL+=imm8;
	IP+=2;
#ifdef SHOW_DEBUG
	printf("ADD AL, %X\n", imm8);
#endif
	return 0;
}

/* ADD AX, imm16 */
int op_0x05(){
	int16 imm16=RMEM16(R_IP+1);
	AL+=imm16;
	IP+=3;
#ifdef SHOW_DEBUG
	printf("ADD AX, %X", imm16);
#endif
	return 0;
}

/* ADD r/m8, imm8 */
int op_0x80(){
	int8 imm8=RMEM8(R_IP+2);
	int8 rm8;
	MOD_REG_RM(1);
	rm8=read_rm_val_8(op_data.ds);
	rm8+=imm8;
	write_rm_val_8(rm8, op_data.ds);
	IP+=3;
#ifdef SHOW_DEBUG
	printf("ADD %s, %X\n", print_rm_val_8(op_data.ds), imm8);
#endif
	return 0;
}
