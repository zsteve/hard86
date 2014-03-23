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

#include "../../../system/multithreading/mutex/c/mutex.h"

using namespace std;

/**
 * callback function for breakpoints
 * @param sys_mutex system mutex
 */
void BreakPointHit(MUTEX sys_mutex){
	cout << "Breakpoint Hit" << endl;
	sys_state_ptr sys_state=get_system_state();
	cout << "Press any key to continue..." << endl;
	_getch();
}

/**
 * callback function before instruction executes
 * @param sys_mutex system mutex
 */
void PreInstructionExecute(MUTEX sys_mutex){
	cout << "PreInstructionExecute" << endl;
	sys_state_ptr sys_state=get_system_state();
}

/**
 * callback function after instruction executes
 * @param sys_mutex system mutex
 */
void PostInstructionExecute(MUTEX sys_mutex){
	cout << "PostInstructionExecute" << endl;
	sys_state_ptr sys_state=get_system_state();
}