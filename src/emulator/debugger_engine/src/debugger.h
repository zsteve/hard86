/**
* @file debugger module
* Stephen Zhang, 2014
*/

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "vdev.h"
#include "../../emulator_engine/src/emulator_engine_interface.h"
#include "../../../system/multithreading/mutex/c/cmutex.h"

class Debugger{
public:
	Debugger(VDevList* vdevList)
	{
		m_vdevList=vdevList;
	}

	virtual ~Debugger(){}

	static void BreakPointHit(MUTEX sysMutex, sys_state_ptr sysState);
	static void PreInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState);
	static void PostInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState);

protected:
	static VDevList* m_vdevList;
};

#endif