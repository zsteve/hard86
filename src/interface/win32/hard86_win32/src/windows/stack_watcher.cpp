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

#include "stack_watcher.h"
#include "../emulator.h"
#include "../../../../../ext_itoa/ext_itoa.h"

namespace nsHard86Win32{

bool StackWatcher::m_registered=false;

StackWatcher::StackWatcher() : Hard86ToolWindow(m_registered){
	m_enabled=true;
}

LRESULT CALLBACK StackWatcher::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
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
		EnableWindow(Child<CodeList>(CODELIST)->GetHWND(), (BOOL)wParam);
		m_enabled=(bool)wParam;
		break;
	case WM_COMMAND:
		OnCommand(hWnd, uMsg, wParam, lParam);
		break;
	case H86_UPDATE_SYS_DATA:
		{
			if(!m_enabled) break;
			sys_state_ptr sysState=(sys_state_ptr)lParam;

			CodeList* codeList=Child<CodeList>(CODELIST);
			int capacity=1000;
			uint16 sp=sysState->sp;
			codeList->Clear();

			int count=0;

			do{
				uint32 addr=(sysState->ss << 4) + sp;
				wstring data=ext_itow(Emulator::GetInstance()->ReadMem16(addr), 16, 4);
				codeList->Insert(make_pair((sysState->ss << 16) | sp, data));
				sp+=2;
			} while(sp!=0 && count++ < 150);

			InvalidateRect(codeList->GetHWND(), NULL, false);
		}
		break;
	case WM_VSCROLL:
		OnVScroll(hWnd, uMsg, wParam, lParam);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

#define MSGHANDLER(name) void StackWatcher::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

MSGHANDLER(Create){
	CodeList*& codeList=Child<CodeList>(CODELIST);
	codeList=new CodeList(CodeList::LEFT, true);
	codeList->Create(0, 0, ClientWidth(), ClientHeight(), hWnd, CODELIST);
	codeList->Insert(make_pair(NULL, wstring(L"No file loaded")));
	codeList->SetFont(Settings::GetFont(Settings::Fonts::MONOSPACE_FONT));
	codeList->Show();
}

MSGHANDLER(Command){

}

MSGHANDLER(VScroll){

}

#undef MSGHANDLER

}
