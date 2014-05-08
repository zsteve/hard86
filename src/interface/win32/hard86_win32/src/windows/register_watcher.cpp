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

#include "register_watcher.h"
#include "../emulator.h"

namespace nsHard86Win32{

	// SingleRegWatcher
	INT_PTR CALLBACK SingleRegWatcher::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg){
	case WM_INITDIALOG:
		{
			// Create subclass of edit box function
			SetWindowSubclass(GetDlgItem(hWnd, IDC_REGVALUE), &EditSubclassProc, m_editSubclassID, NULL);
		}
		break;
	case WM_CLOSE:
		{
			RemoveWindowSubclass(GetDlgItem(hWnd, IDC_REGVALUE), &EditSubclassProc, m_editSubclassID);
		}
	case H86_UPDATE_SYS_DATA:
		{
			Mutex sysMutex((void*)wParam);
			sys_state_ptr sysState=(sys_state_ptr)lParam;

			sysMutex.Lock();

			nsEmulator::Emulator* e=nsEmulator::Emulator::GetInstance();
			uint16 num=e->ReadReg(m_regNum);
			wchar_t data[5];
			_itow((uint32)num, data, 16);
			SetDlgItemText(hWnd, IDC_REGVALUE, data);

			sysMutex.Unlock();
		}
		break;
	default:
		return 0;
	}
	return 0;
}

	bool RegisterWatcher::m_registered=false;

	const wchar_t RegisterWatcher::m_regNames[][6]=\
	{
		L"AX", L"CX", L"DX", L"BX", L"SP",
		L"BP", L"SI", L"DI", L"ES", L"CS",
		L"SS", L"DS", L"IP"
	};

	LRESULT CALLBACK RegisterWatcher::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
		case WM_CREATE:
			{
				m_dragManager.AddWindow(m_hWnd);
				OnCreate(hWnd, uMsg, wParam, lParam);
				break;
			}
		case WM_MOVING:
			{
				Hard86ToolWindow::OnMoving(hWnd, uMsg, wParam, lParam);
				break;
			}
		case WM_SIZING:
			{
				Hard86ToolWindow::OnSizing(hWnd, uMsg, wParam, lParam);
				break;
			}
		case WM_NCLBUTTONDOWN:
			{
				Hard86ToolWindow::OnNCLButtonDown(hWnd, uMsg, wParam, lParam);
				break;
			}
		case H86_UPDATE_SYS_DATA:
			{
				for(int i=REGWATCHER_AX; i<=REGWATCHER_IP; i++){
					SingleRegWatcher* w=m_regWatchers[i];
					w->SendMessage(H86_UPDATE_SYS_DATA, wParam, lParam);
				}
			}
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		return 0;
	}

	void RegisterWatcher::CreateChildren(HWND hWnd){
		int ypos=0;
		int height=0, width=0;
		for(int i=REGWATCHER_AX; i<=REGWATCHER_IP; i++){
			SingleRegWatcher* w=m_regWatchers[i]=new SingleRegWatcher(i);
			w->Create(hWnd);
			w->SetName(m_regNames[i]);
			SetWindowXY(w->GetHWND(), 0, ypos);
			height=w->WindowHeight();
			width=w->WindowWidth();
			ypos+=height;
		}
		SetWindowSize(hWnd, width, ypos+height);
	}

#define MSGHANDLER(name) void  RegisterWatcher::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Create){
		CreateChildren(hWnd);
	}

#undef MSGHANDLER
}
