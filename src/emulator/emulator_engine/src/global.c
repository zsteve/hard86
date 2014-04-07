#include "global.h"

unsigned short signext8(unsigned char a){
	if(a & 0x80){
		/* negative */
		return (unsigned short)a | 0xff00;
	}
	return a;
}

unsigned long signext16(unsigned short a){
	if(a & 0x8000){
		return (unsigned long)a | 0xffff0000;
	}
	return a;
}