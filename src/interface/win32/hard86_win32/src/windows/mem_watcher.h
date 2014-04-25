#ifndef HARD86_MEM_WATCHER_H
#define HARD86_MEM_WATCHER_H

#include <Windows.h>
#include "../global.h"
#include "toolwindow.h"
#include "hexgrid.h"

namespace nsHard86Win32{

using namespace nsObjWin32::nsGUI;

class MemoryWatcher : public Hard86ToolWindow{
public:
	MemoryWatcher() : Hard86ToolWindow(m_registered){
		m_style|=WS_SIZEBOX;
	}

	virtual ~MemoryWatcher(){
		DestroyWindow(m_hWnd);
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND Create(int x, int y, int w, int h, HWND hwndParent, HMENU hMenu=NULL){
		Window::Create(m_exStyle, L"Memory", m_style, x, y, w, h, hwndParent, hMenu);
		Show();
		return m_hWnd;
	}
private:

	// Message handlers
#define MSGHANDLER(name) void On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Create);

#undef MSGHANDLER

	void CreateChildren(HWND hWnd);

	enum Children{
		MEMGRID
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