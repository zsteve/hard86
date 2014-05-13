#ifndef __MAIN_H__
#define __MAIN_H__

#include <windows.h>
#include <stdio.h>
#include "cmutex.h"

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef unsigned char uint8;
typedef unsigned long uint32;
typedef unsigned short uint16;

typedef struct{
	/* system memory */
	uint8* mem;
	uint32 mem_size;

	/* I/O bus */
	/*	The I/O address space consists of 2 16 (64K)
	individually addressable 8-bit I/O ports,
	numbered 0 through FFFFH
	*/
	uint8* io_bus;
	/* registers */
	/*	general purpose
	AX, BX, CX, DX

	we store this as an array of uint8
	so we can easily access h.o byte and l.o byte
	*/
	uint8 ax[2];
	uint8 bx[2];
	uint8 cx[2];
	uint8 dx[2];

	uint16 sp;
	uint16 bp;
	uint16 si;
	uint16 di;

	/* special */
	/* segment registers */
	uint16 cs;
	uint16 ss;
	uint16 ds;
	uint16 es;

	/* pointers & indicators */
	uint16 ip;
	uint32 r_ip;	/*	this is the 'real' ip value
					that is, not segmented	*/

	/*	flags register
	format :
	Bit# : 15	14	13/12	11	10	9	8	7	6	5	4	3	2	1	0
	Flag : 0	NT	IOPL	OF	DF	IF	TF	SF	ZF	0	AF	0	PF	1	CF
	*/
	union{
		struct{
			uint8 CF : 1;
			uint8 reserved_1 : 1;	/* always 1 */
			uint8 PF : 1;
			uint8 reserved_2 : 1; /* always 0 */
			uint8 AF : 1;
			uint8 reserved_3 : 1;	/* always 0 */
			uint8 ZF : 1;
			uint8 SF : 1;
			uint8 TF : 1;
			uint8 IF : 1;
			uint8 DF : 1;
			uint8 OF : 1;
			uint8 IOPL : 2;
			uint8 NT : 1;
			uint8 reserved_4 : 1;	/* 1 on 8086, 0 above */
		}f_bits;
		uint16 flags;
	};

}sys_state_type, *sys_state_ptr;

void DLL_EXPORT SomeFunction(const LPCSTR sometext);
int DLL_EXPORT VirtualDevice_AcceptEmulationMutex(MUTEX a, sys_state_ptr b);
int DLL_EXPORT VirtualDevice_Terminate();
int DLL_EXPORT VirtualDevice_Initialize(void* a, void* b);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
