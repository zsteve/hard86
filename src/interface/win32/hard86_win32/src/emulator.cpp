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
		EmulatorThread* emuThread=GetInstance();

		emulator->Execute();

		emuThread->m_state=Suspended;
		emuThread->SysMutex().Unlock();
		emuThread->NotifyMsgWindow();

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

	// VDevDlg

	VDevDlg* VDevDlg::m_instance;

	INT_PTR CALLBACK VDevDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg){
		case WM_CLOSE:
			if(H86Project::HasInstance()){
				H86Project* project=H86Project::GetInstance();
				VDevList* vdevList=VDevList::GetInstance();
				project->Remove_VDevList();

				vector<pair<string, string> > vdevListVect(0);
				for(VDevList::iterator it=vdevList->begin();
					it!=vdevList->end();
					++it){
					vdevListVect.push_back(make_pair(wstrtostr((*it).second.GetFileName()), wstrtostr((*it).second.GetFileName())));
				}
				project->Add_VDevList(vdevListVect);
			}
			EndDialog(hWnd, WM_CLOSE);
			break;
		case WM_INITDIALOG:
			{
				VDevList* vdevList=VDevList::GetInstance();
				if(!vdevList->empty()){
					for(VDevList::iterator it=vdevList->begin();
						it!=vdevList->end();
						++it){
						::SendMessage(GetDlgItem(hWnd, IDC_VDEVLISTBOX), LB_ADDSTRING, NULL, (LPARAM)(*it).second.GetFileName().c_str());
					}
				}
			}
			break;
		case WM_DROPFILES:
			{
				if(!Emulator::HasInstance()) break;
				HDROP hDrop=(HDROP)wParam;
				int nFiles=DragQueryFile(hDrop, -1, NULL, NULL);
				
				vector<wstring> dropFiles(nFiles);

				for(int i=0; i<nFiles; i++){
					wchar_t fName[MAX_PATH]=L"";
					DragQueryFile(hDrop, i, fName, MAX_PATH);
					dropFiles.push_back(wstring(fName));
				}

				VDevList* vdevList=VDevList::GetInstance();
				for(vector<wstring>::iterator it=dropFiles.begin();
					it!=dropFiles.end();
					++it){
					// Load library
					HMODULE hMod=LoadLibrary(it->c_str());
					if(hMod){
						vdevList->Add(VDev((VDev::InitFunc)GetProcAddress(hMod, "VirtualDevice_Initialize"), \
							(VDev::TermFunc)GetProcAddress(hMod, "VirtualDevice_Terminate"), \
							(VDev::AcceptMutexFunc)GetProcAddress(hMod, "VirtualDevice_AcceptEmulationMutex"),
							*it));
						::SendMessage(GetDlgItem(hWnd, IDC_VDEVLISTBOX), LB_ADDSTRING, NULL, (LPARAM)(*it).c_str());
					}
				}
			}
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)){
			case IDC_REMOVE:
				{
					int item=SendDlgItemMessage(hWnd, IDC_VDEVLISTBOX, LB_GETCURSEL, NULL, NULL);
					if(item!=LB_ERR){
						VDevList* vdevList=VDevList::GetInstance();
						vdevList->Remove((*(vdevList->begin()+item)).first);
						SendDlgItemMessage(hWnd, IDC_VDEVLISTBOX, LB_DELETESTRING, (WPARAM)item, NULL);
					}
				}
				break;
			}
			break;
		default:
			return 0;
		}
		return 0;
	}
}
