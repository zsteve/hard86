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

#ifndef HARD86_STATUSBAR_H
#define HARD86_STATUSBAR_H

#include "../../../objwin32/src/gui/userwindow.h"
#include "winmanager.h"

namespace nsHard86Win32{

	using namespace nsObjWin32::nsGUI;

	class StatusBar : public UserWindow{
	public:
		static const int DEF_H=16;

		StatusBar(){
			m_style|=WS_CHILD;
			m_className=L"Hard86_StatusBar";
			if(!m_registered){
				Register();
				m_registered=true;
			}
		}

		virtual ~StatusBar(){

		}

		ATOM Register();

		HWND Create(int x, int y, int w, HWND hwndParent, int ID);

		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void SetStatus(wchar_t* status);
		void SetInfo(wchar_t* info);
	private:
		
		enum Children{
			STATUS, INFO
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