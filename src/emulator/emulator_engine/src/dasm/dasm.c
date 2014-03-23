/**
* @file disassembler module
* Stephen Zhang, 2014
*/

#include <stdlib.h>
#include <string.h>

#include "dasm.h"
#include "../emulator_engine.h"
#include "../emulator_engine_interface.h"
#include "../../../../system/multithreading/mutex/c/mutex.h"
#include "../../../../global/defines.h"
#include "../../../../global/typedefs.h"
#include "../../../../system/datastruct/clist/clist.h"

/* extern stuff from main emulator engine */

extern int execute_flag;
extern op_func op_table[256];

extern sys_state_type sys_state;
extern op_data_type op_data;
extern MUTEX sys_mutex;
extern void(*out_opinfo_ptr)(char* str, va_list* v);

static clist dasm_list={ 0, 0, 0 };

void dasm_print_op(char* str, va_list* v){
	static char dasm_out[80];
	char* dasm_list_out;
	vsprintf(dasm_out, str, *v);
	/* must be freed upon list destroy */
	dasm_list_out=(char*)malloc(sizeof(char)*(strlen(dasm_out)+1));
	strcpy(dasm_list_out, dasm_out);
	clist_push_back(&dasm_list, dasm_list_out);
}

/**
	disassembles system memory from given IP value
	assumes system mutex is UNLOCKED.
	@param n_instr number of instructions to disassemble
	@param initial_ip initial ip value
**/
clist dasm_disassemble(int n_instr, uint16 initial_ip, uint16 initial_cs){
	sys_state_type tmp_sys_state;
	op_data_type tmp_op_data;
	clist ret_list;
	int i=0;

	mutex_lock(sys_mutex);

	tmp_sys_state=sys_state;
	tmp_op_data=op_data;

	/* redirect output */
	out_opinfo_ptr=&dasm_print_op;

	/* clear list */
	clist_destroy(&dasm_list);
	dasm_list=clist_create();

	/* set up IP and CS */
	IP=initial_ip;
	CS=initial_cs;

	while(i<n_instr){
		R_IP=GET_ADDR(IP, CS);
		
		(*op_table[read_mem_8(GET_ADDR(IP, CS))])();
		i++;
	}
	out_opinfo_ptr=0;

	sys_state=tmp_sys_state;
	op_data=tmp_op_data;

	mutex_unlock(sys_mutex);

	ret_list=dasm_list;
	dasm_list.begin=0;
	dasm_list.curr=0;
	dasm_list.end=0;
	return ret_list;
}