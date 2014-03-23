#include "global.h"

unsigned short signext8(unsigned char a){
	if(a & 80){
		/* negative */
		return (unsigned short)a & 0xff00;
	}
	return a;
}