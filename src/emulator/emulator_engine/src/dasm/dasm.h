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