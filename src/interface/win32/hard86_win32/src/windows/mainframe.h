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

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <map>
#include <vector>

#include "../global.h"
#include "winmanager.h"

#include "codelist.h"
#include "register_watcher.h"
#include "mem_watcher.h"
#include "stack_watcher.h"
#include "bp_list.h"

#include "../../../objwin32/src/gui/frame.h"
#include "../../../objwin32/src/file/file.h"

using namespace nsObjWin32::nsGUI;
using namespace nsObjWin32::nsFiles;

namespace nsHard86Win32{

class MainFrame : public Frame, public StickyWindow{

	static const int ANIMATION_TIMER=1;

public:
	MainFrame() : m_memWatchers(0){
		m_className=L"MainFrame";
		m_style^=(WS_SIZEBOX | WS_MAXIMIZEBOX);
	}

	virtual ~MainFrame(){DestroyWindow(m_hWnd);}

	HWND Create(int w, int h, int x, int y);

	ATOM Register();

	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:

	// Message handlers
#define MSGHANDLER(name) void  On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Create);
	MSGHANDLER(Sizing);
	MSGHANDLER(Moving);
	MSGHANDLER(NCLButtonDown);
	MSGHANDLER(Command);
	MSGHANDLER(H86UpdateSysData);

#undef MSGHANDLER

	void LoadFile();
	void LoadFileToEmulator(const wstring& path, const wstring& fasPath);

	void LoadProject();
	void LoadProjectToFrontend(const wstring& path);

	void CreateChildren(HWND hWnd);

	template<typename T>
	T*& Child(int id){
		return (T*&)m_children[id];
	}

	enum Children{ 
		DASMVIEW,
		STATUSBAR
	};

	HWND m_hwndToolbar;
	int m_toolbarHeight;
	static const int NUM_TOOLBAR_BUTTONS=5;

	WinManager m_children;

	// Tool Windows
	RegisterWatcher m_regWatcher;
	std::vector<MemoryWatcher> m_memWatchers;
	StackWatcher m_stackWatcher;
	BPList m_bpList;

private:
};

}

#endif