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

#include "bp_list.h"
#include "../emulator.h"
#include "../../../../../ext_itoa/ext_itoa.h"
#include "../../../../../emulator/debugger_engine/src/debugger.h"
#include "../settings.h"

namespace nsHard86Win32{

	using namespace nsDebugger;

	bool BPList::m_registered=false;

	BPList::BPList() : Hard86ToolWindow(m_registered){

	}

	LRESULT CALLBACK BPList::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
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
	case WM_COMMAND:
		OnCommand(hWnd, uMsg, wParam, lParam);
		break;
	case H86_UPDATE_SYS_DATA:
		{

		}
		break;
	case WM_VSCROLL:
		OnVScroll(hWnd, uMsg, wParam, lParam);
		break;
	case WM_DESTROY:
		{
			if(H86Project::HasInstance()){
				H86Project* project=H86Project::GetInstance();
				project->Remove_BPList();

				Debugger* debugger=Debugger::GetInstance();

				vector<pair<uint16, uint16> > bpVect(0);
				for(BreakpointList::iterator it=debugger->BreakpointBegin();
					it!=debugger->BreakpointEnd();
					++it){
					bpVect.push_back(make_pair((uint16)HI_WORD(it->second.Addr()), (uint16)LO_WORD(it->second.Addr())));
				}
				project->Add_BPList(bpVect);
			}
		}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}


#define MSGHANDLER(name) void BPList::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Create){
		ListBox*& listBox=Child<ListBox>(LISTBOX);
		listBox=new ListBox();
		listBox->Create(L"", 0, 0, ClientWidth(), ClientHeight(), hWnd, LISTBOX);
		listBox->SetFont(Settings::GetFont(Settings::Fonts::MONOSPACE_FONT));
		listBox->Show();
		Update();
	}

	MSGHANDLER(Command){
		switch(LOWORD(wParam)){
		case LISTBOX:
			switch(HIWORD(wParam)){
			case LBN_DBLCLK:

				break;
			}
			break;
		default:
			break;
		}
	}

	MSGHANDLER(VScroll){

	}

#undef MSGHANDLER

	void BPList::Update(){
		ListBox* listBox=Child<ListBox>(LISTBOX);
		listBox->clear();
		for(BreakpointList::iterator it=Debugger::GetInstance()->BreakpointBegin();
			it!=Debugger::GetInstance()->BreakpointEnd();
			++it){
			listBox->push_back(ext_itow(HIWORD(it->second.Addr()), 16, 5)+L":"+ext_itow(LOWORD(it->second.Addr()), 16, 5));
		}
		listBox->Update();
	}


}
