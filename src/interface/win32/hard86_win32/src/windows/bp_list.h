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

#ifndef HARD86_BP_LIST_H
#define HARD86_BP_LIST_H

#include <Windows.h>
#include "../global.h"
#include "toolwindow.h"
#include "../../../objwin32/src/gui/listbox.h"

namespace nsHard86Win32{

	class BPList : public Hard86ToolWindow{
	public:
		BPList();

		virtual ~BPList(){
			DestroyWindow(m_hWnd);
		}

		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		HWND Create(int x, int y, int w, int h, HWND hwndParent, HMENU hMenu=NULL){
			Window::Create(m_exStyle, L"Breakpoints", m_style, x, y, w, h, hwndParent, hMenu);
			Show();
			return m_hWnd;
		}

		void Update();

	private:

#define MSGHANDLER(name) void On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

		MSGHANDLER(Create);
		MSGHANDLER(Command);
		MSGHANDLER(VScroll);

#undef MSGHANDLER

		void CreateChildren(HWND hWnd);

		enum Children{
			LISTBOX
		};

		WinManager m_children;

		template<typename T>
		T*& Child(int id){
			return (T*&)m_children[id];
		}

		static bool m_registered;
	};

}

#endif