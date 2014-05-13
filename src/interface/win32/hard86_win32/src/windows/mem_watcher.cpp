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

#include "mem_watcher.h"
#include "../../../objwin32/src/gui/global.h"

#include "../emulator.h"

namespace nsHard86Win32{

bool MemoryWatcher::m_registered=false;

MemoryWatcher::MemoryWatcher() : Hard86ToolWindow(m_registered){
	m_style|=WS_SIZEBOX;
}

LRESULT CALLBACK MemoryWatcher::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
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
	case WM_SIZE:
		OnSize(hWnd, uMsg, wParam, lParam);
		break;
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
			Mutex sysMutex((void*)wParam);
			sysMutex.Lock();

			sys_state_ptr sysState=(sys_state_ptr)lParam;
			HexGrid* hexGrid=Child<HexGrid>(MEMGRID);
			vector<uint8>& gridData=hexGrid->GetData();
			int capacity=hexGrid->GetCapacity();
			gridData.clear();
			gridData.resize(capacity);
			uint32 baseAddr=(GetSeg()<<4)+GetAddr();
			for(int i=0; i<capacity; i++){
				gridData[i]=nsEmulator::Emulator::GetInstance()->ReadMem8(i+baseAddr);
			}
			sysMutex.Unlock();

			InvalidateRect(hexGrid->GetHWND(), NULL, false);
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

#define MSGHANDLER(name) void MemoryWatcher::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

MSGHANDLER(Create){
	CreateChildren(hWnd);
}

MSGHANDLER(Size){
	unsigned long w, h;
	w=LOWORD(lParam);
	h=HIWORD(lParam);
	unsigned long newlParam=(h-EditBox::DEF_H << 16) | (w-ScrollBar::DEF_W);
	Child<HexGrid>(MEMGRID)->SendMessage(WM_SIZE, wParam, newlParam);
	Child<HexGrid>(MEMGRID)->SetXY(0, EditBox::DEF_H);
	
	ScrollBar*& scrollBar=Child<ScrollBar>(SCROLLBAR);
	SetWindowSize(scrollBar->GetHWND(), ScrollBar::DEF_W, h-Button::DEF_H);
	SetWindowXY(scrollBar->GetHWND(), w-ScrollBar::DEF_W, EditBox::DEF_H);
	SetScrollRange(scrollBar->GetHWND(), SB_CTL, 0, nsEmulator::Emulator::MemSize-(Child<HexGrid>(MEMGRID)->GetCapacity()), false);
	SetScrollPos(scrollBar->GetHWND(), SB_CTL, 0, false);

	if(EmulatorThread::State()==EmulatorThread::Suspended){
		EmulatorThread::NotifyWindow(this);
	}
}

MSGHANDLER(Command){
	switch(LOWORD(wParam)){
	case GOTO_BUTTON:
		if(EmulatorThread::State()==EmulatorThread::Suspended){
			EmulatorThread::NotifyWindow(this);
		}
		break;
	}
}

MSGHANDLER(VScroll){

	using namespace nsEmulator;

	SCROLLINFO si;
	si.cbSize=sizeof(si);
	si.fMask=SIF_ALL;
	GetScrollInfo(Child<ScrollBar>(SCROLLBAR)->GetHWND(), SB_CTL, &si);

	HexGrid* hexGrid=Child<HexGrid>(MEMGRID);

	uint32 addr=(GetSeg()<<4)+GetAddr();
	int capacity=hexGrid->GetCapacity();
	switch(LOWORD(wParam)){
	case SB_LINEUP:
		if(addr > hexGrid->GetColumns()){
			addr-=hexGrid->GetColumns();
		}
		si.nPos=addr;
		break;
	case SB_LINEDOWN:
		if(addr < Emulator::MemSize-capacity-hexGrid->GetColumns()){
			addr+=hexGrid->GetColumns();
		}
		si.nPos=addr;
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		if(si.nTrackPos <= Emulator::MemSize-capacity){
			addr=si.nTrackPos;
		}
		si.nPos=si.nTrackPos;
		break;
	}
	
	SetAddr(addr & 0xffff);
	SetSeg((addr & 0xf0000) >> 4);
	si.fMask=SIF_POS;
	SetScrollInfo(Child<ScrollBar>(SCROLLBAR)->GetHWND(), SB_CTL, &si, TRUE);
	if(EmulatorThread::State()==EmulatorThread::Suspended){
		EmulatorThread::NotifyWindow(this);
	}
}

#undef MSGHANDLER

void MemoryWatcher::CreateChildren(HWND hWnd){
	int w, h;
	w=ClientWidth();
	h=ClientHeight();

	HexGrid*& memGrid=Child<HexGrid>(MEMGRID);
	memGrid=new HexGrid(false);		// Memory viewer wants its own scrollbar
	memGrid->Create(0, 0, w, h, hWnd, MEMGRID);
	SetWindowSize(memGrid->GetHWND(), w-ScrollBar::DEF_W, h);

	ScrollBar*& scrollBar=Child<ScrollBar>(SCROLLBAR);
	scrollBar=new ScrollBar();
	scrollBar->Create(w-ScrollBar::DEF_W, 0, ScrollBar::DEF_W, h-EditBox::DEF_H, hWnd, SCROLLBAR);
	scrollBar->SetY(EditBox::DEF_H);
	scrollBar->Show();

	SetScrollRange(scrollBar->GetHWND(), SB_CTL, 0, 65536-(memGrid->GetCapacity()), false);
	SetScrollPos(scrollBar->GetHWND(), SB_CTL, 0, false);

	EditBox*& segBox=Child<EditBox>(SEGBOX);
	segBox=new EditBox();
	segBox->Create(L"0000", 0, 0, (w-Button::DEF_W)/2, EditBox::DEF_H, hWnd, SEGBOX);
	segBox->SetFont(Settings::GetFont(Settings::Fonts::MONOSPACE_FONT));
	segBox->Show();

	EditBox*& addrBox=Child<EditBox>(ADDRBOX);
	addrBox=new EditBox();
	addrBox->Create(L"0000", (w-Button::DEF_W)/2, 0, (w-Button::DEF_W)/2, EditBox::DEF_H, hWnd, ADDRBOX);
	addrBox->SetFont(Settings::GetFont(Settings::Fonts::MONOSPACE_FONT));
	addrBox->Show();

	PushButton*& gotoButton=Child<PushButton>(GOTO_BUTTON);
	gotoButton=new PushButton();
	gotoButton->Create(L"Goto", w-Button::DEF_W, 0, Button::DEF_W, Button::DEF_H, hWnd, GOTO_BUTTON);
	gotoButton->SetFont(Settings::GetFont(Settings::Fonts::GUI_FONT));
	gotoButton->Show();
}

uint32 MemoryWatcher::GetAddr(){

	wstring strAddr=Child<EditBox>(ADDRBOX)->GetText();
	uint32 addr=wcstol(strAddr.c_str(), NULL, 16);
	return addr;
}

uint32 MemoryWatcher::GetSeg(){
	wstring strSeg=Child<EditBox>(SEGBOX)->GetText();
	uint32 seg=wcstol(strSeg.c_str(), NULL, 16);
	return seg;
}

void MemoryWatcher::SetAddr(uint32 i){
	wchar_t strAddr[9];
	_itow(i, strAddr, 16);
	Child<EditBox>(ADDRBOX)->SetText(strAddr);
}

void MemoryWatcher::SetSeg(uint32 i){
	wchar_t strSeg[9];
	_itow(i, strSeg, 16);
	Child<EditBox>(SEGBOX)->SetText(strSeg);
}

}
