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

#include "statusbar.h"
#include "../settings.h"
#include "../../../objwin32/src/gui/static.h"

namespace nsHard86Win32{

	bool StatusBar::m_registered=false;

	ATOM StatusBar::Register(){
		WNDCLASSEX wcx;
		wcx.cbSize=sizeof(WNDCLASSEX);
		wcx.style=CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
		wcx.lpfnWndProc=&Base_WndProc;
		wcx.cbClsExtra=NULL;
		wcx.cbWndExtra=NULL;
		wcx.hInstance=m_hInstance;
		wcx.hIcon=NULL;
		wcx.hCursor=NULL;
		wcx.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
		wcx.lpszMenuName=NULL;
		wcx.lpszClassName=m_className;
		wcx.hIconSm=NULL;
		return (RegisterClassEx(&wcx));
	}

	HWND StatusBar::Create(int x, int y, int w, HWND hwndParent, int ID)
	{
		return Window::Create(m_exStyle, L"", m_style, x, y, w, DEF_H, hwndParent, (HMENU)ID);
	}

	LRESULT CALLBACK StatusBar::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg){
		case WM_CREATE:
			{
				TextStatic*& info=Child<TextStatic>(INFO);
				info=new TextStatic();
				info->SetExStyle(WS_EX_CLIENTEDGE);
				info->Create(L"[Info]", 0, 0, ClientWidth()/2, ClientHeight(), hWnd, INFO);
				info->SetFont(Settings::GetFont(Settings::Fonts::GUI_FONT));
				info->Show();

				TextStatic*& status=Child<TextStatic>(STATUS);
				status=new TextStatic();
				status->SetStyle(SS_RIGHT);
				status->SetExStyle(WS_EX_CLIENTEDGE);
				status->Create(L"[Status]", ClientWidth()/2, 0, ClientWidth()/2, ClientHeight(), hWnd, STATUS);
				status->SetFont(Settings::GetFont(Settings::Fonts::GUI_FONT));
				status->Show();

				break;
			}
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		return 0;
	}

	void StatusBar::SetStatus(wchar_t* status){
		Child<TextStatic>(STATUS)->SetText(status);
	}

	void StatusBar::SetInfo(wchar_t* info){
		Child<TextStatic>(INFO)->SetText(info);
	}

}
