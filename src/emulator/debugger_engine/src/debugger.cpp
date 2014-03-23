/**
* @file debugger module
* Stephen Zhang, 2014
*/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <wchar.h>
#include <varargs.h>

#include <conio.h>

#include "debugger.h"
#include "../../emulator_engine/src/emulator_engine_interface.h"

#include "../../../system/multithreading/mutex/c/cmutex.h"

using namespace std;

VDevList* Debugger::m_vdevList;

/**
 * callback function for breakpoints
 * @param sysMutex system MUTEX
 */
void Debugger::BreakPointHit(MUTEX sysMutex, sys_state_ptr sysState){
	cout << "Breakpoint Hit" << endl;
	sys_state_ptr sys_state=get_system_state();
	cout << "Press any key to continue..." << endl;
	_getch();
}

/**
 * callback function before instruction executes
 * @param sysMutex system MUTEX
 */
void Debugger::PreInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState){
	cout << "PreInstructionExecute" << endl;
	sys_state_ptr sys_state=get_system_state();
}

/**
 * callback function after instruction executes
 * @param sysMutex system MUTEX
 */
void Debugger::PostInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState){
	cout << "PostInstructionExecute" << endl;
	sys_state_ptr sys_state=get_system_state();
	/* update virtual devices */
	if(!m_vdevList->empty()){
		VDevList::iterator it=m_vdevList->begin();
		for(it; it!=m_vdevList->end(); ++it){
			(*it).second.AcceptEmulationMutex(sysMutex, sysState);
		}
	}
}