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

#include "emulator.h"

namespace nsHard86Win32{

	using namespace nsEmulator;

	EmulatorThread* EmulatorThread::m_instance=NULL;

	/// single step flag
	bool EmulatorThread::m_singleStep=false;

	EmulatorThread::EmulatorState EmulatorThread::m_state;
	
	/**
	 * The main thread of the emulator
	 * Expects that nsEmulator::Emulator has been
	 * Set up already (BIOS loaded, memory loaded, etc 
	 */
	DWORD WINAPI EmulatorThread::EmulatorThreadProc(LPVOID lpParam){
		
		Emulator* emulator=Emulator::GetInstance();

		emulator->Execute();

		return 0;
	}

	void EmulatorThread::PreInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState)
	{
		EmulatorThread& e=*GetInstance();
		if(m_singleStep || m_state==EmulatorState::Suspended){
			if(e.m_msgWindow)
				e.m_msgWindow->SendMessage(H86_UPDATE_SYS_DATA, (WPARAM)sysMutex, (LPARAM)sysState);
			else
				OUT_DEBUG("m_msgWindow is a null pointer, failed to send message");
		}
	}

	void EmulatorThread::PostInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState)
	{

	}

	void EmulatorThread::BreakPointHit(MUTEX sysMutex, sys_state_ptr sysState)
	{

	}
}
