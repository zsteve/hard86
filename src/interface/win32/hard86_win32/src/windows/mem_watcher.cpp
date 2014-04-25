#include "mem_watcher.h"
#include "../../../objwin32/src/gui/global.h"

namespace nsHard86Win32{

using namespace nsObjWin32::nsGlobal;

bool MemoryWatcher::m_registered=false;

LRESULT CALLBACK MemoryWatcher::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg){
	case WM_CREATE:
		{
			m_dragManager.AddWindow(m_hWnd);
			OnCreate(hWnd, uMsg, wParam, lParam);
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
	case WM_SIZE:
		Child<HexGrid>(MEMGRID)->SendMessage(WM_SIZE, wParam, lParam);
		break;
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

#define MSGHANDLER(name) void MemoryWatcher::On##name(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)

MSGHANDLER(Create){
	CreateChildren(hWnd);
}

#undef MSGHANDLER

void MemoryWatcher::CreateChildren(HWND hWnd){
	HexGrid*& memGrid=Child<HexGrid>(MEMGRID);
	memGrid=new HexGrid;
	memGrid->Create(0, 0, ClientWidth(hWnd), ClientHeight(hWnd), hWnd, MEMGRID);
}

}