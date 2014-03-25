/**
* @file debugger module
* Stephen Zhang, 2014
*/

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "vdev.h"
#include "../../emulator_engine/src/emulator_engine_interface.h"
#include "../../../system/multithreading/mutex/c/cmutex.h"

namespace nsDebugger{

	using namespace nsVDev;

	/**
	 * singleton Debugger class
	 */
	class Debugger{
	private:
		Debugger()
		{
		}
	public:
		/**
		 * must be called prior to any GetInstance() call
		 */
		static void Init(VDevList* vdevList);

		/**
		 * destroys instance, invalidating all references
		 */
		static void Reset();

		static Debugger* GetInstance();
		virtual ~Debugger(){}

		static void BreakPointHit(MUTEX sysMutex, sys_state_ptr sysState);
		static void PreInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState);
		static void PostInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState);

	protected:
		static VDevList* m_vdevList;
		static Debugger* m_instance;
	};
}

#endif