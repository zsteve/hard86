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

#include "flags.h"
#include "emulator_engine_interface.h"
#include "emulator_engine.h"

extern sys_state_type sys_state;

/*	CPU flags are :
0	NT	IOPL	OF	DF	IF	TF	SF	ZF	0	AF	0	PF	1	CF

NT and IOPL flags are 80386+ only, so we don't need to worry about
them

Control Flags:
DF		Direction
IF		Interrupt flag
TF		Trap - Single step for debugging

Status Flags:
CF		Carry flag
PF		Parity flag
Set if the least-significant byte of the result contains an even number of 1 bits;
cleared otherwise.
AF		Adjust flag
Set if an arithmetic operation generates a carry or a borrow out of bit 3 of the
result; cleared otherwise. This flag is used in binary-coded decimal (BCD) arithmetic.
ZF		Zero flag
SF		Sign flag
Equal to most significant bit of value (1 if negative, 0 if positive)
OF		Overflow flag
*/

static uint8 parity[256]={
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00
};

void set_df(int v){
	FLAG_DF=v ? 1 : 0;
}

void set_if(int v){
	FLAG_IF=v ? 1 : 0;
}

void set_tf(int v){
	FLAG_TF=v ? 1 : 0;
}

void set_cf(int v){
	FLAG_CF=v ? 1 : 0;
}

void set_pf(uint32 v){
	FLAG_PF=parity[v & 255];
}

void set_af(){
	/** TODO **/
}

void set_zf(uint32 v){
	FLAG_ZF=v ? 0 : 1;
}

void set_sf(uint32 v){
	FLAG_SF=v;
}

void set_of(int v){
	FLAG_OF=v ? 1 : 0;
}

#define HAS_OVERFLOW(n, a, bits)\
	((uint32)n+a) >> bits ? 1 : 0

void setf_add16(uint16 a, uint16 b){
	set_of(HAS_OVERFLOW(a, b, 16));
	set_sf(((a+b) & BIT_15)>>15);
	set_zf(a+b);
	set_pf(a+b);
	set_cf((uint32)a+b >> 16 ? 1 : 0);
}

void setf_add8(uint8 a, uint8 b){
	set_of(HAS_OVERFLOW(a, b, 8));
	set_sf(((a+b) & 0x80)>>7);
	set_zf(a+b);
	set_pf(a+b);
	set_cf((uint16)a+b >> 8 ? 1 : 0);
}

void setf_adc16(uint16 a, uint16 b, uint16 c){
	setf_add16(a, b+c);
}

void setf_adc8(uint8 a, uint8 b, uint8 c){
	setf_add8(a, b+c);
}

/* a - b */
void setf_sub16(uint16 a, uint16 b){
	set_of(HAS_OVERFLOW(a, -b, 16));
	set_sf(((a-b) & BIT_15)>>15);
	set_zf(a-b);
	set_pf(a-b);
	set_cf(a < b);
}

void setf_sub8(uint8 a, uint8 b){
	set_of(HAS_OVERFLOW(a, -b, 8));
	set_sf(((a-b) & 0x80)>>7);
	set_zf(a-b);
	set_pf(a-b);
	set_cf(a < b);
}

void setf_mul8(uint8 a, uint8 b){
	uint16 res=a*b;
	if(HI_BYTE(res)){
		set_of(1);
		set_cf(1);
	}
	else{
		set_of(0);
		set_cf(0);
	}
}

void setf_mul16(uint16 a, uint16 b){
	uint32 res=a*b;
	if(HI_WORD(res)){
		set_of(1);
		set_cf(1);
	}
	else{
		set_of(0);
		set_cf(0);
	}
}

void setf_imul8(uint8 a, uint8 b){
	uint16 res=a*b;
	if(HI_BYTE(res)){
		set_of(1);
		set_cf(1);
	}
	else{
		set_of(0);
		set_cf(0);
	}
}

void setf_imul16(uint16 a, uint16 b){
	uint32 res=a*b;
	if(HI_WORD(res)){
		set_of(1);
		set_cf(1);
	}
	else{
		set_of(0);
		set_cf(0);
	}
}


/* a++ */
void setf_inc8(uint8 n){
	set_of(HAS_OVERFLOW(n, 1, 8));
	set_sf(((n+1) & 0x80)>>7);
	set_zf(n+1);
	set_pf(n+1);
}

