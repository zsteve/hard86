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

#include "dasmview.h"

namespace nsHard86Win32{

	bool DasmView::m_registered=false;

	DasmView::DasmView() : m_dasmList(0){

		m_style|=WS_CHILD;
		m_className=L"Hard86_DasmView";
		if(!m_registered){
			Register();
		}
		m_baseAddr=0;
		
	}

	DasmView::~DasmView(){

	}

	ATOM DasmView::Register(){
		WNDCLASSEX wcx;
		wcx.cbSize=sizeof(WNDCLASSEX);
		wcx.style=CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
		wcx.lpfnWndProc=&Base_WndProc;
		wcx.cbClsExtra=NULL;
		wcx.cbWndExtra=NULL;
		wcx.hInstance=m_hInstance;
		wcx.hIcon=NULL;
		wcx.hCursor=(HCURSOR)LoadCursor(m_hInstance, IDC_ARROW);;
		wcx.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
		wcx.lpszMenuName=NULL;
		wcx.lpszClassName=m_className;
		wcx.hIconSm=NULL;
		return (RegisterClassEx(&wcx));
	}

	HWND DasmView::Create(int x, int y, int w, int h, HWND hwndParent, int ID)
	{
		return Window::Create(m_exStyle, L"", m_style, x, y, w, h, hwndParent, (HMENU)ID);
	}

	LRESULT CALLBACK DasmView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg){
		case WM_CREATE:
			OnCreate(hWnd, uMsg, wParam, lParam);
			break;
		case WM_VSCROLL:
			OnVScroll(hWnd, uMsg, wParam, lParam);
		case WM_SIZING:
			OnSizing(hWnd, uMsg, wParam, lParam);
		case H86_UPDATE_SYS_DATA:
			OnH86UpdateSysData(hWnd, uMsg, wParam, lParam);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		return 0;
	}

#define MSGHANDLER(name) void DasmView::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Create){
		CreateChildren(hWnd);
	}

	MSGHANDLER(H86UpdateSysData){
		using namespace nsDasm;
		using namespace nsEmulator;

		// TODO
		// Upon loading of a new file, all data should be refreshed
		// Current code is not doing this!

		Mutex sysMutex((void*)wParam);

		Emulator* emulator=Emulator::GetInstance();
		if(!sysMutex.TryLock()){
			OUT_DEBUG("Failed to lock sysMutex");
			return;	// if locking fails, return anyway - don't worry about updating data.
		}

		uint32 ip=emulator->ReadReg(Regs::ip);
		uint32 cs=emulator->ReadReg(Regs::cs);
		uint32 addr=(cs<<4)+ip;

		{

			for(DasmList::iterator it=m_dasmList.begin();
				it!=m_dasmList.end();
				++it){
				if((*it)->GetExtAddr()==addr){
					// Found another entry with the current IP. Don't update m_dasmList
					CodeList* codeList=Child<CodeList>(CODELIST);
					codeList->SetExtraSelection(it-m_dasmList.begin());
					sysMutex.Unlock();
					return;
				}
			}
		}
		Disassembler dasm;

		m_dasmList=dasm.Disassemble(Child<CodeList>(CODELIST)->GetCapacity(), ip, cs, EmulatorThread::GetSymData());
		Child<CodeList>(CODELIST)->Clear();
		Child<CodeList>(CODELIST)->GetSelectionList().clear();

		for(DasmList::iterator it=m_dasmList.begin();
			it!=m_dasmList.end();
			++it){
			Child<CodeList>(CODELIST)->Insert(make_pair(DWORD_B(it->GetSeg(), it->GetAddr()), strtowstr(it->GetCStr())));

		}
		Child<CodeList>(CODELIST)->SetExtraSelection(0);


		sysMutex.Unlock();
		InvalidateRect(hWnd, NULL, false);
	}

	MSGHANDLER(VScroll){
		/*SCROLLINFO si;
		si.cbSize=sizeof(si);
		si.fMask=SIF_ALL;
		ScrollBar* scrollBar=Child<ScrollBar>(SCROLLBAR);

		GetScrollInfo(scrollBar->GetHWND(), SB_CTL, &si);
		switch(LOWORD(wParam)){
		case SB_LINEUP:
			if(m_baseAddr > 0)
				m_baseAddr--;
			si.nPos=m_baseAddr;
			break;
		case SB_LINEDOWN:
			if(m_baseAddr < nsEmulator::Emulator::MemSize){
				m_baseAddr++;
			}
			si.nPos=m_baseAddr;
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:

			break;
		}
		si.fMask=SIF_POS;
		SetScrollInfo(scrollBar->GetHWND(), SB_CTL, &si, TRUE);
		if(EmulatorThread::State()==EmulatorThread::Suspended){
			EmulatorThread::NotifyWindow(this);
		}*/
	}

	MSGHANDLER(Sizing){
		Child<CodeList>(CODELIST)->SendMessage(WM_SIZING, wParam, lParam);
	}

#undef MSGHANDLER

	void DasmView::CreateChildren(HWND hWnd){
		int w, h;
		w=ClientWidth();
		h=ClientHeight();

		CodeList*& codeList=Child<CodeList>(CODELIST);
		codeList=new CodeList(CodeList::LEFT, false);
		codeList->Create(0, 0, w, h, hWnd, CODELIST);
		codeList->SetFont(Settings::GetFont(Settings::Fonts::MONOSPACE_FONT));
		
		// Disassembly not loaded, so we inform the user
		Reset();

		codeList->Show();

		/*ScrollBar*& scrollBar=Child<ScrollBar>(SCROLLBAR);
		scrollBar=new ScrollBar();
		scrollBar->Create(w-ScrollBar::DEF_W, 0, ScrollBar::DEF_W, h, hWnd, SCROLLBAR);
		scrollBar->Show();
		SetScrollRange(scrollBar->GetHWND(), SB_CTL, 0, nsEmulator::Emulator::MemSize-1, false);*/
	}

}
