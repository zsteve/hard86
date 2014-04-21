#ifndef HARD86_MEM_WATCHER_H
#define HARD86_MEM_WATCHER_H

#include <Windows.h>
#include "../global.h"
#include "toolwindow.h"

namespace nsHard86Win32{

using namespace nsObjWin32::nsGUI;

class MemoryWatcher : public Hard86ToolWindow{
public:
	MemoryWatcher() : Hard86ToolWindow(m_registered){

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
	static bool m_registered;
};

}

#endif