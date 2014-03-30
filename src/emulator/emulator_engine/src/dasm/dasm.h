/**
* @file disassembler module
* Stephen Zhang, 2014
*/

#ifndef DASM_H
#define DASM_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../../../../system/datastruct/clist/clist.h"
#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"

typedef struct{
	char* line;	/// line data
	uint32 addr;	/// address (IP)
	uint32 seg;		/// segment (CS)
}dasm_list_entry;

clist dasm_disassemble(int n_instr, uint16 initial_ip, uint16 initial_cs);

#ifdef __cplusplus
}
#endif

#endif