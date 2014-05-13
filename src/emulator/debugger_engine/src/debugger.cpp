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

	DBGCALLBACK Debugger::m_frontendPreInstructionExecute;
	DBGCALLBACK Debugger::m_frontendPostInstructionExecute;
	DBGCALLBACK Debugger::m_frontendBreakPointHit;

	Debugger* Debugger::GetInstance(){
		if(!m_instance){
			m_instance=new Debugger;
		}
		return m_instance;
	}

	void Debugger::Init(VDevList* vdevList){
		m_vdevList=vdevList;
	}

	void Debugger::Reset(){
		if(m_instance){
			delete m_instance;
		}
		m_instance=NULL;
	}

	void Debugger::InitVDevs(){
		// start initializing
		if(!m_vdevList->empty()){
			for(VDevList::iterator it=m_vdevList->begin();
				it!=m_vdevList->end();
				++it){
				(*it).second.Initialize((*it).second.GetParams().first, (*it).second.GetParams().second);
			}
		}
	}

	/**
	 * callback function for breakpoints
	 * @param sysMutex system MUTEX
	 */
	void Debugger::BreakPointHit(MUTEX sysMutex, sys_state_ptr sysState){

	}

	/**
	 * callback function before instruction executes
	 * @param sysMutex system MUTEX
	 */
	void Debugger::PreInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState){
		sys_state_ptr sys_state=get_system_state();

		m_frontendPreInstructionExecute(sysMutex, sysState);

		/* update virtual devices */
		if(!m_vdevList->empty()){
			VDevList::iterator it=m_vdevList->begin();
			for(it; it!=m_vdevList->end(); ++it){
				(*it).second.AcceptEmulationMutex(sysMutex, sysState);
			}
		}
	}

	/**
	 * callback function after instruction executes
	 * @param sysMutex system MUTEX
	 */
	void Debugger::PostInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState){
		
	}

	/**
	 * @return true if already existing (overwritten)
	 * @return false if not already existing
	 */
	bool Debugger::AddBreakpoint(uint32 addr){
		bool exists=m_bpList.Exists(addr);
		m_bpList[addr]=BreakpointList::Breakpoint(addr);
		return exists;
	}

	/**
	 * @return true for success
	 * @return false for failure
	 */
	bool Debugger::RemoveBreakpoint(uint32 addr){
		if(!m_bpList.Exists(addr)) return false;
		m_bpList.erase(addr);
		return true;
	}

	bool Debugger::ActivateBreakpoint(uint32 addr){
		if(!m_bpList.Exists(addr)) return false;
		m_bpList[addr].Activate();
		return true;
	}

	bool Debugger::DeactivateBreakpoint(uint32 addr){
		if(!m_bpList.Exists(addr)) return false;
		m_bpList[addr].Deactivate();
		return true;
	}

	bool Debugger::BreakpointExists(uint32 addr){
		return m_bpList.Exists(addr);
	}

	void Debugger::ClearBreakpoints(){
		m_bpList.clear();
	}

	bool Debugger::HasBreakpoints(){
		return !m_bpList.empty();
	}


	BreakpointList::BreakpointList(){

	}

	BreakpointList::BreakpointList(const BreakpointList& src){
		m_bpMap =src.m_bpMap;
	}

	BreakpointList::~BreakpointList(){

	}

	void Debugger::RegisterFrontendCallbacks(DBGCALLBACK preInstructionExecute,
											DBGCALLBACK postInstructionExecute,
											DBGCALLBACK breakPointHit)
	{
		m_frontendPreInstructionExecute=preInstructionExecute;
		m_frontendPostInstructionExecute=postInstructionExecute;
		m_frontendBreakPointHit=breakPointHit;
	}
}

