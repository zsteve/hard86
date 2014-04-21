#ifndef MAINFRAME_H
#define MAINFRAME_H

#include "../../../objwin32/src/gui/frame.h"
#include "winmanager.h"
#include <map>
#include <vector>

#include "codelist.h"

#include "register_watcher.h"
#include "mem_watcher.h"

using namespace nsObjWin32::nsGUI;

namespace nsHard86Win32{

class MainFrame : public Frame, public StickyWindow{
public:
	MainFrame() : m_memWatchers(0){
		m_className=L"MainFrame";
	}

	virtual ~MainFrame(){DestroyWindow(m_hWnd);}

	HWND Create(int w, int h, int x, int y);

	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:

	// Message handlers
#define MSGHANDLER(name) LRESULT CALLBACK On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Create);
	MSGHANDLER(Sizing);
	MSGHANDLER(Moving);
	MSGHANDLER(NCLButtonDown);

#undef MSGHANDLER

	void CreateChildren(HWND hWnd);

	template<typename T>
	T*& Child(int id){
		return (T*&)m_children[id];
	}

	enum Children{ 
		DASM_CODE_LIST
	};

	WinManager m_children;

	// Tool Windows
	RegisterWatcher m_regWatcher;
	std::vector<MemoryWatcher> m_memWatchers;

private:
};

}

#endif