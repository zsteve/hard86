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

#define M_IP sys_state.m_ip

/* JMP rel8 */
int op_0xeb(){
	uint8 rel8=*M_IP;
	IP+=rel8;
#ifdef SHOW_DEBUG
	printf("JMP %X\n", rel8);
#endif
	return 0;
}

/* MOV+ r16, imm16 */
int op_0xb8(){
	uint8 r16, imm16;
	MOD_REG_RM(1);
	r16=op_data.rm;
	/* imm16 is in lower 3 bits of instruction */
	imm16=WORD_B(*(M_IP+2), *(M_IP+1));
#ifdef SHOW_DEBUG
	
#endif
	return 0;
}