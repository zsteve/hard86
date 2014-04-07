/**
* @file flag modifying routines
* x86 emulator engine for hard86
* Stephen Zhang, 2014
*/

#ifndef FLAGS_H
#define FLAGS_H

#include "emulator_engine.h"

#define FLAG_DF	sys_state.f_bits.DF
#define FLAG_IF	sys_state.f_bits.IF
#define FLAG_TF	sys_state.f_bits.TF
#define FLAG_CF	sys_state.f_bits.CF
#define FLAG_PF	sys_state.f_bits.PF
#define FLAG_AF	sys_state.f_bits.AF
#define FLAG_ZF	sys_state.f_bits.ZF
#define FLAG_SF	sys_state.f_bits.SF
#define FLAG_OF	sys_state.f_bits.OF

void set_df(int v);
void set_if(int v);
void set_cf(int v);
void set_pf(uint32 v);
void set_af();
void set_zf(uint32 v);
void set_sf8(uint8 v);
void set_sf16(uint16 v);
void set_of(int v);

#define SETF_2OP_PROTO(name, type)\
	void name(type a, type b);

#define SETF_3OP_PROTO(name, type)\
	void name(type a, type b, type c);

#define SETF_1OP_PROTO(name, type)\
	void name(type a);

SETF_2OP_PROTO(setf_add8, uint8)
SETF_2OP_PROTO(setf_add16, uint16)
SETF_3OP_PROTO(setf_adc8, uint8)
SETF_3OP_PROTO(setf_adc16, uint16)
SETF_2OP_PROTO(setf_sub8, uint8)
SETF_2OP_PROTO(setf_sub16, uint16)
SETF_1OP_PROTO(setf_inc8, uint8)
SETF_1OP_PROTO(setf_inc16, uint16)
SETF_1OP_PROTO(setf_dec8, uint8)
SETF_1OP_PROTO(setf_dec16, uint16)
SETF_1OP_PROTO(setf_neg8, uint8);
SETF_1OP_PROTO(setf_neg16, uint16);
SETF_2OP_PROTO(setf_shl8, uint8);
SETF_2OP_PROTO(setf_shl16, uint16);
SETF_2OP_PROTO(setf_shr8, uint8);
SETF_2OP_PROTO(setf_shr16, uint16);
SETF_2OP_PROTO(setf_and8, uint8);
SETF_2OP_PROTO(setf_and16, uint16);
SETF_2OP_PROTO(setf_or8, uint8);
SETF_2OP_PROTO(setf_or16, uint16);
SETF_2OP_PROTO(setf_xor8, uint8);
SETF_2OP_PROTO(setf_xor16, uint16);
SETF_2OP_PROTO(setf_mul8, uint8);
SETF_2OP_PROTO(setf_mul16, uint16);

#endif