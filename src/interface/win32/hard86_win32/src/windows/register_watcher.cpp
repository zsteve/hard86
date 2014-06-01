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
			break;
		}
		case WM_ENABLE:
			EnableWindow(GetDlgItem(hWnd, IDC_REGVALUE), (BOOL)wParam);
			break;
		case H86_UPDATE_SYS_DATA:
		{
			Mutex sysMutex((void*)wParam);
			sys_state_ptr sysState=(sys_state_ptr)lParam;

			sysMutex.Lock();

			nsEmulator::Emulator* e=nsEmulator::Emulator::GetInstance();
			uint16 val=e->ReadReg(m_regNum);
			wchar_t data[5];
			_itow((uint32)val, data, 16);
			SetDlgItemText(hWnd, IDC_REGVALUE, data);

			sysMutex.Unlock();
		}
			break;
		case H86_USER_INPUT:
		{
			sys_state_ptr sysState=(sys_state_ptr)lParam;

			nsEmulator::Emulator* e=nsEmulator::Emulator::GetInstance();
			wchar_t data[5];
			GetDlgItemText(hWnd, IDC_REGVALUE, data, 5);
			uint16 val=wcstol(data, NULL, 16);
			e->WriteReg(m_regNum, val);
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
		case WM_CLOSE:
			Hard86ToolWindow::OnClose(hWnd, uMsg, wParam, lParam);
			break;
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
		case WM_ENABLE:
			for(int i=REGWATCHER_AX; i<=REGWATCHER_IP; i++){
				SingleRegWatcher* w=m_regWatchers[i];
				EnableWindow(w->GetHWND(), (BOOL)wParam);
			}
			m_enabled=(bool)wParam;
			break;
		case H86_UPDATE_SYS_DATA:
		{
			if(!m_enabled) break;
			for(int i=REGWATCHER_AX; i<=REGWATCHER_IP; i++){
				SingleRegWatcher* w=m_regWatchers[i];
				w->SendMessage(H86_UPDATE_SYS_DATA, wParam, lParam);
			}
		}
			break;
		case H86_USER_INPUT:
		{
			if(!m_enabled) break;
			for(int i=REGWATCHER_AX; i<=REGWATCHER_IP; i++){
				SingleRegWatcher* w=m_regWatchers[i];
				w->SendMessage(H86_USER_INPUT, wParam, lParam);
			}
		}
			break;
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

	// FlagWatcherDlg
	INT_PTR CALLBACK FlagWatcherDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
		case WM_INITDIALOG:
		{
			break;
		}
		case WM_CLOSE:
		{
			EndDialog(hWnd, WM_QUIT);
			break;
		}
		case WM_ENABLE:
			EnableWindow(GetDlgItem(hWnd, IDC_SF), (BOOL)wParam);
			EnableWindow(GetDlgItem(hWnd, IDC_ZF), (BOOL)wParam);
			EnableWindow(GetDlgItem(hWnd, IDC_AF), (BOOL)wParam);
			EnableWindow(GetDlgItem(hWnd, IDC_PF), (BOOL)wParam);
			EnableWindow(GetDlgItem(hWnd, IDC_CF), (BOOL)wParam);
			break;
		case H86_UPDATE_SYS_DATA:
		{
			sys_state_ptr sysState=(sys_state_ptr)lParam;
			// Set flags
			Button_SetCheck(GetDlgItem(hWnd, IDC_SF), sysState->f_bits.SF);
			Button_SetCheck(GetDlgItem(hWnd, IDC_ZF), sysState->f_bits.ZF);
			Button_SetCheck(GetDlgItem(hWnd, IDC_AF), sysState->f_bits.AF);
			Button_SetCheck(GetDlgItem(hWnd, IDC_PF), sysState->f_bits.PF);
			Button_SetCheck(GetDlgItem(hWnd, IDC_CF), sysState->f_bits.CF);
		}
			break;
		case H86_USER_INPUT:
		{
			sys_state_ptr sysState=(sys_state_ptr)lParam;
			sysState->f_bits.SF=Button_GetCheck(GetDlgItem(hWnd, IDC_SF));
			sysState->f_bits.ZF=Button_GetCheck(GetDlgItem(hWnd, IDC_ZF));
			sysState->f_bits.AF=Button_GetCheck(GetDlgItem(hWnd, IDC_AF));
			sysState->f_bits.PF=Button_GetCheck(GetDlgItem(hWnd, IDC_PF));
			sysState->f_bits.CF=Button_GetCheck(GetDlgItem(hWnd, IDC_CF));
		}
			break;
		default:
			return 0;
		}
		return 0;
	}

	// FlagWatcher

	bool FlagWatcher::m_registered=false;

	LRESULT CALLBACK FlagWatcher::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		switch(uMsg){
			case WM_CREATE:
			{
				m_dragManager.AddWindow(m_hWnd);
				OnCreate(hWnd, uMsg, wParam, lParam);
				break;
			}
			case WM_CLOSE:
				Hard86ToolWindow::OnClose(hWnd, uMsg, wParam, lParam);
				break;
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
			case WM_ENABLE:
				m_flagWatcherDlg.SendMessage(uMsg, wParam, lParam);
				break;
			case H86_UPDATE_SYS_DATA:
			{
				m_flagWatcherDlg.SendMessage(uMsg, wParam, lParam);
				break;
			}
			case H86_USER_INPUT:
			{
				m_flagWatcherDlg.SendMessage(uMsg, wParam, lParam);
				break;
			}
			default:
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		return 0;
	}

	void FlagWatcher::CreateChildren(HWND hWnd){
		m_flagWatcherDlg.Create(hWnd);
		SetSize(m_flagWatcherDlg.WindowWidth()+(WindowWidth()-ClientWidth()), 
				m_flagWatcherDlg.WindowHeight()+(WindowHeight()-ClientHeight()));
	}

#define MSGHANDLER(name) void FlagWatcher::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Create){
		CreateChildren(hWnd);
	}

#undef MSGHANDLER

}
