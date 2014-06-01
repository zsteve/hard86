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

#include "toolwindow.h"

namespace nsHard86Win32{
	
#define MSGHANDLER(name) void  Hard86ToolWindow::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Sizing){
		StickyWindow::OnSizing(hWnd, uMsg, wParam, lParam);
		DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	MSGHANDLER(Moving){
		StickyWindow::OnMoving(hWnd, uMsg, wParam, lParam);
		DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	MSGHANDLER(NCLButtonDown){
		StickyWindow::OnNCLButtonDown(hWnd, uMsg, wParam, lParam);
		DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	MSGHANDLER(Close){
		Show(SW_HIDE);
	}

#undef MSGHANDLER

}