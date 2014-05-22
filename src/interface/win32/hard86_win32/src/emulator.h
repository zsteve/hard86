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

#ifndef HARD86_EMULATOR_H
#define HARD86_EMULATOR_H

#include "global.h"
#include "h86project.h"

#include "../../objwin32/src/gui/window.h"
#include "../../objwin32/src/gui/dialog.h"

#include "../../../../emulator/emulator_engine/src/cpp_interface/emulator_cpp.h"
#include "../../../../emulator/symbols_loader/src/sym_loader.h"
#include "../../../../emulator/debugger_engine/src/debugger.h"
#include "../../../../system/multithreading/thread/cpp/thread.h"
#include "../../../../system/multithreading/event/cpp/event.h"

#include "resource/resource.h"

namespace nsHard86Win32{

	using namespace nsEmulator;
	using namespace nsSymLoader;
	using namespace nsDebugger;

	class EmulatorThread{
	private:
		EmulatorThread() : m_thread(&EmulatorThreadProc, NULL)
		{
			m_state=Suspended;

			Debugger::GetInstance()->RegisterFrontendCallbacks(PreInstructionExecute,
					PostInstructionExecute,
					BreakPointHit);
			m_singleStep=false;
		
		}

		static EmulatorThread* m_instance;

		static bool m_breakpointHit;	// true if breakpoint was hit during last instruction
	public:

		~EmulatorThread(){

			m_msgWindow=NULL;

			try{
				Emulator* emulator=Emulator::GetInstance();

				emulator->Stop();

				m_sysMutex.Unlock();

				Resume();

				SetSingleStep(false);
				WaitForSingleObject(m_thread.GetHandle(), INFINITE);
			}
			catch(runtime_error e){
				m_thread.Kill();
			}
		}

		static EmulatorThread* GetInstance(){
			if(!m_instance){
				m_instance=new EmulatorThread();
			}
			return m_instance;
		}

		static void DisposeInstance(){
			if(m_instance){
				delete m_instance;
				m_instance=NULL;
			}
		}

		static void Start(){
			EmulatorThread* e=GetInstance();
			e->m_state=Running;
			e->m_sysMutex.Unlock();
			e->m_thread.Start();
		}

		static void Resume(){
			EmulatorThread* e=GetInstance();
			e->m_state=Running;
			e->m_sysMutex.Unlock();
			if(e->m_thread.State()==Thread::Suspended){
				e->m_thread.Start();
			}
		}

		static void Suspend(){
			EmulatorThread* e=GetInstance();
			e->m_state=Suspended;
			e->m_sysMutex.Lock();
		}

		static Mutex& SysMutex(){ return GetInstance()->m_sysMutex; }

		enum EmulatorState{
			Running, Suspended, Terminated
		};

		static EmulatorState State(){ return GetInstance()->m_state; }

		static void SetSingleStep(bool b){ GetInstance()->m_singleStep=b; }
		static bool IsSingleStep(){ return GetInstance()->m_singleStep; }

		static SymbolData& GetSymData(){ return GetInstance()->m_symData; }

		static void SetMsgWindow(nsObjWin32::nsGUI::Window* msgWindow){ GetInstance()->m_msgWindow=msgWindow; }

		static void NotifyMsgWindow(){
			return NotifyWindow(GetInstance()->m_msgWindow);
		}

		static void NotifyWindow(nsObjWin32::nsGUI::Window* window){
			if(!nsEmulator::Emulator::HasInstance()) return;
			if(!window) return;
			EmulatorThread* e=GetInstance();
			window->SendMessage(H86_UPDATE_SYS_DATA, 
								(WPARAM)e->m_sysMutex.GetHandle(),
								(LPARAM)nsEmulator::Emulator::GetInstance()->SystemState());
		}

	private:

		Thread m_thread;
		Mutex m_sysMutex;

		SymbolData m_symData;

		nsObjWin32::nsGUI::Window* m_msgWindow;	/// pointer to window to send H86_UPDATE_SYS_DATA message to

		static DWORD WINAPI EmulatorThreadProc(LPVOID lpParam);
		static void PreInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState);
		static void PostInstructionExecute(MUTEX sysMutex, sys_state_ptr sysState);
		static void BreakPointHit(MUTEX sysMutex, sys_state_ptr sysState);

		static EmulatorState m_state;

		static bool m_singleStep;

	};

	using namespace nsObjWin32::nsGUI;

	class VDevDlg : public Dialog{
	private:
		VDevDlg(){
			m_resId=IDD_VDEV;
		}

		virtual ~VDevDlg(){
			DestroyWindow(m_hWnd);
		}

		static VDevDlg* m_instance;
	public:

		static VDevDlg* GetInstance(){
			if(m_instance){
				delete m_instance;
			}
			m_instance=new VDevDlg();
			return m_instance;
		}

		INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
	};
}

#endif
