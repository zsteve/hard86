/**
* @file debugger module
* Stephen Zhang, 2014
*/

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "../../../system/multithreading/mutex/c/mutex.h"

void BreakPointHit(MUTEX sys_mutex);
void PreInstructionExecute(MUTEX sys_mutex);
void PostInstructionExecute(MUTEX sys_mutex);

#endif