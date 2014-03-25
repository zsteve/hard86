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

namespace nsDebugger{

	using namespace nsVDev;

	VDevList* Debugger::m_vdevList=NULL;
	Debugger* Debugger::m_instance=NULL;

	Debugger* Debugger::GetInstance(){
		if(!m_instance){
			m_instance=new Debugger;
		}
		return m_instance;
	}

	void Debugger::Init(VDevList* vdevList){
		m_vdevList=vdevList;
		// start initializing
		if(!m_vdevList->empty()){
			for(VDevList::iterator it=m_vdevList->begin();
				it!=m_vdevList->end();
				++it){
				(*it).second.Initialize((*it).second.GetParams().first, (*it).second.GetParams().second);
			}
		}
	}

	void Debugger::Reset(){
		if(m_instance){
			delete m_instance;
		}
		m_instance=NULL;
	}

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

}