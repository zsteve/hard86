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

#ifndef HARD86_TOOLWINDOW_H
#define HARD86_TOOLWINDOW_H

#include "../../../objwin32/src/gui/userwindow.h"
#include "winmanager.h"

using namespace nsObjWin32::nsGUI;

namespace nsHard86Win32{
	class Hard86ToolWindow : public ToolWindow, public StickyWindow{
	public:
		Hard86ToolWindow(bool& registeredFlag){
			m_style|=WS_SYSMENU;
			m_className=L"Hard86_ToolWindow";
			if(!registeredFlag){
				Register();
				registeredFlag=true;
			}
		}
		virtual ~Hard86ToolWindow(){}

		virtual HWND Create(int x, int y, int w, int h, HWND hwndParent, HMENU hMenu=NULL)=0;

#define MSGHANDLER(name) void  On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

		MSGHANDLER(Sizing);
		MSGHANDLER(Moving);
		MSGHANDLER(NCLButtonDown);
		MSGHANDLER(Close);

#undef MSGHANDLER
	};
}

#endif