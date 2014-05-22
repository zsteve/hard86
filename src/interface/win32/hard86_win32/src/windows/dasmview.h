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

#ifndef HARD86_DASMVIEW_H
#define HARD86_DASMVIEW_H

#include <Windows.h>
#include "codelist.h"
#include "winmanager.h"
#include "../../../objwin32/src/gui/scrollbar.h"
#include "../../../objwin32/src/gui/userwindow.h"

#include "../emulator.h"

#include "../../../../../emulator/emulator_engine/src/cpp_interface/emulator_cpp.h"
#include "../../../../../emulator/debugger_engine/src/debugger.h"
#include "../../../../../emulator/emulator_engine/src/dasm/dasm_cpp.h"

namespace nsHard86Win32{

	class DasmView : public UserWindow{
	public:
		DasmView();
		virtual ~DasmView();

		ATOM Register();

		HWND Create(int x, int y, int w, int h, HWND hwndParent, int ID);

		LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void Reset(){
			CodeList* codeList=Child<CodeList>(CODELIST);
			codeList->Clear();
			codeList->Insert(make_pair(0, wstring(L"Disassembly not loaded")));
			InvalidateRect(codeList->GetHWND(), NULL, false);
		}

		// returns the seg:addr of the cursor selected entry
		uint32 GetCursorSelection(){
			vector<pair<uint32, wstring> >::iterator entry=Child<CodeList>(CODELIST)->GetCurSel();
			return entry->first;
		}

	private:

		static bool m_registered;

		enum Children{
			CODELIST,
			SCROLLBAR
		};

		template<typename T>
		T*& Child(int id){
			return (T*&)m_children[id];
		}

		WinManager m_children;

		void CreateChildren(HWND hWnd);

		uint32 m_baseAddr;

		// Message handlers
#define MSGHANDLER(name) void  On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

		MSGHANDLER(Create);
		MSGHANDLER(VScroll);
		MSGHANDLER(H86UpdateSysData);
		MSGHANDLER(Sizing);

#undef MSGHANDLER

		nsDasm::DasmList m_dasmList;
	};

}

#endif