#include "mem_watcher.h"

namespace nsHard86Win32{

bool MemoryWatcher::m_registered=false;

LRESULT CALLBACK MemoryWatcher::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg){
	case WM_CREATE:
		{
			m_dragManager.AddWindow(m_hWnd);
			break;
		}
	case WM_MOVING:
		{
			Hard86ToolWindow::OnMoving(hWnd, uMsg, wParam, lParam);
			break;
		}
	case WM_SIZING:
		{
			Hard86ToolWindow::OnSizing(hWnd, uMsg, wParam, lParam);
			break;
		}
	case WM_NCLBUTTONDOWN:
		{
			Hard86ToolWindow::OnNCLButtonDown(hWnd, uMsg, wParam, lParam);
			break;
		}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

}