void setf_inc16(uint16 n){
	set_of(HAS_OVERFLOW(n, 1, 16));
	set_sf(((n+1) & BIT_15)>>15);
	set_zf(n+1);
	set_pf(n+1);
}

/* a-- */
void setf_dec8(uint8 n){
	set_of(HAS_OVERFLOW(n, -1, 8));
	set_sf(((n-1) & 0x80)>>7);
	set_zf(n-1);
	set_pf(n-1);
}

void setf_dec16(uint16 n){
	set_of(HAS_OVERFLOW(n, -1, 16));
	set_sf(((n-1) & BIT_15)>>15);
	set_zf(n-1);
	set_pf(n-1);
}

void setf_neg8(uint8 n){
	set_of(HAS_OVERFLOW(0, -n, 8));
	set_sf(((-n) & 0x80)>>7);
	set_zf(-n);
	set_pf(-n);
	set_cf(0);
}

void setf_neg16(uint16 n){
	set_of(HAS_OVERFLOW(0, -n, 16));
	set_sf(((n-1) & BIT_15)>>15);
	set_zf(n-1);
	set_pf(n-1);
	set_cf(0);
}

void setf_shl8(uint8 n, uint8 s){
	if(s==1){
		// 1 bit shift
		if((((n<<1) & 0x80)>>8)==FLAG_CF){
			set_of(0);
		}
		else{
			set_of(1);
		}
	}
	set_sf(((n<<s) & 0x80)>>7);
	set_zf(n<<s);
	set_pf(n<<s);
	set_cf((((uint16)n) << s) >> 7);
}

void setf_shl16(uint16 n, uint16 s){
	if(s==1){
		// 1 bit shift
		if((((n<<1) & BIT_15)>>8)==FLAG_CF){
			set_of(0);
		}
		else{
			set_of(1);
		}
	}
	set_sf(((n<<s) & BIT_15)>>15);
	set_zf(n<<s);
	set_pf(n<<s);
	set_cf((((uint32)n) << s) >> 15);
}

void setf_shr8(uint8 n, uint8 s){
	set_of(n & BIT_7);
	set_sf(((n >> s) & 0x80) >> 7);
	set_zf(n>>s);
	set_pf(n>>s);
	set_cf((((uint16)n << 8)>>s) & BIT_7);
}

void setf_shr16(uint16 n, uint8 s){
	set_of(n & BIT_15);
	set_sf(((n >> s) & BIT_15) >> 15);
	set_zf(n>>s);
	set_pf(n>>s);
	set_cf((((uint32)n << 16)>>s) & BIT_15);
}

void setf_and8(uint8 a, uint8 b){
	set_of(0);
	set_sf((a & b) & BIT_7);
	set_zf(a&b);
	set_pf(a&b);
	set_cf(0);
}

void setf_and16(uint16 a, uint16 b){
	set_of(0);
	set_sf((a & b) & BIT_15);
	set_zf(a&b);
	set_pf(a&b);
	set_cf(0);
}

void setf_or8(uint8 a, uint8 b){
	set_of(0);
	set_sf((a | b) & BIT_7);
	set_zf(a|b);
	set_pf(a|b);
	set_cf(0);
}

void setf_or16(uint16 a, uint16 b){
	set_of(0);
	set_sf((a | b) & BIT_15);
	set_zf(a|b);
	set_pf(a|b);
	set_cf(0);
}

void setf_xor8(uint8 a, uint8 b){
	set_of(0);
	set_sf((a ^ b) & BIT_7);
	set_zf(a ^ b);
	set_pf(a ^ b);
	set_cf(0);
}

void setf_xor16(uint16 a, uint16 b){
	set_of(0);
	set_sf((a ^ b) & BIT_15);
	set_zf(a ^ b);
	set_pf(a ^ b);
	set_cf(0);
}

void setf_test8(uint8 a, uint8 b){
	set_zf(a & b);
	set_sf(a & b);
	set_pf(a & b);
}

void setf_test16(uint16 a, uint16 b){
	set_zf(a & b);
	set_sf(a & b);
	set_pf(a & b);
}

#undef HAS_OVERFLOW