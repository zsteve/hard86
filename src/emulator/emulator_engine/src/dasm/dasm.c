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

#include <stdlib.h>
#include <string.h>

#include "dasm.h"
#include "../emulator_engine.h"
#include "../emulator_engine_interface.h"
#include "../../../../system/multithreading/mutex/c/cmutex.h"
#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"
#include "../../../../system/datastruct/clist/clist.h"

/* extern stuff from main emulator engine */

extern int execute_flag;
extern op_func op_table[256];

extern sys_state_type sys_state;
extern op_data_type op_data;
extern void(*out_opinfo_ptr)(char* str, va_list* v);

extern void reset_op_data();

static clist dasm_list={ 0, 0, 0 };

void dasm_print_op(char* str, va_list* v){
	static char dasm_out[80];
	static int i=0;

	dasm_list_entry* e=(dasm_list_entry*)malloc(sizeof(dasm_list_entry));

	vsprintf(dasm_out, str, *v);
	/* must be freed upon list destroy */
	e->line=(char*)malloc(sizeof(char)*80);
	e->line[0]=NULL;

	if(op_data.seg_ovr){
		switch(op_data.seg_ovr){
		case 0x2e:
			strcat(e->line, "CS: ");
			break;
		case 0x36:
			strcat(e->line, "SS: ");
			break;
		case 0x3e:
			strcat(e->line, "DS: ");
			break;
		case 0x26:
			strcat(e->line, "ES: ");
			break;
		}
	}
	if(op_data.rep){
		strcat(e->line, "REP ");
	}
	else if(op_data.repne){
		strcat(e->line, "REPNE ");
	}

	strcat(e->line, dasm_out);

	e->seg=CS;
	e->addr=IP;
	i++;
	clist_push_back(&dasm_list, (void*)e);
}

/**
	disassembles system memory from given IP value
	assumes system MUTEX is UNLOCKED.
	@param n_instr number of instructions to disassemble
	@param initial_ip initial ip value
**/
clist dasm_disassemble(int n_instr, uint16 initial_ip, uint16 initial_cs){
	sys_state_type tmp_sys_state;
	op_data_type tmp_op_data;
	clist ret_list;

	int i=0;

	mutex_lock(sys_state.sys_mutex);

	execute_flag=0;

	tmp_sys_state=sys_state;
	tmp_op_data=op_data;

	/* redirect output */
	out_opinfo_ptr=&dasm_print_op;


	/* clear list */
	{
		clist_node* e=dasm_list.begin;
		while(e!=0){
			dasm_list_entry* d=e->data;
			free(d->line);
			e=e->next;
		}
	}
	clist_destroy(&dasm_list);
	dasm_list=clist_create();

	/* set up IP and CS */
	IP=initial_ip;
	CS=initial_cs;

	while(i<n_instr){
		reset_op_data();
		R_IP=GET_ADDR(IP, CS);
		process_instr_prefixes();
		(*op_table[read_mem_8(GET_ADDR(IP, CS))])();
		i++;
	}
	reset_op_data();
	out_opinfo_ptr=0;

	sys_state=tmp_sys_state;
	op_data=tmp_op_data;

	execute_flag=1;

	mutex_unlock(sys_state.sys_mutex);

	ret_list=dasm_list;
	return ret_list;
}