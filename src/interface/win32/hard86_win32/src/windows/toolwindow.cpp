#include "toolwindow.h"

namespace nsHard86Win32{
	
#define MSGHANDLER(name) LRESULT CALLBACK Hard86ToolWindow::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

	MSGHANDLER(Sizing){
		StickyWindow::OnSizing(hWnd, uMsg, wParam, lParam);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	MSGHANDLER(Moving){
		StickyWindow::OnMoving(hWnd, uMsg, wParam, lParam);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	MSGHANDLER(NCLButtonDown){
		StickyWindow::OnNCLButtonDown(hWnd, uMsg, wParam, lParam);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

#undef MSGHANDLER

}